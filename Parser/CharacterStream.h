#pragma once
#include "Structs.h"

#include "Result.h"

//TODO handle comments
namespace Parser {
	class CharacterStream {
		std::vector<int8_t> file;
		int64_t index;
		Position cursor;
	public:
		CharacterStream(std::istream& file_, std::string filename);
		virtual bool isEnd(int64_t offset = 1) const;
		virtual Result<Character> getValue() const;
		Result<int8_t> getValue(int64_t n) const;
		virtual Result<std::vector<Character>> getValues(int numberOfElementsToGet) const;
		virtual void popValue(int numberOfElementsToPop);
		size_t size() const;
		Position getCursor() const;
	};
}