#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <ntddscsi.h>
#include <string>

using namespace std;

#define bThousand 1024
#define Hundred 100
#define BYTE_SIZE 8

const char* busType[] = { "UNKNOWN", "SCSI", "ATAPI", "ATA", "ONE_TREE_NINE_FOUR",
							"SSA", "FIBRE", "USB", "RAID", "ISCSI", "SAS", "SATA", "SD", "MMC" };

void GetSize(HANDLE dhandle, int counter)
{
	_ULARGE_INTEGER volumeFreeSpace;
	_ULARGE_INTEGER volumeSpace;
	_ULARGE_INTEGER diskFreeSpace;
	volumeFreeSpace.QuadPart = volumeSpace.QuadPart = diskFreeSpace.QuadPart = 0;
	string path = "\\\\.\\";
	HANDLE volumeHandle = INVALID_HANDLE_VALUE;
	DISK_GEOMETRY diskGeometry = { 0 };
	DWORD bytesReturned = 0;
	STORAGE_DEVICE_NUMBER stdn;
	STORAGE_DEVICE_NUMBER* deviceNumber;
	char* local = new char[BUFSIZ];
	memset(local, 0, BUFSIZ);
	int n;
	string resultpath;
	char symbl;

	// Запрашиваем сведения о геометрии диска, на котором расположен раздел.
	if (!DeviceIoControl(dhandle, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &diskGeometry, sizeof(DISK_GEOMETRY), &bytesReturned, (LPOVERLAPPED)NULL))
	{
		cout << "Error " << GetLastError() << endl;
	}
	long long driveSize = ((diskGeometry.Cylinders.QuadPart) * diskGeometry.TracksPerCylinder * diskGeometry.SectorsPerTrack * diskGeometry.BytesPerSector);

	
	DWORD logicalDrivesMask = GetLogicalDrives();
	for (int i = 0; i < 26; i++)
	{
		n = ((logicalDrivesMask >> i) & 0x00000001);
		if (n == 1)
		{
			symbl = char(65 + i);
			resultpath = path + symbl + ":";
			string secondpath;
			secondpath = symbl;
			secondpath.append(":\\");
			if ((volumeHandle = CreateFileA(resultpath.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
			{
				cout << "Error: " << GetLastError() << endl;
			}
			if (!DeviceIoControl(volumeHandle, IOCTL_STORAGE_GET_DEVICE_NUMBER, &stdn, sizeof(STORAGE_DEVICE_NUMBER), local, BUFSIZ, &bytesReturned, (LPOVERLAPPED)NULL))
			{
				cout << "Error" << GetLastError() << endl;
				CloseHandle(volumeHandle);
			}
			deviceNumber = (STORAGE_DEVICE_NUMBER*)local;
			if (counter == deviceNumber->DeviceNumber)
			{
				if (GetDiskFreeSpaceEx(secondpath.c_str(), 0, &volumeSpace, &volumeFreeSpace))
				{
					diskFreeSpace.QuadPart += volumeFreeSpace.QuadPart;
				}
				else
				{
					CloseHandle(volumeHandle);
				}
			}
			CloseHandle(volumeHandle);
		}
	}
	cout.setf(ios::left);
	cout
		<< setw(16) << "Total space[Gb]"
		<< setw(16) << "Free space[Gb]"
		<< setw(16) << "Busy space[Gb]" << endl;

	cout
		<< setw(16) << (driveSize / 1048576 / 1024)
		<< setw(16) << (diskFreeSpace.QuadPart / 1048576 / 1024)
		<< setw(16) << ((driveSize - diskFreeSpace.QuadPart) / 1048576 / 1024)
		<< endl;
}

void getDeviceInfo(HANDLE diskHandle, STORAGE_PROPERTY_QUERY storageProtertyQuery) {
	STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)calloc(bThousand, 1); //Used to retrieve the storage device descriptor data for a device.
	deviceDescriptor->Size = bThousand;

	//Sends a control code directly to a specified device driver
	if (!DeviceIoControl(diskHandle, IOCTL_STORAGE_QUERY_PROPERTY, &storageProtertyQuery, sizeof(storageProtertyQuery), deviceDescriptor, bThousand, NULL, 0)) {
		printf("%d", GetLastError());
		CloseHandle(diskHandle);
		exit(-1);
	}

	cout << "Product ID:    " << (char*)(deviceDescriptor)+deviceDescriptor->ProductIdOffset << endl;
	cout << "Version        " << (char*)(deviceDescriptor)+deviceDescriptor->ProductRevisionOffset << endl;
	cout << "Bus type:      " << busType[deviceDescriptor->BusType] << endl;
	cout << "Serial number: " << (char*)(deviceDescriptor)+deviceDescriptor->SerialNumberOffset << endl;
}

void getMemoryTransferMode(HANDLE diskHandle, STORAGE_PROPERTY_QUERY storageProtertyQuery) {
	STORAGE_ADAPTER_DESCRIPTOR adapterDescriptor;
	if (!DeviceIoControl(diskHandle, IOCTL_STORAGE_QUERY_PROPERTY, &storageProtertyQuery, sizeof(storageProtertyQuery), &adapterDescriptor, sizeof(STORAGE_DESCRIPTOR_HEADER), NULL, NULL)) {
		cout << GetLastError();
		exit(-1);
	}
	else {
		cout << "Transfer mode: ";
		adapterDescriptor.AdapterUsesPio ? cout << "DMA" : cout << "PIO";
		cout << endl;
	}
}

void getAtaSupportStandarts(HANDLE diskHandle) {

	UCHAR identifyDataBuffer[512 + sizeof(ATA_PASS_THROUGH_EX)] = { 0 };

	ATA_PASS_THROUGH_EX& PTE = *(ATA_PASS_THROUGH_EX*)identifyDataBuffer;	//Структура для отправки АТА команды устройству
	PTE.Length = sizeof(PTE);
	PTE.TimeOutValue = 10;									//Размер структуры
	PTE.DataTransferLength = 512;							//Размер буфера для данных
	PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);		//Смещение в байтах от начала структуры до буфера данных
	PTE.AtaFlags = ATA_FLAGS_DATA_IN;						//Флаг, говорящий о чтении байтов из устройства

	IDEREGS* ideRegs = (IDEREGS*)PTE.CurrentTaskFile;
	ideRegs->bCommandReg = 0xEC;

	//Производим запрос устройству
	if (!DeviceIoControl(diskHandle,
		IOCTL_ATA_PASS_THROUGH,								//Флаг, говорящий что мы посылаем структуру с командами типа ATA_PASS_THROUGH_EX
		&PTE, sizeof(identifyDataBuffer), &PTE, sizeof(identifyDataBuffer), NULL, NULL)) {
		cout << GetLastError() << std::endl;
		return;
	}
	WORD* data = (WORD*)(identifyDataBuffer + sizeof(ATA_PASS_THROUGH_EX));	//Получаем указатель на массив полученных данных
	short ataSupportByte = data[80];
	int i = 2 * BYTE_SIZE;
	int bitArray[2 * BYTE_SIZE];
	//Превращаем байты с информацией о поддержке ATA в массив бит
	while (i--) {
		bitArray[i] = ataSupportByte & 32768 ? 1 : 0;
		ataSupportByte = ataSupportByte << 1;
	}

	//Анализируем полученный массив бит.
	cout << "ATA Support:   ";
	for (int i = 8; i >= 4; i--) {
		if (bitArray[i] == 1) {
			cout << "ATA" << i;
			if (i != 4) {
				cout << ", ";
			}
		}
	}
	cout << endl;

	//Вывод поддерживаемых режимов DMA
	unsigned short dmaSupportedBytes = data[63];
	int i2 = 2 * BYTE_SIZE;
	//Превращаем байты с информацией о поддержке DMA в массив бит
	while (i2--) {
		bitArray[i2] = dmaSupportedBytes & 32768 ? 1 : 0;
		dmaSupportedBytes = dmaSupportedBytes << 1;
	}

	//Анализируем полученный массив бит.
	cout << "DMA Support:   ";
	for (int i = 0; i < 8; i++) {
		if (bitArray[i] == 1) {
			cout << "DMA" << i;
			if (i != 2) cout << ", ";
		}
	}
	cout << endl;

	unsigned short pioSupportedBytes = data[63];
	int i3 = 2 * BYTE_SIZE;
	//Превращаем байты с информацией о поддержке PIO в массив бит
	while (i3--) {
		bitArray[i3] = pioSupportedBytes & 32768 ? 1 : 0;
		pioSupportedBytes = pioSupportedBytes << 1;
	}

	//Анализируем полученный массив бит.
	cout << "PIO Support:   ";
	for (int i = 0; i < 2; i++) {
		if (bitArray[i] == 1) {
			cout << "PIO" << i + 3;
			if (i != 1) cout << ", ";
		}
	}
	cout << endl;
}

int main() {

	int counter = 0;

	STORAGE_PROPERTY_QUERY storagePropertyQuery; //properties query of a storage device or adapter
	storagePropertyQuery.QueryType = PropertyStandardQuery; // flags indicating the type of query 
	storagePropertyQuery.PropertyId = StorageDeviceProperty; // Indicates whether the caller is requesting a device descriptor

	while (true)
	{
		string path = "\\\\.\\PhysicalDrive" + to_string(counter);

		HANDLE diskHandle = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		if (diskHandle == INVALID_HANDLE_VALUE) {
			if (GetLastError() == 5)
			{
				cout << "Access denied" << endl;
			}
			else if (GetLastError() == 2) {
				cout << "Can not get disk handle" << endl;
			}
			system("pause");
			return -1;
		}

		getDeviceInfo(diskHandle, storagePropertyQuery);
		getMemoryTransferMode(diskHandle, storagePropertyQuery);
		getAtaSupportStandarts(diskHandle);
		GetSize(diskHandle, 0);

		cout << "----------------------------------------" << endl;

		CloseHandle(diskHandle);

		counter++;
	}


	system("pause");
	return 0;
}