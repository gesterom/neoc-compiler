#pragma once
namespace Parser {
	class TokenStream {
		std::vector<Token> tab;
		bool match_impl(std::function<bool(Token t)> condition);
		size_t index = 0;
	public:
		TokenStream();
		TokenStream(std::vector<Token> vec);
		bool isEnd(int64_t offset) const;
		Result<Token> getValue() const;
		Result<Token> getPopValue();
		Result<Token> getValue(int64_t n) const;
		Result<std::vector<Token>> getValues(int numberOfElementsToGet) const;
		void popValue(int numberOfElementsToPop);
		TokenStream& resetStream();
		TokenStream& modify(std::function<void(Token&)> func);
		TokenStream& remove(std::function<bool(Token&)> func);

		typedef std::function<bool(Token)> tokenPredictor;
		typedef std::function<Token(std::vector<Token>)> tokenProducer;

		TokenStream& colapsTokensIntoOne(std::vector<tokenPredictor> predictors, tokenProducer producer);
		bool match(Token::TokenType type);
		bool match(std::string str);
		bool match(Token::TokenType type, std::string str);
		size_t getIndex();
		void resetIndex(size_t newIndex);
		/*class MatchResult;
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
				Token notMatchedToken;
			};
		private:
			MatchResult data;
			Iterator& match_impl(std::function<bool(Token)>);
		public:
			Iterator(TokenStream& ts_);
			Iterator(Iterator&) = delete;
			Iterator operator=(Iterator&) = delete;
			Iterator operator=(Iterator&&) = delete;

			Iterator& match(Token::TokenType type);
			Iterator& match(std::string str);
			Iterator& match(Token::TokenType type, std::string str);

			MatchResult	end();
		};
		TokenStream& consumeTokens(Iterator::MatchResult mr);
		Iterator start();*/
	};
}