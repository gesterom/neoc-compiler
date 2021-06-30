#pragma once
#include <iostream>

#include "Structs.h"

namespace Errors {
	class Error {
	public:
		Position position;
		bool isTerminal = false;
		std::string mesage;
		Error() {}
		std::string formater(Position pos, std::string msg) {
			return pos.toString() + " " + msg;
		}
		virtual std::string getMesage() { return this->mesage; }
	};
	class UnexpectedEndOfFile : public Error {
	public:
		UnexpectedEndOfFile(Position pos) {
			this->position = pos;
			this->mesage = formater(pos, "Unexpected end of file.");
		}
	};
	class UnexpectedToken : public Error {
	public:
		UnexpectedToken(Token::TokenType expected,Token found) {
			this->position = found.position;
			this->mesage = formater(this->position, std::string("Unexpected token [") + found.toString() + "] expect : [" + TokenTypeToString[(int)expected] +"]" );
		}
		UnexpectedToken(Token::TokenType type, std::string expected, Token found) {
			this->position = found.position;
			this->mesage = formater(this->position, std::string("Unexpected token [") + found.toString() + "] expect : [" + TokenTypeToString[(int)type] + "," + expected + "]");
		}
	};
	class LanguageNotAllowUTF8Character : public Error {
	public:
		LanguageNotAllowUTF8Character(Position pos) {
			this->position = pos;
			this->mesage = formater(pos, "Language do not allow UTF8 characters.");
		}
	};
	class UnendedLiteral : public Error {
	public:
		UnendedLiteral(Position pos) {
			this->position = pos;
			this->mesage = formater(pos, "UnendedLiteral");
		}
	};
	class UnknownError : public Error {
	public:
		UnknownError(int64_t errorCode, Position pos) {
			this->position = pos;
			this->mesage = formater(pos, std::string("Unknown Error {\t") + std::to_string(errorCode) + "}");
		}
	};
	class CombineError : public Error {
	public:
		CombineError(Error f, Error s) {
			this->position = f.position;
			this->mesage = f.getMesage() + "\n" + s.getMesage();
		}
	};
};

std::ostream& operator<<(std::ostream& out, Errors::Error er);