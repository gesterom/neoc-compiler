#pragma once

#include <vector>
#include <fstream>
#include <cassert>

#include "IReadStream.h"
#include "Result.h"
#include "Structs.h"

//TODO handle comments
class CharacterStream : IReadStream<Character> {
	std::vector<int8_t> file;
	int64_t index;
	Position cursor;
public:
	CharacterStream(std::string filename);
	virtual bool isEnd(int64_t offset) const;
	virtual Result<Character> getValue() const;
	Result<int8_t> getValue(int64_t n) const;
	virtual Result<std::vector<Character>> getValues(int numberOfElementsToGet) const;
	virtual void popValue(int numberOfElementsToPop);
	size_t size() const;
	Position getCursor() const;
};