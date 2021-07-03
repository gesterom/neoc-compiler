#include "pch.h"
#include "TokenStream.h"
namespace Parser {
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
			if(tab.size() == 0) return Result<Token>(Errors::UnexpectedEndOfFile(Position()));
			return Result<Token>(Errors::UnexpectedEndOfFile(tab[tab.size() - 1].position));
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

	TokenStream& TokenStream::remove(std::function<bool(Token&)> func) {
		std::vector<Token> res;
		for (auto item : tab) {
			if (not func(item)) {
				res.push_back(item);
			}
		}
		this->tab = res;
		return* this;
	}

	size_t startingIndex(size_t tabSize, size_t predictorsSize) {
		if (predictorsSize > tabSize) return 0;
		else {
			return tabSize - predictorsSize + 1;
		}
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
				i += temp.size() - 1;
			}
			else {
				res.push_back(tab[i]);
			}
		}
		for (size_t i = startingIndex(tab.size(),predictors.size()); i < tab.size();i++) {
			res.push_back(tab[i]);
		}
		this->tab = res;
		return *this;
	}

	TokenStream::Iterator::Iterator(TokenStream& ts_) : ts(ts_) {
		this->orState.allMatched = false;
	}

	TokenStream::Iterator& TokenStream::Iterator::match_impl(std::function<bool(Token t)> condition) {
		//TODO semicolon warnings
		if (this->data.allMatched == false) return *this;
		int index = 0;
		Token tok;
		auto offset = [&index](Iterator* it) { return it->data.offset + it->ts.index + index;};
		auto getToken = [offset](Iterator* it) { return it->ts.tab[offset(it)];};
		offset(this);

		while (not ts.isEnd((offset(this)+1)%(1024*1024*1024))
			and (not condition(getToken(this)))
			and (getToken(this).type == Token::TokenType::Whitespace
				or getToken(this).type == Token::TokenType::newlineSymbol))
		{
			index++;
		}
		//FIXME
		bool res = ts.getValue((this->data.offset + index) % (1024 * 1024 * 1024)).execute<bool>(
			[condition, &tok](Token t) -> bool {
			bool res_ = condition(t);
			if (res_) tok = t;
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);

		if (res) {
			this->data.tokens.push_back(tok);
		}
		else {
			this->data.allMatched = false;
			this->data.notMatchedToken = tok;
		}

		if (this->data.allMatched) this->data.numberOfMatchedTokens += 1 + index;
		this->data.offset += 1 + index;
		return *this;
	}

	TokenStream::Iterator& TokenStream::Iterator::match(Token::TokenType type) {
		auto con = [type](Token t) {
			return t.type == type;
		};
		return match_impl(con);
	}
	TokenStream::Iterator& TokenStream::Iterator::match(std::string str) {
		auto con = [str](Token t) {
			return t.value == str;
		};
		return match_impl(con);
	}
	TokenStream::Iterator& TokenStream::Iterator::match(Token::TokenType type, std::string str) {

		auto con = [type, str](Token t) {
			return t.value == str and t.type == type;
		};
		return match_impl(con);
	}
	TokenStream::Iterator& TokenStream::Iterator::optional(Token::TokenType type) {
		Token tok;
		bool res = ts.getValue(this->data.offset).execute<bool>(
			[type, &tok](Token t) -> bool {
			bool res_ = t.type == type;
			if (res_)	tok = t;
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
			[str, &tok](Token t) -> bool {
			bool res_ = t.value == str;
			if (res_)	tok = t;
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
			[type, str, &tok](Token t) -> bool {
			bool res_ = t.type == type and t.value == str;
			if (res_) tok = t;
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
	TokenStream::Iterator& TokenStream::Iterator::or_() {
		if (not this->orState.allMatched and this->data.allMatched) {
			this->orState = this->data;
		}
		return *this;
	}

	TokenStream::Iterator::MatchResult TokenStream::Iterator::orResult() {
		return this->orState;
	}


	TokenStream::Iterator::MatchResult::operator bool() {
		return this->allMatched;
	}
	TokenStream::Iterator TokenStream::start() {
		return Iterator(*this);
	}
	TokenStream& TokenStream::consumeTokens(TokenStream::Iterator::MatchResult mr) {
		if (mr.allMatched)
			this->popValue(mr.numberOfMatchedTokens);
		return *this;
	}
	std::ostream& operator<<(std::ostream& out, TokenStream::Iterator::MatchResult it) {
		out << "[" << it.offset << "]" << "{" << it.numberOfMatchedTokens << "}" << (it.allMatched ? "TRUE" : "FALSE");
		return out;
	}
	TokenStream::Iterator& TokenStream::Iterator::matchExpression() {
		if (this->data.allMatched == false) return *this;

		auto data_copy = this->data;
		//function/procedure call;
		if( this->match(Token::TokenType::Id).match(Token::TokenType::parentheses,"(").matchExpression().match(Token::TokenType::parentheses,")").end() )
			return *this;

		// array acces
		this->data = data_copy;
		if( this->match(Token::TokenType::Id).match(Token::TokenType::parentheses, "[").matchExpression().match(Token::TokenType::parentheses, "]").end() ) 
			return *this;

		//Constructor call
		this->data = data_copy;
		if (this->match(Token::TokenType::TypeId).match(Token::TokenType::parentheses, "(").matchExpression().match(Token::TokenType::parentheses, ")").end())
			return *this;

		auto matchLiterals = [this,data_copy](Token::TokenType type) -> bool {
			this->data = data_copy;
			if (this->match(type).end()) {
				auto a = this->data;
				this->data = data_copy;
				if (this->match(type).match(Token::TokenType::operatorSymbol).matchExpression().end()) {
					return true;
				}
				else {
					this->data = a;
					return true;
				}
			}
			return false;
		};

		if (matchLiterals(Token::TokenType::charLiteral)) return *this;
		if (matchLiterals(Token::TokenType::stringLiteral)) return *this;
		if (matchLiterals(Token::TokenType::integerLiteral)) return *this;
		if (matchLiterals(Token::TokenType::floatLiteral)) return *this;
		if (matchLiterals(Token::TokenType::Id)) return *this;
		return *this;
	}
	TokenStream::Iterator& TokenStream::Iterator::matchReturn() {
		if (this->data.allMatched == false) return *this;
		this->match(Token::TokenType::keyword, "return").matchExpression().match(Token::TokenType::semicolonSymbol);
		return *this;
	}

	TokenStream::Iterator& TokenStream::Iterator::matchVaribleDeclaration() {
		if (this->data.allMatched == false) return *this;
		this->match(Token::TokenType::TypeId).match(Token::TokenType::Id);
		return *this;
	}

	TokenStream::Iterator& TokenStream::Iterator::matchArguments() {
		if (this->data.allMatched == false) return *this;
		this->matchVaribleDeclaration();
		auto copy = this->data;
		if (this->match(Token::TokenType::operatorSymbol, ",").end()) {
			this->matchArguments();
		}
		else {
			this->data = copy;
		}
		return *this;
	}

	TokenStream::Iterator& TokenStream::Iterator::matchProcedureDeclaration() {
		if (this->data.allMatched == false) return *this;
		this->match(Token::TokenType::preamble, "procedure")
			.match(Token::TokenType::Id)
			.match(Token::TokenType::parentheses, "(")
			.matchArguments()
			.match(Token::TokenType::parentheses, ")");

		return *this;
	}

	TokenStream::Iterator& TokenStream::Iterator::matchStatement() {
		if (this->data.allMatched == false) return *this;
		auto copy = this->data;
		//block stmt
		if (this->match(Token::TokenType::parentheses, "{").matchStatement().match(Token::TokenType::parentheses, "}").end()) return *this;
		
		//Varible declaration stmt;
		this->data = copy;
		if (this->matchVaribleDeclaration().match(Token::TokenType::semicolonSymbol).end()) return *this;

		//TODO if,switch,for,while, send, lambda;
		this->data = copy;
		if (this->matchReturn().end()) return *this;
		this->data = copy;
		if (this->match(Token::TokenType::keyword, "if")
			.match(Token::TokenType::parentheses, "(")
			.matchExpression()
			.match(Token::TokenType::parentheses, ")")
			.matchStatement().end()) 
				return *this;

		this->data = copy;
		if (this->match(Token::TokenType::keyword, "while")
			.match(Token::TokenType::parentheses, "(")
			.matchExpression()
			.match(Token::TokenType::parentheses, ")")
			.matchStatement().end()) 
				return *this;

		//expression stmt
		this->data = copy;
		if (this->matchExpression().match(Token::TokenType::semicolonSymbol).end()) return *this;

		//empty stmt;
		this->data = copy;
		if (this->match(Token::TokenType::semicolonSymbol).end()) return *this;
		return *this;
	}
}