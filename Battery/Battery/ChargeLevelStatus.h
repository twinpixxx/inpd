#pragma once

enum ChargeLevelStatus {
	High = 0x01,
	Low = 0x02,
	Critical = 0x04,
	Charging = 0x08,
	NoSystemBattery = 128,
	UnknownChargeLevelStatus = 255
};

std::ostream& operator<<(std::ostream& lhs, ChargeLevelStatus e) {
	if (Critical & e) lhs << " Critical ";
	if (Low & e) lhs << " Low ";
	if (High & e) lhs << " High ";
	if (Charging & e) lhs << " Charging ";
	if (NoSystemBattery & e)  lhs << "No System Battery";
	if (UnknownChargeLevelStatus == e) lhs << "Unknown";
	return lhs;
}