#pragma once

#include "pch.h"

namespace Parser {

	struct Position {
		int64_t line;
		int64_t column;
		std::string filename;
		Position();
		Position(int64_t line, int64_t column, std::string filename);
		std::string toString() const;
	};
	struct Character {
		int8_t value;
		Position position;
		Character();
		Character(int8_t ch, Position pos);
	};

	extern std::vector<std::string> TokenTypeToString;

	struct Token {
		enum class TokenType {
			unInitValue = 0,
			stringLiteral,
			charLiteral,
			integerLiteral,
			floatLiteral,
			operatorSymbol,
			unaryOperatorSymbol,
			newlineSymbol,
			semicolonSymbol,
			parentheses,
			namespaceAcces,
			preamble,
			keyword,
			TypeId,
			Id,
			Whitespace,
			singielComment,
			multiComment,
		};
		Position position;
		std::string value;
		TokenType type;
		Token();
		Token(std::string v, TokenType t, Position p);
		std::string toString();
	};
	std::ostream& operator<<(std::ostream& out, Character e);
	std::ostream& operator<<(std::ostream& out, Token t);
	std::ostream& operator<<(std::ostream& out, Position t);
}