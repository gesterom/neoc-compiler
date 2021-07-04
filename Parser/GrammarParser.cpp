#include "pch.h"
#include "GrammarParser.h"
namespace Parser {
	namespace GrammarParser {
		GrammarParser::GrammarParser(Parser::TokenStream ts) {
			this->ts = ts;
		}
		SyntaxNodes::File GrammarParser::parseFile() {
			SyntaxNodes::File res;
			while (not ts.isEnd(1)) {
				if (auto a = ts.start().matchProcedureDeclaration().matchStatement().end()) {
					res.procedures.push_back(parseProcedure());
				}
			}
			return res;
		}
		SyntaxNodes::Declaration::Procedure GrammarParser::parseProcedure() {
			SyntaxNodes::Declaration::Procedure res;
			if (auto a = ts.start()
				.match(Token::TokenType::preamble, "procedure")
				.match(Token::TokenType::Id)
				.match(Token::TokenType::parentheses, "(").end()) {
				res.name = a.tokens[1];
				ts.consumeTokens(a);
			}
			return res;
		}
	}
}