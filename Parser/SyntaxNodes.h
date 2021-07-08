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
			std::shared_ptr<Statements::Statement> body = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
	};
	namespace Expressions {
		class Expression : public Node {
		public:
		};

		class ProcedureCall : public Expression {
		public:
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class OperatorCall : public Expression {
		public:
			std::shared_ptr< Expressions::Expression> left = nullptr;
			std::shared_ptr< Expressions::Expression> right = nullptr;
			Parser::Token op;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class Literal : public Expression {
		public:
			Parser::Token value;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
	}
	namespace Statements {
		class Statement : public Node {
		public:
		};
		class Block : public Statement {
		public:
			std::vector<std::shared_ptr< Statement > > program;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
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
			std::shared_ptr< Expressions::Expression> expr = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class Return : public Statement {
		public:
			std::shared_ptr< Expressions::Expression> exp = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
		class Expression : public Statement {
		public:
			std::shared_ptr< Expressions::Expression> exp = nullptr;
			virtual void visit(Parser::GrammarParser::IVisitor* visitor);
		};
	}
	class File : public Node {
	public:
		std::vector<Declaration::Procedure> procedures;
		std::vector < Declaration::Type > types;
		virtual void visit(Parser::GrammarParser::IVisitor* visitor);
	};

}
