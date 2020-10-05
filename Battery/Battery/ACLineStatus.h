#pragma once

enum ACLineStatus {
	Offline = 0,
	Online = 1,
	UnknownStatus = 255
};


std::ostream& operator<<(std::ostream& lhs, ACLineStatus e) {
	switch (e) {
	case Online: lhs << "Online"; break;
	case Offline: lhs << "Offline"; break;
	case UnknownStatus: lhs << "Unknown"; break;
	}
	return lhs;
}