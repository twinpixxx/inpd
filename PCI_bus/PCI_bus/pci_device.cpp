#include "pci_device.h"
#include <regex>

cPci_device::cPci_device() {}

cPci_device::~cPci_device()
{
	this->vendorID.erase(this->vendorID.begin(), this->vendorID.end());
	this->deviceID.erase(this->deviceID.begin(), this->deviceID.end());
	this->deviceMFG.erase(this->deviceMFG.begin(), this->deviceMFG.end());
}

void cPci_device::add(std::string props_buffer, std::string MFG)
{
    std::string id;

    std::regex regVENID = std::regex(std::string("VEN_.{5}"));
    std::smatch match;
    std::regex_search(props_buffer, match, regVENID);

    id = std::string(match[0]).substr(4, 4);

    std::transform(id.begin(), id.end(), id.begin(), ::tolower);

    this->setVendorID(id);



    std::regex regDEVID = std::regex(std::string("DEV_.{5}"));
    std::regex_search(props_buffer, match, regDEVID);

    id = std::string(match[0]).substr(4, 4);

    std::transform(id.begin(), id.end(), id.begin(), ::tolower);

    this->setDeviceID(id);


    this->setDeviceMFG(MFG);
}