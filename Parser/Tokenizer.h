#pragma once
#include "Structs.h"
#include "TokenStream.h"

namespace Parser {
	class Tokenizer
	{
		struct expectTokenRes;
	public:
		struct expectTokenRes {
			bool matched;
			Token token;
			int consumed;
			expectTokenRes();
			expectTokenRes(Token token);
			expectTokenRes(Token token, int consumed);
		};
		Result<TokenStream> tokenize(CharacterStream cs);
		Result<Token> getToken(CharacterStream& cs);
	};
}