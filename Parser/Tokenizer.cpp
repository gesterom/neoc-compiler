#include "pch.h"
#include "Tokenizer.h"
namespace Parser {
	Tokenizer::expectTokenRes::expectTokenRes() {
		this->matched = false;
		this->token = Token();
		this->consumed = 0;
	}
	Tokenizer::expectTokenRes::expectTokenRes(Token token) {
		this->matched = true;
		this->token = token;
		this->consumed = token.value.size();
	}
	Tokenizer::expectTokenRes::expectTokenRes(Token token, int consumed) {
		this->matched = true;
		this->token = token;
		this->consumed = consumed;
	}
	std::ostream& operator<<(std::ostream& out, Tokenizer::expectTokenRes e) {
		out << "\t>>> " << (e.matched ? "[T]" : "[F]") << e.token;
		return out;
	}
	typedef std::function< Result<Tokenizer::expectTokenRes>(const CharacterStream cs, std::string keyword, Token::TokenType type)> expectMatch;
	Result<Tokenizer::expectTokenRes> expectKeyword(const CharacterStream cs, std::string keyword, Token::TokenType type) {
		return cs.getValues(keyword.size() + 1)
			.execute<Tokenizer::expectTokenRes>([cs, keyword, type](std::vector<Character> vec) {
			for (int i = 0;i < keyword.size();i++) {
				if (keyword[i] != vec[i].value)
					return Tokenizer::expectTokenRes();
			}
			if (not LexerRules::identyfireProhibitedCharacter(vec[keyword.size()].value)) {
				return Tokenizer::expectTokenRes();
			}
			return Tokenizer::expectTokenRes{ Token{keyword, type, cs.getCursor()} };
		}
		).solveError([]() {return Tokenizer::expectTokenRes();});
	}

	Result<Tokenizer::expectTokenRes> expectOperator(const CharacterStream cs, std::string keyword, Token::TokenType type) {
		return cs.getValues(keyword.size())
			.execute<Tokenizer::expectTokenRes>(
				[cs, keyword, type](std::vector<Character> vec) {
			for (int i = 0;i < keyword.size();i++) {
				if (keyword[i] != vec[i].value)
					return Tokenizer::expectTokenRes();
			}
			return Tokenizer::expectTokenRes(Token(keyword, type, cs.getCursor()));
		}
		).solveError([]() -> Tokenizer::expectTokenRes {return Tokenizer::expectTokenRes();});
	}

	Result<Tokenizer::expectTokenRes> expectLiteral(const CharacterStream cs, LexerRules::LiteralDefiniton literal) {
		auto a = cs.getValue();
		if (a.isError()) return a.carryError<Tokenizer::expectTokenRes>();
		auto ch = a.unpack();
		if (literal.start(ch.value)) {
			std::string tokenValue;
			if (literal.consumeStarting)
				tokenValue += (const char)ch.value;
			for (int i = 1;true;i++) {
				auto b = cs.getValue(i);
				if (b.isError()) {
					if(literal.consumeStoping && literal.skipStoping)
						return Result<Tokenizer::expectTokenRes>(Errors::UnendedLiteral(cs.getCursor()));
					else { break; }
				}
				if (not literal.stop(b.unpack()) or literal.consumeStoping) {
					tokenValue += (char)b.unpack();
				}
				if (literal.stop(b.unpack())) break;
			}
			int padding = 0;
			if (literal.consumeStoping == false and literal.skipStoping == true) {
				padding = 1;
			}

			return Result<Tokenizer::expectTokenRes>{Tokenizer::expectTokenRes{ Token(tokenValue, literal.type, cs.getCursor()), (int)(tokenValue.size() % (1024 * 1024)) + padding }};
		}
		return Result<Tokenizer::expectTokenRes>(Tokenizer::expectTokenRes());
	}

	Result < Tokenizer::expectTokenRes > carryToken(Tokenizer::expectTokenRes a, Tokenizer::expectTokenRes b) {
		if (a.matched == true and b.matched == true)
			return Result<Tokenizer::expectTokenRes>(Errors::UnknownError(1, a.token.position));
		if (a.matched == true)
			return Result< Tokenizer::expectTokenRes >(a);
		if (b.matched == true)
			return Result< Tokenizer::expectTokenRes >(b);
		else
			return Result< Tokenizer::expectTokenRes >(b);
	}

	Result < Tokenizer::expectTokenRes >  ignoreLiterals(Tokenizer::expectTokenRes a, Tokenizer::expectTokenRes b) {
		if (a.matched == true and b.matched == true)
			return Result< Tokenizer::expectTokenRes >(b);
		if (a.matched == true)
			return Result< Tokenizer::expectTokenRes >(a);
		if (b.matched == true)
			return Result< Tokenizer::expectTokenRes >(b);
		else
			return Result< Tokenizer::expectTokenRes >(a);
	}

