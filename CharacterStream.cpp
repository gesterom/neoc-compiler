#include "CharacterStream.h"

CharacterStream::CharacterStream(std::string filename) {
	std::fstream file_(filename);
	char ch;
	while (file_.get(ch).good()) {
		if (ch == EOF) break;
		if (ch == '\n' and file[file.size() - 1] == '\r') {
			file[file.size() - 1] = '\n';
		}
		else
			file.push_back(ch);
	}
	debugVar((file_.good()?"TRUE" : "FALSE"));
	index = 0;
	cursor.line = 1;
	cursor.column = 1;
	cursor.filename = filename;
}
bool CharacterStream::isEnd(int64_t offset) const {
	assert(offset != 0);
	if (offset == 0)
		exit(1);
	return (this->index + offset) > file.size();
}
Result<Character> CharacterStream::getValue() const {
	if (this->isEnd(1)) {
		return Result<Character>(Errors::UnexpectedEndOfFile(cursor));
	}
	int8_t ch = file[this->index];
	return Result<Character>(Character{ ch,cursor });
}
Result<int8_t> CharacterStream::getValue(int64_t n) const {
	//debugLine();
	assert(n < INT64_MAX);
	if (this->isEnd(n + 1)) {
		return Result<int8_t>(Errors::UnexpectedEndOfFile(cursor));
	}

	int8_t ch = file[this->index + n];
	return Result<int8_t>(ch);
}
Result<std::vector<Character>> CharacterStream::getValues(int numberOfElementsToGet) const {
	if (this->isEnd(numberOfElementsToGet)) {
		return  Result<std::vector < Character >>{ Errors::UnexpectedEndOfFile(cursor)};
	}
	std::vector<Character> res;
	Position pos = cursor;
	for (int i = 0; i < numberOfElementsToGet; i++) {
		int8_t ch = file[this->index + i];
		res.push_back(Character{ ch,pos });

		if (ch == '\n') {
			pos.line++;
			pos.column = 0;
		}
		pos.column++;
	}
	return Result<std::vector < Character >>(res);
}
void CharacterStream::popValue(int numberOfElementsToPop) {
	for (int i = 0; i < numberOfElementsToPop; i++) {
		if (this->index + i >= file.size()) { this->index += i;return; }

		int8_t ch = file[this->index + i];
		if (ch == '\n') {
			cursor.line++;
			cursor.column = 0;
		}
		cursor.column++;
	}
	this->index += numberOfElementsToPop;
	return;
}
size_t CharacterStream::size() const {
	return file.size();
}
Position CharacterStream::getCursor() const {
	return this->cursor;
}