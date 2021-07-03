#include "pch.h"
#include "ASTVisitor.h"

#include "SyntaxNodes.h"
namespace Parser {
	namespace GrammarParser {
		std::ostream& PrintVisitor::print() {
			for (int i = 0; i < tab; i++) {
				std::cout << "\t";
			}
			return std::cout;
		}
		void PrintVisitor::visit(SyntaxNodes::File* file) {
			for (auto& item : file->procedures) {
				this->tab++;
				item.visit(this);
				this->tab--;
			}
		}
		void PrintVisitor::visit(SyntaxNodes::Declaration::Procedure* p) {
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
		void PrintVisitor::visit(SyntaxNodes::Statements::Return* r) {
			print() << "[Return] : " << std::endl;
			this->tab++;
			r->exp->visit(this);
			this->tab--;
		}
		void PrintVisitor::visit(SyntaxNodes::Statements::Expression* exp) {
			//print() << "[Return] : " << std::endl;
			exp->exp->visit(this);
		}
		void PrintVisitor::visit(SyntaxNodes::Expressions::Expression*) {}
		void PrintVisitor::visit(SyntaxNodes::Expressions::Literal* l) {
			print() << "Literal : " << l->value << std::endl;
		}
		void PrintVisitor::visit(SyntaxNodes::Expressions::OperatorCall*) {}
		void PrintVisitor::visit(SyntaxNodes::Expressions::ProcedureCall*) {}
		void PrintVisitor::visit(SyntaxNodes::Declaration::Type*) {}
		void PrintVisitor::visit(SyntaxNodes::Statements::VaribleDeclaration* var) {
			print() << var->type << " : " << var->name << std::endl;
		};
		void PrintVisitor::visit(SyntaxNodes::Statements::VaribleDeclDefinition* var) {
			print() << var->type << " : " << var->name << std::endl;
		};
		void PrintVisitor::visit(SyntaxNodes::Statements::Block* block) {
			print() << "[BLOCK]" << std::endl;
			this->tab++;
			for (auto item : block->program) {
				item->visit(this);
			}
			this->tab--;
		};
		void PrintVisitor::visit(SyntaxNodes::Declaration::Varible*) {};
		void PrintVisitor::visit(SyntaxNodes::Declaration::Arguments* a) {
			for (auto& item : a->args) {
				print() << item.type << " : " << item.name << std::endl;
			}
		}
	}
}