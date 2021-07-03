#include "pch.h"

#include "../Parser/pch.h"
#include "../Parser/TokenStream.h"

TEST(TokenStream, match_tokenType) {
	std::stringstream ss("2");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	bool test = false;
	auto res = ts.tokenize(cs);
	EXPECT_TRUE(!res.isError());
	res.modify([&test](Parser::TokenStream& ts) {
		test = ts.start().match(Parser::Token::TokenType::integerLiteral).end();
	});
	EXPECT_TRUE(test);
}

TEST(TokenStream, match_Expression) {
	std::stringstream ss("2");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	auto res = Parser::Tokenizer().tokenize(cs).unpack();
	EXPECT_TRUE(res.start().matchExpression().end().allMatched);
}

TEST(TokenStream, match_5Terminals) {
	std::stringstream ss("return and procedure 1234 lll");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	auto res = Parser::Tokenizer().tokenize(cs).unpack();
	auto a = res.start()
		.match(Parser::Token::TokenType::keyword, "return")
		.match(Parser::Token::TokenType::preamble, "and")
		.match(Parser::Token::TokenType::preamble, "procedure")
		.match(Parser::Token::TokenType::integerLiteral)
		.match(Parser::Token::TokenType::Id)
		.end();

	EXPECT_TRUE(a.allMatched);
}

TEST(TokenStream, match_ReturnStmt) {
	std::stringstream ss("return 0;");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	auto res = Parser::Tokenizer().tokenize(cs).unpack();
	EXPECT_TRUE(res.start().matchReturn().end().allMatched);
}