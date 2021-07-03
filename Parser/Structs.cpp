// Parser.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "Structs.h"

// TODO: This is an example of a library function
namespace Parser {
	Position::Position() {
		this->column = -1;
		this->line = -1;
		this->filename = "";
	}
	Position::Position(int64_t line, int64_t column, std::string filename) {
		this->column = column;
		this->line = line;
		this->filename = filename;
	}
	std::string Position::toString() const {
		return std::string("[") + filename + ":\t" + std::to_string(line) + " . " + std::to_string(column) + std::string("]");
	}

	Character::Character() {
		this->value = -1;
		this->position = Position();
	}
	Character::Character(int8_t ch, Position pos) {
		this->value = ch;
		this->position = pos;
	}
	static std::vector<std::string> TokenTypeToString = {
		"unInitValue",
		"stringLiteral",
		"charLiteral",
		"integerLiteral",
		"floatLiteral",
		"operatorSymbol",
		"unaryOperatorSymbol",
		"newlineSymbol",
		"semicolonSymbol",
		"parentheses",
		"namespaceAcces",
		"preamble",
		"keyword",
		"TypeId",
		"Id",
		"Whitespace",
		"singielComment",
		"multiComment",
	};

	Token::Token() {
		this->position = Position();
		this->value = "";
		this->type = TokenType::unInitValue;
	}
	Token::Token(std::string v, TokenType t, Position p) {
		this->value = v;
		this->position = p;
		this->type = t;
	}
	std::string Token::toString() {
		return this->position.toString() + " { " + this->value + " } ( (" + std::to_string((int)this->type) + ") " + TokenTypeToString[(int)this->type] + " )";
	}


	std::ostream& operator<<(std::ostream& out, Character e) {
		out << e.position << " > " << e.value;

		return out;
	}
	std::ostream& operator<<(std::ostream& out, Token t) {

		out << t.toString();

		return out;
	}
	std::ostream& operator<<(std::ostream& out, Position t) {
		out << t.toString();
		return out;
	}
}