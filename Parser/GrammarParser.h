#pragma once

#include <tuple>

namespace Parser {
	namespace GrammarParser {

		template<typename... Types>
		class ParserResult;

		class GrammarParser
		{
			TokenStream ts;
			int index = 0;

		public: 
			ParserResult<> start();
			GrammarParser(TokenStream ts);
			SyntaxNodes::File parseFile();
			Result<SyntaxNodes::Declaration::Procedure> parseProcedure();
			Result<SyntaxNodes::Declaration::Varible> parseVarible();
			Result<SyntaxNodes::Declaration::Arguments> parseArguments();
			Result<SyntaxNodes::Statements::Statement> parseStmt();
			Result<SyntaxNodes::Statements::Block> parseBlockStmt();
			Result<SyntaxNodes::Expressions::Expression> parseExpression();
			Result<Token> parseTerminal(Token::TokenType type, std::string value);
			Result<Token> parseTerminal(Token::TokenType type);
			Result<Token> parseTerminal(std::string value);
		};

		template<std::size_t N, std::size_t... I, class Tp>
		constexpr auto reverse_tuple_impl(std::index_sequence<I...>, Tp const& tp)
		{
			return std::make_tuple(std::get<N - 1 - I>(tp)...);
		}

		template<class Tp, std::size_t N = std::tuple_size<Tp>::value, class S = std::make_index_sequence<N>>
		constexpr auto reverse_tuple(Tp const& tp)
		{
			return reverse_tuple_impl<N>(S(), tp);
		}



		template<typename... Types>
		class ParserResult {



		public:
			std::tuple<Types...> values;
			bool allMatched = true;
			int offset = 0;
			GrammarParser& gp;

			template<int index>
			auto get() {
				constexpr int o = std::tuple_size<decltype (this->values) >() - index - 1;
				return std::get<o>(values);
			}

			void consumeTokens() {
				
			}

			ParserResult(std::tuple<Types...> a,bool matched,GrammarParser& parser) : gp(parser)
			{
				values = a;
				this->allMatched = matched;
			}
			operator bool() {
				return this->allMatched;
			}

			ParserResult<Result<SyntaxNodes::Expressions::Expression>, Types... > parseExpression() {
				std::tuple<Result<SyntaxNodes::Expressions::Expression>> a;
				bool matched = true;
				if (allMatched == false) {
					a = std::make_tuple(Result<SyntaxNodes::Expressions::Expression>(Errors::Error{}));
					matched = false;
				}
				else {
					auto t = gp.parseExpression();
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<SyntaxNodes::Expressions::Expression>, Types... >(b, matched, gp);
			}


			ParserResult<Result<SyntaxNodes::Declaration::Arguments>, Types... > parseArguments() {
				std::tuple<Result<SyntaxNodes::Declaration::Arguments>> a;
				bool matched = true;
				if (allMatched == false) {
					a = std::make_tuple(Result<SyntaxNodes::Declaration::Arguments>(Errors::Error{}));
					matched = false;
				}
				else {
					auto t = gp.parseArguments();
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a,values);
				return ParserResult<Result<SyntaxNodes::Declaration::Arguments>, Types... >( b , matched , gp );
			}

			ParserResult<Result<SyntaxNodes::Statements::Block>, Types... > parseBlockStmt() {
				std::tuple<Result<SyntaxNodes::Statements::Block>> a;
				bool matched = true;
				if (allMatched == false) {
					a = std::make_tuple(Result<SyntaxNodes::Statements::Block>(Errors::Error{}));
					matched = false;
				}
				else {
					auto t = gp.parseBlockStmt();
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<SyntaxNodes::Statements::Block>, Types... >(b, matched, gp);
			}

			ParserResult<Result<SyntaxNodes::Statements::Statement>, Types... > parseStmt() {
				std::tuple<Result<SyntaxNodes::Statements::Statement>> a;
				bool matched = true;
				if (allMatched == false) {
					a = std::make_tuple(Result<SyntaxNodes::Statements::Statement>(Errors::Error{}));
					matched = false;
				}
				else {
					auto t = gp.parseStmt();
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<SyntaxNodes::Statements::Statement>, Types... >( b , matched , gp );
			}

			ParserResult<Result<SyntaxNodes::Declaration::Procedure>,Types... > parseProcedure() {
				std::tuple<Result<SyntaxNodes::Declaration::Procedure>> a;
				bool matched = true;
				if (allMatched == false) {
					a = std::make_tuple(Result<SyntaxNodes::Declaration::Procedure>(Errors::Error{}));
					matched = false;
				}
				else {
					auto t = gp.parseProcedure();
					matched = ! t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<SyntaxNodes::Declaration::Procedure>, Types... >{ b , matched , gp};
			}

			ParserResult<Result<SyntaxNodes::Declaration::Varible>, Types... > parseVarible() {
				std::tuple<Result<SyntaxNodes::Declaration::Varible>> a;
				bool matched = true;
				if (allMatched == false) {
					a = std::make_tuple(Result<SyntaxNodes::Declaration::Varible>(Errors::Error{}));
					matched = false;
				}
				else {
					auto t = gp.parseVarible();
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<SyntaxNodes::Declaration::Varible>, Types... >{ b , matched , gp };
			}

			ParserResult<Result<Token>, Types... > parseTerminal(Token::TokenType type,std::string value) {
				std::tuple< Result<Token> > a;
				bool matched = true;
				if (this->allMatched == false) {
					a = std::make_tuple(Result<Token>(Errors::Error{}));
				}
				else {
					auto t = gp.parseTerminal(type, value);
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<Token>, Types... >( b , matched , gp);
			}
			ParserResult<Result<Token>, Types... > parseTerminal(Token::TokenType type) {
				std::tuple<Result<Token>> a;
				bool matched = true;
				if (this->allMatched == false) {
					a = std::make_tuple(Result<Token>(Errors::Error{}));
				}
				else {
					auto t = gp.parseTerminal(type);
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<Token>, Types... >(b, matched, gp);
			}
			ParserResult<Result<Token>,Types... > parseTerminal(std::string value) {
				std::tuple<Result<Token>> a;
				bool matched = true;
				if (this->allMatched == false) {
					a = std::make_tuple(Result<Token>(Errors::Error{}));
				}
				else {
					auto t = gp.parseTerminal(value);
					matched = !t.isError();
					a = std::make_tuple(t);
				}
				auto b = std::tuple_cat(a, values);
				return ParserResult<Result<Token>, Types... >(b, matched, gp);
			}

		};

	}
}
