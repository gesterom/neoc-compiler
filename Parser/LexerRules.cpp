#include "pch.h"
#include "LexerRules.h"
namespace Parser {
	namespace LexerRules {
		bool isWhitespace(int8_t ch) {
			return
				ch == ' ' or
				ch == '\n' or
				ch == '\r' or
				ch == '\t';
		}
		bool identyfireProhibitedCharacter(int8_t ch) {
			return
				ch == ' ' or
				ch == '\n' or
				ch == '\r' or
				ch == '\t' or
				ch == '\\' or
				ch == '/' or
				ch == '.' or
				ch == '*' or
				ch == '(' or
				ch == ')' or
				ch == '[' or
				ch == ']' or
				ch == '{' or
				ch == '}' or
				ch == ';' or
				ch == ':' or
				ch == '"' or
				ch == '\'' or
				ch == '=' or
				ch == '+' or
				ch == '-' or
				ch == '<' or
				ch == '>' or
				ch == '?' or
				ch == '%' or
				ch == ',';
		}

		//string-WS
		// keywords
		std::vector<std::string> preambleList = {
			"procedure","type","function","alias","class","test","subcase", // types
			"namespace",
			"send","receive", // OOP
			"call", // function OP
			"not","and","or","xor" // logical operators
		};
		std::vector<std::string> keywordList = { "return","while","if","_","for","switch","lambda","assert","expect" };
		//operators
		// //
		// +
		// = 
		// ...



		std::vector<std::string> operatorList = { "//" ,"\n",";",
												"+","-","*","/","%",
												"<=",">=","==","<",">",
												"=",
												".",",","::" };

		std::vector<std::string> multiCommentsSymbols = { "/*","*/" };
		std::vector<std::string> singielCommentsSymbols = { "//" };

		std::vector<std::string> parenthesesList = { "(",")","[","]","{","}" };
		std::vector<std::string> unaryOperatorList = { "call","not" };
		//literals

		// Type - begin from Upercase letters
		// function - begin from lowercase letters
		// @ 
		// # - compiler readable comments

		// "string"
		// 1234
		// 'c'
		// 1234.0

		namespace Literals {
			bool upercaseMatch(int8_t a) {
				return a >= 'A' and a <= 'Z';
			}
			bool quoteMatch(int8_t a) {
				return '"' == a;
			}
			bool singielQuoteMatch(int8_t a) {
				return '\'' == a;
			}
			bool lowercaseMatch(int8_t a) {
				return a >= 'a' and a <= 'z';
			}
			bool isNotWhitespace(int8_t a) {
				return not isWhitespace(a);
			}
			bool numberMatch(int8_t a) {
				return a >= '0' and a <= '9';
			}
			bool notNumberMatch(int8_t a) {
				return not numberMatch(a);
			}
			LiteralDefiniton Type{ upercaseMatch,identyfireProhibitedCharacter,true,false,false,Token::TokenType::TypeId };
			LiteralDefiniton String{ quoteMatch,quoteMatch,true,true,true,Token::TokenType::stringLiteral };
			LiteralDefiniton CharLiteral{ singielQuoteMatch,singielQuoteMatch,true,true,true,Token::TokenType::charLiteral };
			LiteralDefiniton Id{ lowercaseMatch,identyfireProhibitedCharacter,true,false,false,Token::TokenType::Id };
			LiteralDefiniton Whitespace{ isWhitespace,isNotWhitespace,true,false,false,Token::TokenType::Whitespace };
			LiteralDefiniton IntegerNumber{ numberMatch,notNumberMatch,true,false,false,Token::TokenType::integerLiteral };

		}

		std::vector<LiteralDefiniton> literalsList = { Literals::Type,Literals::String,Literals::Id,Literals::Whitespace,Literals::IntegerNumber,Literals::CharLiteral };
	}
}