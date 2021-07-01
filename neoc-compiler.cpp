// neoc-compiler.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <functional>
#include <string>
#include <cstdlib>
#include <cassert>

#include "Structs.h"
#include "Result.h"
#include "Errors.h"
#include "CharacterStream.h"
#include "LexerRules.h"
#include "Tokenizer.h"
#include "Logger.h"


namespace GrammarParser {
	namespace SyntaxNodes {
		//c declaration
		namespace Declaration {
			class Procedure;
			class Type;
			class Varible;
			class Arguments;
		}
		namespace Statements {
			class Statement;
			class Block;
			class VaribleDeclaration;
			class VaribleDeclDefinition;
		}
		class File;


		class IVisitor {
		public:
			virtual void visit(File*) = 0;
			virtual void visit(Statements::Block*) = 0;
			virtual void visit(Statements::VaribleDeclaration*) = 0;
			virtual void visit(Statements::VaribleDeclDefinition*) = 0;
			virtual void visit(Declaration::Procedure*) = 0;
			virtual void visit(Declaration::Type*) = 0;
			virtual void visit(Declaration::Varible*) = 0;
			virtual void visit(Declaration::Arguments*) = 0;
		};
		class Node {
		public:
			virtual void visit(IVisitor * visitor) = 0;
		};
		//c definiton

