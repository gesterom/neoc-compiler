#pragma once
namespace Parser {
	class TokenStream {
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
				Token notMatchedToken;
			};
		private:
			MatchResult data;
			MatchResult orState;
			Iterator& match_impl(std::function<bool(Token)>);
		public:
			Iterator(TokenStream& ts_);
			Iterator(Iterator&) = delete;
			Iterator operator=(Iterator&) = delete;
			Iterator operator=(Iterator&&) = delete;

			Iterator& match(Token::TokenType type);
			Iterator& match(std::string str);
			Iterator& match(Token::TokenType type, std::string str);
			Iterator& matchReturn();
			Iterator& matchExpression();



			Iterator& optional(Token::TokenType type);
			Iterator& optional(std::string str);
			Iterator& optional(Token::TokenType type, std::string str);
			Iterator& or_();

			MatchResult	end();
			MatchResult orResult();
		};
		TokenStream& consumeTokens(Iterator::MatchResult mr);
		Iterator start();
	};
}