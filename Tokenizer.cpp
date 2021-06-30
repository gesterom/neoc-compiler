#include "Tokenizer.h"

#pragma once

#include <vector>

#include "Result.h"
#include "Structs.h"
#include "IReadStream.h"
#include "CharacterStream.h"
#include "LexerRules.h"
#include "Errors.h"

namespace Tokenizer {

	TokenStream::TokenStream() {}
	TokenStream::TokenStream(std::vector<Token> vec) : tab(vec) {}

	bool TokenStream::isEnd(int64_t offset) const {
		assert(offset != 0);
		return (this->index + offset) > tab.size();
	}
	Result<Token> TokenStream::getValue() const {
		if (this->isEnd(1)) return Result<Token>(Errors::UnexpectedEndOfFile(tab[tab.size() - 1].position));

		return Result<Token>{tab[index]};
	}
	Result<Token> TokenStream::getValue(int64_t n) const {
		assert(n < INT64_MAX);
		if (this->isEnd(n + 1)) {
			return Result<Token>(Errors::UnexpectedEndOfFile(tab[tab.size()-1].position));
		}

		Token ch = tab[this->index + n];
		return Result<Token>(ch);
	}
	Result<std::vector<Token>> TokenStream::getValues(int numberOfElementsToGet) const {
		if (this->isEnd(numberOfElementsToGet)) return Result<std::vector<Token>>(Errors::UnexpectedEndOfFile(tab[tab.size() - 1].position));

		std::vector<Token> res;
		for (int i = 0; i < numberOfElementsToGet;i++) {
			res.push_back(tab[index + i]);
		}
		return Result<std::vector<Token>>(res);
	}
	void TokenStream::popValue(int numberOfElementsToPop) {
		this->index += numberOfElementsToPop;
	}
	TokenStream& TokenStream::resetStream() {
		this->index = 0;
		return *this;
	}
	TokenStream& TokenStream::modify(std::function<void(Token&)> func) {
		for (auto& token : tab) {
			func(token);
		}
		return *this;
	}

	TokenStream& TokenStream::colapsTokensIntoOne(std::vector<tokenPredictor> predictors, tokenProducer producer) {
		std::vector<Token> res;
		for (int i = 0; i + predictors.size() <= tab.size();i++) {
			bool temp = true;
			for (int j = 0; j < predictors.size(); j++) {
				temp = temp && predictors[j](tab[i + j]);
			}
			if (temp == true) {
				std::vector<Token> temp(tab.begin() + i, tab.begin() + i + predictors.size());
				res.push_back(producer(temp));
				i += temp.size()-1;
			}
			else {
				res.push_back(tab[i]);
			}
		}
		for (int i = tab.size() - predictors.size()+1; i < tab.size();i++) {
			res.push_back(tab[i]);
		}
		this->tab = res;
		return *this;
	}
		
