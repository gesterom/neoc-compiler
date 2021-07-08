#include "pch.h"

#include "../Parser/pch.h"
#include "../Parser/TokenStream.h"
#include "../Parser/GrammarParser.h"
#include "../Parser/Structs.h"

TEST(TokenStream, match_tokenType) {
	std::stringstream ss("2");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	auto res = ts.tokenize(cs);
	ASSERT_FALSE(res.isError());
	auto temp = res.unpack();
	EXPECT_TRUE(temp.match(Parser::Token::TokenType::integerLiteral));
}

TEST(TokenStream, match_tokenType_ignoreWhitespace) {
	std::stringstream ss("    2");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	auto res = ts.tokenize(cs);
	ASSERT_FALSE(res.isError());
	auto temp = res.unpack();
	EXPECT_TRUE(temp.match(Parser::Token::TokenType::integerLiteral));
}

TEST(TokenStream, match_tokenType_ignoreNewLine) {
	std::stringstream ss("\n2");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	auto res = ts.tokenize(cs);
	ASSERT_FALSE(res.isError());
	auto temp = res.unpack();
	EXPECT_TRUE(temp.match(Parser::Token::TokenType::integerLiteral));
}

TEST(TokenStream, match_tokenType_ignoreNewLineSlashR) {
	std::stringstream ss("\r\n2");
	Parser::CharacterStream cs(ss, "filename");
	Parser::Tokenizer ts;
	bool test = false;
	auto res = ts.tokenize(cs);
	ASSERT_FALSE(res.isError());
	auto temp = res.unpack();
	EXPECT_TRUE(temp.match(Parser::Token::TokenType::integerLiteral));
}

TEST(GrammarParser, parse_Varible) {
	std::stringstream ss("Int a");
	Parser::CharacterStream cs(ss, "filename");
	auto res = Parser::Tokenizer().tokenize(cs).unpack();
	Parser::GrammarParser::GrammarParser gp(res);

	auto a = gp.parseVarible();
	ASSERT_FALSE(a.isError());
	auto b = a.unpack();
	EXPECT_EQ(b.type.type, Parser::Token::TokenType::TypeId);
	EXPECT_EQ(b.type.value, "Int");
	EXPECT_EQ(b.name.type, Parser::Token::TokenType::Id);
	EXPECT_EQ(b.name.value, "a");
}

//TEST(TokenStream, match_5Terminals) {
//	std::stringstream ss("return and procedure 1234 lll");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	auto a = res.start()
//		.match(Parser::Token::TokenType::keyword, "return")
//		.match(Parser::Token::TokenType::preamble, "and")
//		.match(Parser::Token::TokenType::preamble, "procedure")
//		.match(Parser::Token::TokenType::integerLiteral)
//		.match(Parser::Token::TokenType::Id)
//		.end();
//
//	EXPECT_TRUE(a.allMatched);
//}
//
//TEST(TokenStream, match_ReturnStmt) {
//	std::stringstream ss("return 0;");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	EXPECT_TRUE(res.start().matchReturn().end().allMatched);
//}
//
//TEST(TokenStream, match_ProcedureDecl) {
//	std::stringstream ss("procedure main(Int a, String b,Bool c)");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	EXPECT_TRUE(res.start().matchProcedureDeclaration().end().allMatched);
//}
//
//TEST(TokenStream, match_VaribleDeclStmt) {
//	std::stringstream ss("Int a;");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	EXPECT_TRUE(res.start().matchVaribleDeclaration().match(Parser::Token::TokenType::semicolonSymbol).end().allMatched);
//}
//
//TEST(TokenStream, match_expresionStmt) {
//	std::stringstream ss("a*b+c/15;");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	EXPECT_TRUE(res.start().matchExpression().match(Parser::Token::TokenType::semicolonSymbol).end().allMatched);
//}
//
//TEST(TokenStream, match_expresionStmt_array) {
//	std::stringstream ss("a[15 + c];");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	EXPECT_TRUE(res.start().matchExpression().match(Parser::Token::TokenType::semicolonSymbol).end().allMatched);
//}
//
//TEST(TokenStream, match_expresionStmt_functionCall) {
//	std::stringstream ss("a(15 + c);");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	EXPECT_TRUE(res.start().matchExpression().match(Parser::Token::TokenType::semicolonSymbol).end().allMatched);
//}
//
//TEST(TokenStream, match_expresionStmt_ConstructorCall) {
//	std::stringstream ss("Type(15 + c);");
//	Parser::CharacterStream cs(ss, "filename");
//	Parser::Tokenizer ts;
//	auto res = Parser::Tokenizer().tokenize(cs).unpack();
//	EXPECT_TRUE(res.start().matchExpression().match(Parser::Token::TokenType::semicolonSymbol).end().allMatched);
//}