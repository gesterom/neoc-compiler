#pragma once

#include "Structs.h"

namespace Errors {
	class Error {
	public:
		Parser::Position position;
		bool isTerminal = false;
		std::string mesage;
		Error() {}
		std::string formater(Parser::Position pos, std::string msg) {
			return pos.toString() + "Error : " + msg;
		}
		virtual std::string getMesage() { return this->mesage; }
	};
	class InternalError : public Error {
	public:
		InternalError(std::string filename, int lineNumber) {
			this->mesage = std::string("[") + filename + ":" + std::to_string(lineNumber) + "] Internal unexpected error";
		}
	};
	class UnexpectedEndOfFile : public Error {
	public:
		UnexpectedEndOfFile(Parser::Position pos) {
			this->position = pos;
			this->mesage = formater(pos, "Unexpected end of file.");
		}
	};
	class UnexpectedToken : public Error {
	public:
		UnexpectedToken(Parser::Token::TokenType expected, Parser::Token found) {
			this->position = found.position;
			this->mesage = formater(this->position, std::string("Unexpected token [") + found.toString() + "] expect : [" + Parser::TokenTypeToString[(int)expected] + "]");
		}
		UnexpectedToken(Parser::Token::TokenType type, std::string expected, Parser::Token found) {
			this->position = found.position;
			this->mesage = formater(this->position, std::string("Unexpected token [") + found.toString() + "] expect : [" + Parser::TokenTypeToString[(int)type] + "," + expected + "]");
		}
		UnexpectedToken(Parser::Token found) {
			this->position = found.position;
			this->mesage = formater(this->position, std::string("Unexpected token [") + found.toString() + "] ");
		}
	};
	class LanguageNotAllowUTF8Character : public Error {
	public:
		LanguageNotAllowUTF8Character(Parser::Position pos) {
			this->position = pos;
			this->mesage = formater(pos, "Language do not allow UTF8 characters.");
		}
	};
	class UnendedLiteral : public Error {
	public:
		UnendedLiteral(Parser::Position pos) {
			this->position = pos;
			this->mesage = formater(pos, "UnendedLiteral");
		}
	};
	class UnknownError : public Error {
	public:
		UnknownError(int64_t errorCode, Parser::Position pos) {
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
	std::ostream& operator<<(std::ostream& out, Error er);
}