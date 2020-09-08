#pragma once
#include <string>
#include <iostream>

class cPci_device
{
protected:
	std::string vendorID;
	std::string deviceID;
	std::string deviceMFG;

public:
	cPci_device();
	~cPci_device();


	void setVendorID(std::string id) { this->vendorID = id; }
	void setDeviceID(std::string id) { this->deviceID = id; }
	void setDeviceMFG(std::string MFG) { this->deviceMFG = MFG; }

	std::string getVendorID() { return this->vendorID; }
	std::string getDeviceID() { return this->deviceID; }
	std::string getDeviceMFG() { return this->deviceMFG; }

	void add(std::string, std::string);
};