		namespace Declaration {
			class Type : public Node{
			public:
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
			};
			class Varible : public Node{
			public:
				Token type;
				Token name;
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
			};
			class Arguments : public Node {
			public:
				std::vector<Declaration::Varible> args;
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
			};
			class Procedure : public Node{
			public:
				Token name;
				Arguments args;
				Token returnType;
				Statements::Statement* body;
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
			};
		}
		namespace Statements {
			class Statement : public Node {};
			class Block : public Statement {
			public:
				std::vector<Statement*> program;
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
			};
			class VaribleDeclaration : public Statement {
			public:
				Token name;
				Token type;
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
			};
			class VaribleDeclDefinition : public Statement {
			public:
				Token name;
				Token type;
				//Expresion expr;
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
			};
		}
		class File : public Node{
			public:
				std::vector<Declaration::Procedure> procedures;
				std::vector < Declaration::Type > types;
				virtual void visit(IVisitor* visitor) {
					visitor->visit(this);
				}
		};
		class PrintVisitor : public IVisitor{
			int tab = 0;
		public:
			std::ostream& print() {
				for (int i = 0; i < tab; i++) {
					std::cout << "\t";
				}
				return std::cout;
			}
			virtual void visit(File* file) {
				for (auto& item : file->procedures) {
					this->tab++;
					item.visit(this);
					this->tab--;
				}
			}
			virtual void visit(Declaration::Procedure* p ) {
				print() << "[Procedure]" << std::endl;
				this->tab++;
				print() << "name : " << p->name << std::endl;
				print() << "args : " << std::endl;
				this->tab++;
				p->args.visit(this);
				this->tab--;
				p->body->visit(this);
				this->tab--;
			}
			virtual void visit(Declaration::Type*) {};
			virtual void visit(Statements::VaribleDeclaration* var) {
				print() << var->type << " : " << var->name << std::endl;
			};
			virtual void visit(Statements::VaribleDeclDefinition* var) {
				print() << var->type << " : " << var->name << std::endl;
			};
			virtual void visit(Statements::Block* block) {
				print() << "[BLOCK]" << std::endl;
				this->tab++;
				for (auto item : block->program) {
					item->visit(this);
				}
				this->tab--;
			};
			virtual void visit(Declaration::Varible*) {};
			virtual void visit(Declaration::Arguments* a) {
				for (auto& item : a->args) {
					print() << item.type << " : " << item.name << std::endl;
				}
			}
		};
	}
	class Parser {
		Tokenizer::TokenStream ts;
	public:
		Parser(Tokenizer::TokenStream ts) {
			this->ts = ts;
		}
		Result<SyntaxNodes::File> parseFile() {
			SyntaxNodes::File res;
			bool loop = true;
			auto errorHandler = [&loop](Errors::Error er) {
				std::cout << er << std::endl;
				loop = false;
				return er;
			};
			while (not ts.isEnd(1) and loop == true) {
				if (auto a = ts.start().match(Token::TokenType::preamble, "procedure").end()) {
					parseProcedureDeclaration().execute<SyntaxNodes::File>([&res](SyntaxNodes::Declaration::Procedure p) {
						res.procedures.push_back(p);
						return res;
					}).onError(errorHandler);
					continue;
				}
				if (auto a = ts.start().match(Token::TokenType::preamble, "type").end()) {
					parseTypeDeclaration().execute<SyntaxNodes::File>([&res](SyntaxNodes::Declaration::Type t) {
						res.types.push_back(t);
						return res;
					}).onError(errorHandler);
					continue;
				}
				break;
				//TODO function,class,alias
				//auto a = parseProcedure();
			}
			return res;
		}
		Result<SyntaxNodes::Declaration::Varible> parseVaribleDeclaration() {
			if (auto a = ts.start()
				.optional(Token::TokenType::Whitespace)
				.match(Token::TokenType::TypeId)
				.optional(Token::TokenType::Whitespace)
				.match(Token::TokenType::Id)
				.optional(Token::TokenType::Whitespace)
				.end())
			{
				SyntaxNodes::Declaration::Varible res;
				res.type = a.tokens[1];
				res.name = a.tokens[3];
				ts.consumeTokens(a);
				return Result<SyntaxNodes::Declaration::Varible>(res);
			}
			else {
				return ts.getValue().createError(
					[](Token t) {
						return Errors::UnexpectedToken(Token::TokenType::TypeId, t);
				}).carryError<SyntaxNodes::Declaration::Varible>();
			}
		}
		Result<SyntaxNodes::Declaration::Arguments> parseArgumentsDeclaration() {
			Result<SyntaxNodes::Declaration::Arguments> res{ SyntaxNodes::Declaration::Arguments() };
			Token last;
			auto func = [this,&last](SyntaxNodes::Declaration::Varible var, SyntaxNodes::Declaration::Arguments args) -> SyntaxNodes::Declaration::Arguments {
				args.args.push_back(var);
				last = var.type;
				if (auto b = this->ts.start().match(Token::TokenType::operatorSymbol, ",").end()) {
					this->ts.consumeTokens(b);
				}
				return args;
			};

			bool loop = true;
			while ((not ts.start().match(Token::TokenType::parentheses, ")").end()) && loop) {
				res = parseVaribleDeclaration()
					.combine<SyntaxNodes::Declaration::Arguments, SyntaxNodes::Declaration::Arguments>(func, res)
					.onError(
						[&loop](Errors::Error er) {
							std::cout << "ERROR" << std::endl;
							std::cout << er << std::endl;
							loop = false;
							return er;
						}
					);
			}
			if (ts.start().match(Token::TokenType::parentheses, ")").end()) {
				return res;
			}
			else {
				return Result<SyntaxNodes::Declaration::Arguments>(Errors::UnexpectedEndOfFile(last.position));
			}
		}
		Result<SyntaxNodes::Declaration::Procedure> parseProcedureDeclaration() {
			if (auto a = ts.start()
				.match(Token::TokenType::preamble, "procedure")
				.optional(Token::TokenType::Whitespace)
				.match(Token::TokenType::Id)
				.optional(Token::TokenType::Whitespace)
				.match(Token::TokenType::parentheses, "(")
				.end()) {

				auto name = a.tokens[2];
				ts.consumeTokens(a);

				auto res = parseArgumentsDeclaration().combine<SyntaxNodes::Declaration::Procedure, Token>(
					[](SyntaxNodes::Declaration::Arguments ar, Token t)-> SyntaxNodes::Declaration::Procedure {
					SyntaxNodes::Declaration::Procedure res;
					res.name = t;
					res.args = ar;
					return res;
				},Result<Token>(name));

				if (auto b = ts.start().match(Token::TokenType::parentheses, ")").end()) {
					ts.consumeTokens(b); //FIXME
					res = res.combine<SyntaxNodes::Declaration::Procedure, SyntaxNodes::Statements::Statement*>(
						[](SyntaxNodes::Declaration::Procedure p, SyntaxNodes::Statements::Statement* stmt) {
							p.body = stmt;
							return p;
						},parseStatement());
					return res;
				}
				else {
					return Result<SyntaxNodes::Declaration::Procedure>(Errors::UnexpectedToken(Token::TokenType::parentheses, ")", ts.getValue().valueOr(Token())));
				}
				return res;
			}
			else {
				return ts.getValue().createError(
					[](Token t) {
					return Errors::UnknownError(2, t.position);
				}).carryError<SyntaxNodes::Declaration::Procedure>();
			}
		}
		Result<SyntaxNodes::Statements::Statement*> parseStatement() {
			if (auto a = ts.start().match(Token::TokenType::parentheses, "{").end()) {
				ts.consumeTokens(a);
				Result<SyntaxNodes::Statements::Block*> res(new SyntaxNodes::Statements::Block());

				auto func = [](SyntaxNodes::Statements::Statement* newStmt, SyntaxNodes::Statements::Block* block) {
					block->program.push_back(newStmt);
					return block;
				};

				bool loop = true;
				while (not ts.start().match(Token::TokenType::parentheses, "}").end() && loop) {
					res = parseStatement()
						.combine<SyntaxNodes::Statements::Block*, SyntaxNodes::Statements::Block*>(func, res)
						.onError(
							[&loop](Errors::Error er) {
								std::cout << er << std::endl;
								loop = false;
								return er;
							}
						);
				}
				if (ts.start().match(Token::TokenType::parentheses, "}").end()) {
					return res.execute<SyntaxNodes::Statements::Statement*>([](SyntaxNodes::Statements::Block* block) {
						return block;
					});
				}
				else {
					return ts.getValue().createError(
						[](Token t) {
						return InternalError_MACRO("help");
					}).carryError<SyntaxNodes::Statements::Statement*>();
				}
			}
			if (auto a = ts.start()
				.match(Token::TokenType::TypeId)
				.optional(Token::TokenType::Whitespace)
				.match(Token::TokenType::Id)
				.optional(Token::TokenType::Whitespace)
				.match(Token::TokenType::operatorSymbol,";")
				.end()) {
				SyntaxNodes::Statements::VaribleDeclaration* res = new SyntaxNodes::Statements::VaribleDeclaration();
				res->type = a.tokens[0];
				res->name = a.tokens[2];
				ts.consumeTokens(a);
				return Result<SyntaxNodes::Statements::Statement*>(res);
			}
			return ts.getValue().createError(
				[](Token t) {
				return Errors::UnexpectedToken(Token::TokenType::TypeId, t);
			}).carryError<SyntaxNodes::Statements::Statement*>();
		}
		Result<SyntaxNodes::Declaration::Type> parseTypeDeclaration() {
			return SyntaxNodes::Declaration::Type();
		}
	};
}


