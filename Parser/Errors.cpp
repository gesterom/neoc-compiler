#include "pch.h"

namespace Errors {
	std::ostream& operator<<(std::ostream& out, Errors::Error er) {
		out << er.getMesage() << std::endl;
		return out;
	}
}