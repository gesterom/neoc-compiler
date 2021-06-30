#pragma once

#include <vector>
#include <string>
#include <functional>

#include "Structs.h"

namespace LexerRules {
	bool isWhitespace(int8_t ch);
	bool identyfireProhibitedCharacter(int8_t ch);

	//string-WS
	// keywords
	extern std::vector<std::string> preambleList;
	extern std::vector<std::string> keywordList;
	//operators
	// //
	// +
	// = 
	// ...



	extern std::vector<std::string> operatorList;
	//literals

	// Type - begin from Upercase letters
	// function - begin from lowercase letters
	// @ 
	// # - compiler readable comments

	// "string"
	// 1234
	// 'c'
	// 1234.0

	typedef std::function<bool(int8_t)> paternMatch;
	struct LiteralDefiniton {
		paternMatch start;
		paternMatch stop;
		bool consumeStarting;
		bool consumeStoping;
		bool skipStoping;
		Token::TokenType type;
	};

	namespace Literals {
		bool upercaseMatch(int8_t a);
		bool quoteMatch(int8_t a);
		bool singielQuoteMatch(int8_t a);
		bool lowercaseMatch(int8_t a);
		bool isNotWhitespace(int8_t a);
		bool numberMatch(int8_t a);
		bool notNumberMatch(int8_t a);
		extern LiteralDefiniton Type;
		extern LiteralDefiniton String;
		extern LiteralDefiniton CharLiteral;
		extern LiteralDefiniton Id;
		extern LiteralDefiniton Whitespace;
		extern LiteralDefiniton IntigerNumber;

	}

	extern std::vector<LiteralDefiniton> literalsList;
}