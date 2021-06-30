#include "Errors.h"


std::ostream& operator<<(std::ostream& out, Errors::Error er) {
	out << er.getMesage();
	return out;
}
std::ostream& operator<<(std::ostream& out, Character e) {
	out << e.position.toString() << "\t--> " << e.value;
	return out;
}
std::ostream& operator<<(std::ostream& out, Token t) {
	out << t.toString();
	return out;
}
std::ostream& operator<<(std::ostream& out, bool t){
	out << ((t) ? "TRUE" : "FALSE");
	return out;
}