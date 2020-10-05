#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <WinIoCtl.h>
#include <ntddscsi.h>
#include <conio.h>

#include "BatteryInfo.h"
#include "BatteryInfoProvider.h"
#include "BatteryLifePercent.h"

using namespace std;


void ShowInfo(BatteryInfo battery) {
	cout << "AC line status:          " << battery.ACStatus << endl;
	cout << "Battery charge level:   " << battery.ChargeLevel << endl;
	cout << "Battery life percent:    " << battery.BatteryLifePercent << endl;
	cout << "Battery saver status:    " << battery.BatterySaver << endl;
}

int main() {
	BatteryInfoProvider provider;

	while (true) {
		auto battery = provider.getBatteryInfo();
		ShowInfo(battery);

		Sleep(500);

		system("cls");
	}

	system("pause");
	return 0;
}