void standerizedWhitespace(Tokenizer::TokenStream& ts) {
	ts.modify([](Token& t) {
		if (t.type == Token::TokenType::Whitespace) {
			t.value = " ";
		}
	});
}

auto replaceToken(Token::TokenType typeToReplace,std::string tokenValueToReplace,Token::TokenType newType,std::string newValue) {
	return [typeToReplace,tokenValueToReplace,newType,newValue](Tokenizer::TokenStream& ts) {
		ts.modify([typeToReplace, tokenValueToReplace, newType, newValue](Token& t) {
			if (t.type == typeToReplace and t.value == tokenValueToReplace) {
				debugLine();
				t.value = newValue;
				t.type = newType;
			}
		});
	};
}

void combieneNumbersIntoFloats(Tokenizer::TokenStream& ts) {

	auto numberPredictor = [](Token t) {return t.type == Token::TokenType::integerLiteral;};
	auto piriodPredicotr = [](Token t) {return t.type == Token::TokenType::operatorSymbol and t.value == ".";};

	auto floatProducer = [](std::vector<Token> vec) -> Token {
		Token res;
		res.value = vec[0].value + vec[1].value + vec[2].value;
		res.type = Token::TokenType::floatLiteral;
		res.position = vec[0].position;
		return res;
	};

	ts.colapsTokensIntoOne({numberPredictor,piriodPredicotr,numberPredictor},floatProducer);
}

