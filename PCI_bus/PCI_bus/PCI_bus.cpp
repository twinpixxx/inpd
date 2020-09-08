#pragma comment(lib, "setupapi.lib")

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <SetupAPI.h>
#include <iostream>
#include <list>
#include <string>
#include <regex>
#include "pci_device.h"

int main() {

    // get device info handler for PCI enumerator
    HDEVINFO DeviceInfoSetHandler = SetupDiGetClassDevs(
        NULL,  // Class GUID
        "PCI", // Enumerator
        NULL,  // hwndParent
        DIGCF_ALLCLASSES | DIGCF_PRESENT); // Flags

    if (DeviceInfoSetHandler == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error while loading device info" << std::endl;
        return 1;
    }


    SP_DEVINFO_DATA DeviceInfoData;
    ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    std::list<cPci_device> deviceList;

    size_t DeviceIndex = 0;
    char props_buffer[512];
    char MFG[512];


    while (SetupDiEnumDeviceInfo(
        DeviceInfoSetHandler,
        DeviceIndex,
        &DeviceInfoData)) {


        DeviceIndex++;

        // Get SPDRP_HARDWAREID:
        // REG_MULTI_SZ string that contains the list of hardware IDs for a device. 
        SetupDiGetDeviceRegistryProperty(
            DeviceInfoSetHandler,		// Device Info Set
            &DeviceInfoData,			// Device Info Data
            SPDRP_HARDWAREID,			// Propetry	
            NULL,						// Property Reg Data Type
            (BYTE*)props_buffer,		// Property Buffer
            512,						// Property Buffer Size 
            NULL);						// Required Size

        // Get SPDRP_MFG:
        // REG_SZ string that contains the name of the device manufacturer. 
        SetupDiGetDeviceRegistryProperty(
            DeviceInfoSetHandler,
            &DeviceInfoData,
            SPDRP_MFG, 
            NULL, 
            (BYTE*)MFG,
            512, 
            NULL);


        cPci_device device;
        device.add(props_buffer, MFG);
        deviceList.push_back(device);
        

        device.~cPci_device();
    }
    std::cout << "number of devices:" << deviceList.size() << std::endl;
    
    std::list<cPci_device>::iterator it;
    for (it = deviceList.begin(); it != deviceList.end(); it++)
    {
        std::cout << "VendorID: "<< it->getVendorID() << " Device ID: "
            << it->getDeviceID() << " DeviceMFG: " << it->getDeviceMFG() << std::endl;
        std::cout << "\n\n";
    }

    SetupDiDestroyDeviceInfoList(DeviceInfoSetHandler);

    return 0;
}
