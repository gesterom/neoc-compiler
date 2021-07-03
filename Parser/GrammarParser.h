#pragma once
namespace Parser {
	namespace GrammarParser {
		class GrammarParser
		{
			TokenStream ts;



		public: 
			GrammarParser(TokenStream ts);
			SyntaxNodes::File* parseFile();
		};
	}
}