void combieneIntoNamsepace(Tokenizer::TokenStream& ts) {

	auto namespaceSymbol = [](Token t) {return t.type == Token::TokenType::operatorSymbol and t.value == "::";};
	auto idpredictor = [](Token t) {return t.type == Token::TokenType::Id or t.type==Token::TokenType::TypeId;};

	auto floatProducer = [](std::vector<Token> vec) -> Token {
		Token res;
		res.value = vec[0].value;
		res.type = Token::TokenType::namespaceAcces;
		res.position = vec[0].position;
		return res;
	};

	ts.colapsTokensIntoOne({ idpredictor,namespaceSymbol}, floatProducer);
}

auto printCharacterVector(std::vector<Character> vec) -> std::vector<Character> {
	for (auto ch : vec) {
		std::cout << ch.position.toString() << " " << ch.value << std::endl;
	}
	return vec;
}

Errors::Error gereralErrorHandler(Errors::Error er) {
	std::cout << er.getMesage() << std::endl;
	return er;
}

void printTokenStream(Tokenizer::TokenStream& ts) {
	ts.modify([](Token t) {
		std::cout << t << std::endl;
	});
}

int main()
{
	CharacterStream cs("test_files/test2.nc");
	//re.execute<bool>(func);
	debugLine();
	cs.getValues(cs.size())
		.execute<std::vector<Character>>(printCharacterVector)
		.onError(gereralErrorHandler);
	debugLine();
	Tokenizer::Tokenizer tokenizer;
	auto ts = tokenizer.tokenize(cs)
		.modify(standerizedWhitespace)
		.modify(replaceToken(Token::TokenType::operatorSymbol,"\n",Token::TokenType::newlineSymbol,"(newLine)"))
		.modify(replaceToken(Token::TokenType::operatorSymbol, "(", Token::TokenType::parentheses, "("))
		.modify(replaceToken(Token::TokenType::operatorSymbol, ")", Token::TokenType::parentheses, ")"))
		.modify(replaceToken(Token::TokenType::operatorSymbol, "{", Token::TokenType::parentheses, "{"))
		.modify(replaceToken(Token::TokenType::operatorSymbol, "}", Token::TokenType::parentheses, "}"))
		.modify(replaceToken(Token::TokenType::operatorSymbol, "[", Token::TokenType::parentheses, "["))
		.modify(replaceToken(Token::TokenType::operatorSymbol, "]", Token::TokenType::parentheses, "]"))
		.modify(printTokenStream)
		.modify(combieneNumbersIntoFloats)
		.modify(combieneIntoNamsepace)
		.modify(printTokenStream)
		.flatExecute<GrammarParser::SyntaxNodes::File>([](Tokenizer::TokenStream ts) {
			GrammarParser::Parser parser(ts);
			return parser.parseFile();
		})
		.execute<GrammarParser::SyntaxNodes::File>([](GrammarParser::SyntaxNodes::File root) {
			GrammarParser::SyntaxNodes::PrintVisitor printer;
			root.visit(&printer);
			return root;
		})
		.onError(gereralErrorHandler)
		//TODO comments;
		;
	
}

