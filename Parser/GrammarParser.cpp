#include "pch.h"
#include "GrammarParser.h"
#include "Structs.h"

namespace Parser {

	enum class GrammarSymbols {
			
		terminals,
		characterLiteral,
		integarLiteral,


	};


	namespace GrammarParser {

		ParserResult<> GrammarParser::start() {
			return ParserResult{std::make_tuple(),true,*this};
		}

		GrammarParser::GrammarParser(Parser::TokenStream ts) {
			this->ts = ts;
		}
		SyntaxNodes::File GrammarParser::parseFile() {
			SyntaxNodes::File res;
			//while (not ts.isEnd(1)) {
			//	if (auto a = ts.start().matchProcedureDeclaration().matchStatement().end()) {
			//		//res.procedures.push_back(parseProcedure());
			//	}
			//}
			return res;
		}
		Result<SyntaxNodes::Declaration::Procedure> GrammarParser::parseProcedure(){
			SyntaxNodes::Declaration::Procedure res;

			if (auto a = this->start()
				.parseTerminal(Token::TokenType::preamble, "procedure")
				.parseTerminal(Token::TokenType::Id)
				.parseTerminal(Token::TokenType::parentheses, "(")
				.parseArguments()
				.parseTerminal(Token::TokenType::parentheses, "(")
				.parseStmt()
				)
			{
				res.name = a.get<1>().unpack();
				res.args = a.get<3>().unpack();
				res.body = a.get<5>().unpackVirtual();
				return res;
			}
			exit(-4);
			//ERROR

		}
		Result<SyntaxNodes::Statements::Statement> GrammarParser::parseStmt()
		{
			auto indexCopy = ts.getIndex();
			if (auto a = this->start().parseVarible().parseTerminal(Token::TokenType::semicolonSymbol)) {
				SyntaxNodes::Statements::VaribleDeclaration res;
				res.type = a.get<0>().unpack().type;
				res.name = a.get<0>().unpack().name;
				return Result<SyntaxNodes::Statements::Statement>(new SyntaxNodes::Statements::VaribleDeclaration(res));
			}
			ts.resetIndex(indexCopy);
			if (auto a = this->start()
				.parseTerminal(Token::TokenType::keyword, "if")
				.parseTerminal(Token::TokenType::parentheses, "(")
				.parseExpression()
				.parseTerminal(Token::TokenType::parentheses, ")")
				.parseStmt()
				.parseTerminal(Token::TokenType::keyword, "else")
				.parseStmt()
				)
			{

			}
			ts.resetIndex(indexCopy);
			if (auto a = this->start()
				.parseTerminal(Token::TokenType::keyword, "if")
				.parseTerminal(Token::TokenType::parentheses, "(")
				.parseExpression()
				.parseTerminal(Token::TokenType::parentheses, ")")
				.parseStmt()
				)
			{

			}
			ts.resetIndex(indexCopy);
			if (auto a = this->start()
				.parseTerminal(Token::TokenType::keyword, "while")
				.parseTerminal(Token::TokenType::parentheses, "(")
				.parseExpression()
				.parseTerminal(Token::TokenType::parentheses, ")")
				.parseStmt()
				)
			{

			}
			ts.resetIndex(indexCopy);
			if (auto a = this->start()
				.parseTerminal(Token::TokenType::keyword,"return")
				.parseExpression()
				.parseTerminal(Token::TokenType::semicolonSymbol)
				)
			{
				SyntaxNodes::Statements::Return res;
				res.exp = a.get<1>().unpackVirtual();
				return Result<SyntaxNodes::Statements::Statement>(new SyntaxNodes::Statements::Return(res));
			}
			ts.resetIndex(indexCopy);
			if (auto a = this->start()
				.parseExpression()
				.parseTerminal(Token::TokenType::semicolonSymbol)
				)
			{
				SyntaxNodes::Statements::Expression res;
				res.exp = a.get<0>().unpackVirtual();
				return Result<SyntaxNodes::Statements::Statement>(new SyntaxNodes::Statements::Expression(res));
			}
			ts.resetIndex(indexCopy);
			if (auto a = this->start()
				.parseTerminal(Token::TokenType::parentheses,"{")
				.parseBlockStmt()
				.parseTerminal(Token::TokenType::parentheses, "}")
				)
			{
				return Result<SyntaxNodes::Statements::Statement>(new SyntaxNodes::Statements::Block(a.get<1>().unpack()));
			}
			this->index = indexCopy;
			//ERROR

		}

		Result<SyntaxNodes::Statements::Block> GrammarParser::parseBlockStmt()
		{
			SyntaxNodes::Statements::Block res;
			while (true)
			{
				if (auto a = this->start().parseStmt()) {
					res.program.push_back(a.get<0>().unpackVirtual());
					if (this->start().parseTerminal(Token::TokenType::operatorSymbol, "}")) {
						break;
					}
					else {
						continue;
					}
				}
			}
			return Result<SyntaxNodes::Statements::Block>(res);
		}


		Result<SyntaxNodes::Expressions::Expression> GrammarParser::parseExpression()
		{
			return Result<SyntaxNodes::Expressions::Expression>();
		}

		Result<SyntaxNodes::Declaration::Varible> GrammarParser::parseVarible() {
			SyntaxNodes::Declaration::Varible res;
			if (auto a = this->start()
				.parseTerminal(Token::TokenType::TypeId)
				.parseTerminal(Token::TokenType::Id)
				)
			{
				res.type = a.get<0>().unpack();
				res.name = a.get<1>().unpack();
				return res;
			}
			exit(-4);
			//ERROR
		}

		Result<SyntaxNodes::Declaration::Arguments> GrammarParser::parseArguments()
		{
			SyntaxNodes::Declaration::Arguments res;
			while (true)
			{
				if (auto a = this->start().parseVarible()) {
					res.args.push_back(a.get<0>().unpack());
					if (this->start().parseTerminal(Token::TokenType::operatorSymbol,",")) {
						continue;
					}
					else {
						//TODO ERROR ???
						break;
					}
				}
			}

			return Result<SyntaxNodes::Declaration::Arguments>(res);
		}
		Result<Token> GrammarParser::parseTerminal(Token::TokenType type, std::string value)
		{
			if (ts.match(type, value)) {
				return ts.getPopValue();
			}
			else {
				return ts.getValue().createError([](const Token& t) {
					return Errors::UnexpectedToken(t);
				});
			}
		}
		Result<Token> GrammarParser::parseTerminal(Token::TokenType type)
		{
			if (ts.match(type)) {
				return ts.getPopValue();
			}
			else {
				return ts.getValue().createError([](const Token& t) {
					return Errors::UnexpectedToken(t);
				});
			}
		}
		Result<Token> GrammarParser::parseTerminal(std::string value)
		{
			if (ts.match(value)) {
				return ts.getPopValue();
			}
			else {
				return ts.getValue().createError([](const  Token& t) {
					return Errors::UnexpectedToken(t);
				});
			}
		}
	}
}