	TokenStream::Iterator::Iterator(TokenStream& ts_) : ts(ts_) {

	}
	TokenStream::Iterator& TokenStream::Iterator::match(Token::TokenType type) {
		Token tok;
		this->data.allMatched = this->data.allMatched && ts.getValue(this->data.offset).execute<bool>(
			[type,&tok](Token t) -> bool {
			bool res_ = t.type == type;
			if(res_) tok = t;
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);
		if (this->data.allMatched) { this->data.numberOfMatchedTokens++;}
		this->data.tokens.push_back(tok);
		this->data.offset++;
		return *this;
	}
	TokenStream::Iterator& TokenStream::Iterator::match(std::string str) {
		Token tok;
		this->data.allMatched = this->data.allMatched && ts.getValue(this->data.offset).execute<bool>(
			[str,&tok](Token t) -> bool {
			bool res_ = t.value == str;
			if(res_) tok = t;
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);
		if (this->data.allMatched) this->data.numberOfMatchedTokens++;
		this->data.tokens.push_back(tok);
		this->data.offset++;
		return *this;
	}
	TokenStream::Iterator& TokenStream::Iterator::match(Token::TokenType type, std::string str) {
		Token tok;
		this->data.allMatched = this->data.allMatched && ts.getValue(this->data.offset).execute<bool>(
			[type, str,&tok](Token t) -> bool {
			bool res_ = t.type == type and t.value == str;
			if(res_) tok = t;
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);
		if (this->data.allMatched) this->data.numberOfMatchedTokens++;
		this->data.tokens.push_back(tok);
		this->data.offset++;
		return *this;
	}
	TokenStream::Iterator& TokenStream::Iterator::optional(Token::TokenType type) {
		Token tok;
		bool res = ts.getValue(this->data.offset).execute<bool>(
			[type,&tok](Token t) -> bool {
			bool res_ = t.type == type;
			if(res_)	tok = t;
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);
		if (res) {
			this->data.numberOfMatchedTokens++;
			this->data.offset++;
		}
		this->data.tokens.push_back(tok);
		return *this;
	}
	TokenStream::Iterator& TokenStream::Iterator::optional(std::string str) {
		Token tok;
		bool res = ts.getValue(this->data.offset).execute<bool>(
			[str,&tok](Token t) -> bool {
			bool res_ = t.value == str;
			if(res_)	tok = t;
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);
		if (res) {
			this->data.numberOfMatchedTokens++;
			this->data.offset++;
		}
		this->data.tokens.push_back(tok);
		return *this;
	}
	TokenStream::Iterator& TokenStream::Iterator::optional(Token::TokenType type, std::string str) {
		Token tok;
		bool res = ts.getValue(this->data.offset).execute<bool>(
			[type, str,&tok](Token t) -> bool {
			bool res_ = t.type == type and t.value == str;
			if(res_) tok = t;
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);
		if (res) {
			this->data.numberOfMatchedTokens++;
			this->data.offset++;
		}
		this->data.tokens.push_back(tok);
		return *this;
	}
	TokenStream::Iterator::MatchResult TokenStream::Iterator::end() {
		return this->data;
	}
	//TokenStream::Iterator::MatchResult(Iterator it) {
	//	this->numberOfMatchedTokens = it.numberOfMatchedTokens;
	//	this->allMatched = it.allMatched;
	//}

	TokenStream::Iterator::MatchResult::operator bool(){
		return this->allMatched;
	}
	TokenStream::Iterator TokenStream::start() {
		return Iterator(*this);
	}
	TokenStream& TokenStream::consumeTokens(TokenStream::Iterator::MatchResult mr) {
		if(mr.allMatched)
			this->popValue(mr.numberOfMatchedTokens);
		return *this;
	}
	std::ostream& operator<<(std::ostream& out, TokenStream::Iterator::MatchResult it) {
		out << "[" << it.offset << "]" << "{" << it.numberOfMatchedTokens << "}" << (it.allMatched ? "TRUE" : "FALSE");
		return out;
	}
	expectTokenRes::expectTokenRes() {
		this->matched = false;
		this->token = Token();
		this->consumed = 0;
	}
	expectTokenRes::expectTokenRes(Token token) {
		this->matched = true;
		this->token = token;
		this->consumed = token.value.size();
	}
	expectTokenRes::expectTokenRes(Token token, int consumed) {
		this->matched = true;
		this->token = token;
		this->consumed = consumed;
	}
	std::ostream& operator<<(std::ostream& out, expectTokenRes e) {
		out << "\t>>> " << (e.matched ? "[T]" : "[F]") << e.token;
		return out;
	}

	Result<expectTokenRes> expectKeyword(const CharacterStream cs, std::string keyword, Token::TokenType type) {
		return cs.getValues(keyword.size()+1)
			.execute<expectTokenRes>([cs, keyword, type](std::vector<Character> vec) {
			for (int i = 0;i < keyword.size();i++) {
				if (keyword[i] != vec[i].value)
					return expectTokenRes();
			}
			if (not LexerRules::identyfireProhibitedCharacter(vec[keyword.size()].value)) {
				return expectTokenRes();
			}
			return expectTokenRes{ Token{keyword, type, cs.getCursor()} };
		}
		);
	}

	Result<expectTokenRes> expectOperator(const CharacterStream cs, std::string keyword, Token::TokenType type) {
		return cs.getValues(keyword.size())
			.execute<expectTokenRes>(
				[cs, keyword, type](std::vector<Character> vec) {
			for (int i = 0;i < keyword.size();i++) {
				if (keyword[i] != vec[i].value)
					return expectTokenRes();
			}
			return expectTokenRes(Token(keyword, type, cs.getCursor()));
		}
		);
	}

	Result<expectTokenRes> expectLiteral(const CharacterStream cs, LexerRules::LiteralDefiniton literal) {
		auto a = cs.getValue();
		if (a.isError()) return a.carryError<expectTokenRes>();
		auto ch = a.valueOr(Character());
		if (literal.start(ch.value)) {
			std::string tokenValue;
			if (literal.consumeStarting)
				tokenValue += (const char)ch.value;

			for (int i = 1;true;i++) {
				auto b = cs.getValue(i);
				if (b.isError()) return Result<expectTokenRes>(Errors::UnendedLiteral(cs.getCursor()));

				if (not literal.stop(b.valueOr(' ')) or literal.consumeStoping) {
					tokenValue += (char)b.valueOr(' ');
				}
				if (literal.stop(b.valueOr(' '))) break;
			}
			int padding = 0;
			if (literal.consumeStoping == false and literal.skipStoping == true) {
				padding = 1;
			}
			return Result<expectTokenRes>{expectTokenRes{ Token(tokenValue, literal.type, cs.getCursor()), (int)(tokenValue.size()%(1024*1024)) + padding }};
		}
		return Result<expectTokenRes>(expectTokenRes());
	}

	Result < expectTokenRes > carryToken(expectTokenRes a, expectTokenRes b) {
		if (a.matched == true and b.matched == true)
			return Result<expectTokenRes>(Errors::UnknownError(1, a.token.position));
		if (a.matched == true)
			return Result< expectTokenRes >(a);
		if (b.matched == true)
			return Result< expectTokenRes >(b);
		else
			return Result< expectTokenRes >(a);
	}

	Result < expectTokenRes >  ignoreLiterals(expectTokenRes a, expectTokenRes b) {
		if (a.matched == true and b.matched == true)
			return Result< expectTokenRes >(b);
		if (a.matched == true)
			return Result< expectTokenRes >(a);
		if (b.matched == true)
			return Result< expectTokenRes >(b);
		else
			return Result< expectTokenRes >(a);
	}

	Result<Token> Tokenizer::getToken(CharacterStream& cs) {


		Result< expectTokenRes > res{ Errors::UnexpectedEndOfFile(Position()) };
		int counter = 0;

		for (auto item : LexerRules::preambleList) {
			res = expectKeyword(cs, item, Token::TokenType::preamble)
				.flatSelect(res, carryToken);
		}
		for (auto item : LexerRules::keywordList) {
			res = expectKeyword(cs, item, Token::TokenType::keyword)
				.flatSelect(res, carryToken);
		}
		
		for (auto item : LexerRules::operatorList) {
			res = expectOperator(cs, item, Token::TokenType::operatorSymbol)
				.flatSelect(res, carryToken);
		}
		
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
	Result<TokenStream> Tokenizer::tokenize(CharacterStream cs) {
		std::vector<Token> res;
		bool loop_var = true;
		while (loop_var) {
			
			getToken(cs).execute<bool>([&res](Token t) {
				res.push_back(t);
				return true;
			}).onError([&loop_var](Errors::Error er) {
				
				loop_var = false;
				return er;
			});
		}
		return Result<TokenStream>(res);
	}
}