#include "pch.h"

std::ostream& operator<<(std::ostream& out, Errors::Error er) {
	out << er.getMesage() << std::endl;
	return out;
}