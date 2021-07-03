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
		Procedure::~Procedure() { delete body; }
	}
	namespace Expressions {
		void ProcedureCall::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void OperatorCall::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		OperatorCall::~OperatorCall() {
			delete left;
			delete right;
		}
		void Literal::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
	}
	namespace Statements {
		void Block::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		Block::~Block() {
			for (auto i : program) {
				delete i;
			}
		}
		Parser::Token type;
		void VaribleDeclaration::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		void VaribleDeclDefinition::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		VaribleDeclDefinition::~VaribleDeclDefinition() {
			delete expr;
		}
		void Return::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		Return::~Return() {
			delete exp;
		}
		void Expression::visit(Parser::GrammarParser::IVisitor* visitor) {
			visitor->visit(this);
		}
		Expression::~Expression() {
			delete exp;
		}
	}
	void File::visit(Parser::GrammarParser::IVisitor* visitor) {
		visitor->visit(this);
	}
}