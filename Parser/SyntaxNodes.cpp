#include "pch.h"
#include "SyntaxNodes.h"
#include "ASTVisitor.h"

namespace SyntaxNodes {
	namespace Declaration {
		void Type::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void Varible::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void Arguments::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void Procedure::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
	}
	namespace Expressions {
		void ProcedureCall::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void OperatorCall::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void Literal::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
	}
	namespace Statements {
		void Block::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void VaribleDeclaration::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void VaribleDeclDefinition::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void Return::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void Expression::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
	}
	void File::visit(Parser::GrammarParser::IVisitor* visitor) {
		visitor->visit(this);
	}
}