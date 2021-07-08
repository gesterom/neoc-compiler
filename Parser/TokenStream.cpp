#include "pch.h"
#include "TokenStream.h"

namespace Parser {
	TokenStream::TokenStream() {}
	TokenStream::TokenStream(std::vector<Token> vec) : tab(vec) {}

	bool TokenStream::isEnd(int64_t offset) const {
		return (this->index + offset) > tab.size();
	}
	Result<Token> TokenStream::getValue() const {
		if (this->isEnd(1)) return Result<Token>(Errors::UnexpectedEndOfFile(tab[tab.size() - 1].position));
		return Result<Token>{tab[index]};
	}
	Result<Token> TokenStream::getPopValue()
	{
		auto res = getValue();
		popValue(1);
		return res;
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

	bool TokenStream::match(Token::TokenType type)
	{
		auto con = [type](Token t) {
			return t.type == type;
		};
		return match_impl(con);
	}

	bool TokenStream::match(std::string str)
	{
		auto con = [str](Token t) {
			return t.value == str;
		};
		return match_impl(con);
	}

	bool TokenStream::match(Token::TokenType type, std::string str)
	{
		auto con = [type, str](Token t) {
			return t.value == str and t.type == type;
		};
		return match_impl(con);
	}

	size_t TokenStream::getIndex()
	{
		return this->index;
	}

	void TokenStream::resetIndex(size_t newIndex)
	{
		this->index = newIndex;
	}

	bool TokenStream::match_impl(std::function<bool(Token t)> condition) {
		int offset = 0;
		auto calIndex = [&offset,this]() { return this->index + offset;};
		auto getToken = [calIndex,this]() { return this->tab[calIndex()];};
		while (not this->isEnd((calIndex()+1)%(1024*1024*1024))
			and (not condition(getToken()))
			and (getToken().type == Token::TokenType::Whitespace
				or getToken().type == Token::TokenType::newlineSymbol))
		{
			offset++;
		}
		//FIXME
		return this->getValue((offset) % (1024 * 1024 * 1024)).execute<bool>(
			[condition,this,offset](Token t) -> bool {
			bool res_ = condition(t);
			if (res_) this->resetIndex(this->getIndex()+offset);
			return res_;
		},
			[](Errors::Error er) -> bool {
			return false;
		}
		);
	}
}