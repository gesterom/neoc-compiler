#pragma once

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
}
namespace Parser {
	namespace GrammarParser {

		class IVisitor {
		public:
			virtual void visit(SyntaxNodes::File*) = 0;
			virtual void visit(SyntaxNodes::Expressions::Expression*) = 0;
			virtual void visit(SyntaxNodes::Expressions::ProcedureCall*) = 0;
			virtual void visit(SyntaxNodes::Expressions::OperatorCall*) = 0;
			virtual void visit(SyntaxNodes::Expressions::Literal*) = 0;
			virtual void visit(SyntaxNodes::Statements::Block*) = 0;
			virtual void visit(SyntaxNodes::Statements::VaribleDeclaration*) = 0;
			virtual void visit(SyntaxNodes::Statements::VaribleDeclDefinition*) = 0;
			virtual void visit(SyntaxNodes::Statements::Return*) = 0;
			virtual void visit(SyntaxNodes::Statements::Expression*) = 0;
			virtual void visit(SyntaxNodes::Declaration::Procedure*) = 0;
			virtual void visit(SyntaxNodes::Declaration::Type*) = 0;
			virtual void visit(SyntaxNodes::Declaration::Varible*) = 0;
			virtual void visit(SyntaxNodes::Declaration::Arguments*) = 0;
		};
		class PrintVisitor : public IVisitor {
			int tab = 0;
		public:
			std::ostream& print();
			virtual void visit(SyntaxNodes::File* file);
			virtual void visit(SyntaxNodes::Expressions::Expression*);
			virtual void visit(SyntaxNodes::Expressions::Literal* l);
			virtual void visit(SyntaxNodes::Expressions::OperatorCall*);
			virtual void visit(SyntaxNodes::Expressions::ProcedureCall*);
			virtual void visit(SyntaxNodes::Statements::VaribleDeclaration* var);
			virtual void visit(SyntaxNodes::Statements::VaribleDeclDefinition* var);
			virtual void visit(SyntaxNodes::Statements::Block* block);
			virtual void visit(SyntaxNodes::Statements::Return* r);
			virtual void visit(SyntaxNodes::Statements::Expression*);
			virtual void visit(SyntaxNodes::Declaration::Procedure* p);
			virtual void visit(SyntaxNodes::Declaration::Type*);
			virtual void visit(SyntaxNodes::Declaration::Varible*);
			virtual void visit(SyntaxNodes::Declaration::Arguments* a);
		};
	}
}
