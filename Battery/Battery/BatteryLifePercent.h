#pragma once

class BatteryLifePercent {
	int _code = 0;
public:
	BatteryLifePercent() {}

	BatteryLifePercent(int code) {
		this->_code = code;
	}

	int getValue() {
		return _code;
	}

	 friend std::ostream& operator<<(std::ostream& lhs, BatteryLifePercent e) {
		auto value = e.getValue();
		switch (value) {
		case UnknownChargeLevelStatus: lhs << "No system battery"; break;
		default: lhs << value << "%"; break;
		}
		return lhs;
	}
};
