#pragma once

#include <vector>

#include "Result.h"
#include "IReadStream.h"
#include "CharacterStream.h"
#include "LexerRules.h"

namespace Tokenizer {
	class TokenStream : IReadStream<Token> {
		size_t index = 0;
		std::vector<Token> tab;
	public:
		TokenStream();
		TokenStream(std::vector<Token> vec);
		virtual bool isEnd(int64_t offset) const;
		virtual Result<Token> getValue() const;
		Result<Token> getValue(int64_t n) const;
		virtual Result<std::vector<Token>> getValues(int numberOfElementsToGet) const;
		virtual void popValue(int numberOfElementsToPop);
		TokenStream& resetStream();
		TokenStream& modify(std::function<void(Token&)> func);

		typedef std::function<bool(Token)> tokenPredictor;
		typedef std::function<Token(std::vector<Token>)> tokenProducer;

		TokenStream& colapsTokensIntoOne(std::vector<tokenPredictor> predictors, tokenProducer producer);
		class MatchResult;
		class Iterator {
			TokenStream& ts;
		public:
			struct  MatchResult
			{
				operator bool();
				int numberOfMatchedTokens = 0;
				bool allMatched = true;
				int offset = 0;
				std::vector<Token> tokens;
			};
		private:
			MatchResult data;
		public:
			Iterator(TokenStream& ts_);
			Iterator(Iterator&) = delete;
			Iterator operator=(Iterator&) = delete;
			Iterator operator=(Iterator&&) = delete;

			Iterator& match(Token::TokenType type);
			Iterator& match(std::string str);
			Iterator& match(Token::TokenType type, std::string str);
			Iterator& optional(Token::TokenType type);
			Iterator& optional(std::string str);
			Iterator& optional(Token::TokenType type, std::string str);
			MatchResult	end();
		};
		TokenStream& consumeTokens(Iterator::MatchResult mr);
		Iterator start();
	};

	std::ostream& operator<<(std::ostream& out, TokenStream::Iterator::MatchResult it);

	struct expectTokenRes {
		bool matched;
		Token token;
		int consumed;
		expectTokenRes();
		expectTokenRes(Token token);
		expectTokenRes(Token token, int consumed);
	};
	std::ostream& operator<<(std::ostream& out, expectTokenRes e);

	Result<expectTokenRes> expectKeyword(const CharacterStream cs, std::string keyword, Token::TokenType type);

	Result<expectTokenRes> expectOperator(const CharacterStream cs, std::string keyword, Token::TokenType type);

	Result<expectTokenRes> expectLiteral(const CharacterStream cs, LexerRules::LiteralDefiniton literal);

	Result < expectTokenRes > carryToken(expectTokenRes a, expectTokenRes b);

	Result < expectTokenRes >  ignoreLiterals(expectTokenRes a, expectTokenRes b);


	class Tokenizer {
	public:
		Result<TokenStream> tokenize(CharacterStream cs);
		Result<Token> getToken(CharacterStream& cs);
	};
}