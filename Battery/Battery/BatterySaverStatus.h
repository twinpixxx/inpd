#pragma once

enum BatterySaverStatus {
	Off = 0,
	On = 1
};

std::ostream& operator<<(std::ostream& lhs, BatterySaverStatus e) {
	switch (e) {
	case Off: lhs << "Off"; break;
	case On: lhs << "On"; break;
	}
	return lhs;
}