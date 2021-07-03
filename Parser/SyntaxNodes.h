#pragma once

#include "pch.h"

#include "ASTVisitor.h"

namespace SyntaxNodes {

	namespace Declaration {
		class Procedure;
		class Type;
		class Varible;
		class Arguments;
	}
	namespace Expressions {
		class Expression;
		class Literal;
		class ProcedureCall;
		class OperatorCall;
	}
	namespace Statements {
		class Statement;
		class Block;
		class VaribleDeclaration;
		class VaribleDeclDefinition;
		class Return;
		class Expression;
	}
	class File;
	class Node;


	class Node {
	public:
		virtual void visit(Parser::GrammarParser::IVisitor* visitor) = 0;
		virtual ~Node() {}
	};
	//c definiton

	namespace Declaration {
		class Type : public Node {
		public:
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class Varible : public Node {
		public:
			Parser::Token type;
			Parser::Token name;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class Arguments : public Node {
		public:
			std::vector<Declaration::Varible> args;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class Procedure : public Node {
		public:
			Parser::Token name;
			Arguments args;
			Parser::Token returnType;
			Statements::Statement* body = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
			virtual ~Procedure();
		};
	};
	namespace Expressions {
		class Expression : public Node {};

		class ProcedureCall : public Expression {
		public:
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class OperatorCall : public Expression {
		public:
			Expressions::Expression* left = nullptr;
			Expressions::Expression* right = nullptr;
			Parser::Token op;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
			virtual ~OperatorCall();
		};
		class Literal : public Expression {
		public:
			Parser::Token value;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
	}
	namespace Statements {
		class Statement : public Node {};
		class Block : public Statement {
		public:
			std::vector<Statement*> program;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
			virtual ~Block();
		};
		class VaribleDeclaration : public Statement {
		public:
			Parser::Token name;
			Parser::Token type;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class VaribleDeclDefinition : public Statement {
		public:
			Parser::Token name;
			Parser::Token type;
			Expressions::Expression* expr = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
			virtual ~VaribleDeclDefinition();
		};
		class Return : public Statement {
		public:
			Expressions::Expression* exp = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
			virtual ~Return();
		};
		class Expression : public Statement {
		public:
			Expressions::Expression* exp = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
			virtual ~Expression();
		};
	}
	class File : public Node {
	public:
		std::vector<Declaration::Procedure> procedures;
		std::vector < Declaration::Type > types;
		virtual void visit(Parser::GrammarParser::IVisitor* visitor);
	};

}