	Result<Token> Tokenizer::getToken(CharacterStream& cs) {


		Result< expectTokenRes > res{ expectTokenRes() };
		int counter = 0;

		auto perfectMatch = [](Parser::CharacterStream& cs, expectMatch func ,std::vector<std::string> vec, Token::TokenType type,Result<expectTokenRes> res) {
			for (auto item : vec) {
				res = func(cs, item, type)
					.flatCombine<Tokenizer::expectTokenRes,Tokenizer::expectTokenRes>(carryToken,res);
			}
			return res;
		};
		res;
		res = perfectMatch(cs, expectKeyword,	LexerRules::preambleList,				Token::TokenType::preamble,					res);

		res;
		res = perfectMatch(cs, expectKeyword,	LexerRules::keywordList,				Token::TokenType::keyword,					res);
		res = perfectMatch(cs, expectOperator,	LexerRules::operatorList,				Token::TokenType::operatorSymbol,			res);
		res = perfectMatch(cs, expectOperator,	LexerRules::unaryOperatorList,			Token::TokenType::unaryOperatorSymbol,		res);
		res = perfectMatch(cs, expectOperator,	LexerRules::parenthesesList,			Token::TokenType::parentheses,				res);
		res = perfectMatch(cs, expectOperator,	LexerRules::singielCommentsSymbols,		Token::TokenType::singielComment,			res);
		res = perfectMatch(cs, expectOperator,	LexerRules::multiCommentsSymbols,		Token::TokenType::multiComment,				res);

		for (auto item : LexerRules::literalsList) {
			res = expectLiteral(cs, item)
				.flatSelect(res, ignoreLiterals);
		}

		auto a = res.execute<Token>([&cs](expectTokenRes v) {
			if (v.matched == false) {
				debug("Compiler Internal Error");
				exit(1);
			}
			cs.popValue(v.consumed);
			return v.token;
		});

		return a;
	}

	void standerizedWhitespace(TokenStream& ts) {
		ts.modify([](Token& t) {
			if (t.type == Token::TokenType::Whitespace) {
				t.value = " ";
			}
		});
	}

	void removeSingielComments(TokenStream& ts) {
		bool commentActive = false;
		ts.remove([&commentActive](Token t) {
			if (t.type == Token::TokenType::singielComment)
				commentActive = true;
			if (t.type == Token::TokenType::newlineSymbol)
				commentActive = false;
			return commentActive;
		});
	}

	Result<TokenStream> removeMultiLineComments(TokenStream& ts) {
		int commentActive = 0;
		Token lastUP;
		Token lastDown;
		ts.remove([&commentActive,&lastUP,&lastDown](Token t) -> bool{
			if (t.type == Token::TokenType::multiComment and t.value == "/*") {
				commentActive++;
				lastUP = t;
			}
			if (t.type == Token::TokenType::multiComment and t.value == "*/") {
				commentActive--;
				if (commentActive < 0) {
					lastDown = t;
				}
			}
			return commentActive > 0;
		});
		if (commentActive > 0) {
			return Result<TokenStream>(Errors::UnexpectedToken(lastUP));
		}
		if (commentActive < 0){
			return Result<TokenStream>(Errors::UnexpectedToken(lastDown));
		}
		return Result<TokenStream>(ts);
	}

	void combieneNumbersIntoFloats(TokenStream& ts) {

		auto numberPredictor = [](Token t) {return t.type == Token::TokenType::integerLiteral;};
		auto piriodPredicotr = [](Token t) {return t.type == Token::TokenType::operatorSymbol and t.value == ".";};

		auto floatProducer = [](std::vector<Token> vec) -> Token {
			Token res;
			res.value = vec[0].value + vec[1].value + vec[2].value;
			res.type = Token::TokenType::floatLiteral;
			res.position = vec[0].position;
			return res;
		};

		ts.colapsTokensIntoOne({ numberPredictor,piriodPredicotr,numberPredictor }, floatProducer);
	}

	auto replaceToken(Token::TokenType typeToReplace, std::string tokenValueToReplace, Token::TokenType newType, std::string newValue) {
		return [typeToReplace, tokenValueToReplace, newType, newValue](TokenStream& ts) {
			ts.modify([typeToReplace, tokenValueToReplace, newType, newValue](Token& t) {
				if (t.type == typeToReplace and t.value == tokenValueToReplace) {
					t.value = newValue;
					t.type = newType;
				}
			});
		};
	}

	void combieneIntoNamsepace(TokenStream& ts) {

		auto namespaceSymbol = [](Token t) {return t.type == Token::TokenType::operatorSymbol and t.value == "::";};
		auto idpredictor = [](Token t) {return t.type == Token::TokenType::Id or t.type == Token::TokenType::TypeId;};

		auto floatProducer = [](std::vector<Token> vec) -> Token {
			Token res;
			res.value = vec[0].value;
			res.type = Token::TokenType::namespaceAcces;
			res.position = vec[0].position;
			return res;
		};

		ts.colapsTokensIntoOne({ idpredictor,namespaceSymbol }, floatProducer);
	}

	Result<TokenStream> Tokenizer::tokenize(CharacterStream cs) {
		std::vector<Token> res;
		bool loop_var = true;
		while (not cs.isEnd(1) and loop_var) {
			getToken(cs).execute<bool>([&res](Token t) {
				res.push_back(t);
				return true;
			}).onError([&loop_var](Errors::Error er) {

				loop_var = false;
				return er;
			});
		}
		TokenStream ts(res);
		standerizedWhitespace(ts);
		replaceToken(Token::TokenType::operatorSymbol, "\n", Token::TokenType::newlineSymbol, "(newLine)")(ts);
		replaceToken(Token::TokenType::operatorSymbol, ";", Token::TokenType::semicolonSymbol, "(;)")(ts);
		combieneNumbersIntoFloats(ts);
		combieneIntoNamsepace(ts);
		removeSingielComments(ts);
		return removeMultiLineComments(ts);
	}
}