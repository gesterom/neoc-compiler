#pragma once
#include <string>
#include <vector>

#define debugLine() do{std::cout<<"["<<__FILE__<<"] : {"<<__LINE__<<"}  "<<__FUNCSIG__  <<std::endl;}while(false)
#define debugVar(var) do{std::cout<<"VAR : "<<(#var)<<" { "<< (var) <<" }" <<std::endl;}while(false)
#define debugBool(var) do{std::cout<<"VAR : "<<(#var)<<" { "<< ((var)?"True":"False") <<" }" <<std::endl;}while(false)
#define debug(var) do{std::cout<<"["<<__FILE__<<"] : {"<<__LINE__<<"}  "<<(var)<<std::endl;}while(false)

struct Position {
	int line;
	int column;
	std::string filename;
	Position() {
		this->column = -1;
		this->line = -1;
		this->filename = "";
	}
	std::string toString() const {
		return std::string("[") + filename + ":\t" + std::to_string(line) + " . " + std::to_string(column) + std::string("]");
	}
};
struct Character {
	int8_t value;
	Position position;
	Character() {
		this->value = -1;
		this->position = Position();
	}
	Character(int8_t ch, Position pos) {
		this->value = ch;
		this->position = pos;
	}
};

static std::vector<std::string> TokenTypeToString = {
	"unInitValue",
	"stringLiteral",
	"charLiteral",
	"integerLiteral",
	"floatLiteral",
	"operatorSymbol",
	"newlineSymbol",
	"parentheses",
	"namespaceAcces",
	"preamble",
	"keyword",
	"TypeId",
	"Id",
	"Whitespace",
};
struct Token {
	enum class TokenType {
		unInitValue = 0,
		stringLiteral,
		charLiteral,
		integerLiteral,
		floatLiteral,
		operatorSymbol,
		newlineSymbol,
		parentheses,
		namespaceAcces,
		preamble,
		keyword,
		TypeId,
		Id,
		Whitespace,
	};
	Position position;
	std::string value;
	TokenType type;
	Token() {
		this->position = Position();
		this->value = "";
		this->type = TokenType::unInitValue;
	}
	Token(std::string v, TokenType t, Position p) {
		this->value = v;
		this->position = p;
		this->type = t;
	}
	std::string toString() {
		return this->position.toString() + " { " + this->value + " } (" + TokenTypeToString[(int)this->type] + ")";
	}
};


std::ostream& operator<<(std::ostream& out, Character e);
std::ostream& operator<<(std::ostream& out, Token t);
std::ostream& operator<<(std::ostream& out, bool t);