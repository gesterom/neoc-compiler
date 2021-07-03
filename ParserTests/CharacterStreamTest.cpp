#include "pch.h"

#include <sstream>

#include "../Parser/pch.h"

TEST(CharacterStream, isEnd_emptyString) {
	std::stringstream ss("");
	Parser::CharacterStream cs(ss, "filename");
	EXPECT_TRUE(cs.isEnd());
}

TEST(CharacterStream, isEnd_5_characterString) {
	std::stringstream ss("aaaaa");
	Parser::CharacterStream cs(ss, "filename");
	cs.popValue(4);
	EXPECT_FALSE(cs.isEnd());
	cs.popValue(1);
	EXPECT_TRUE(cs.isEnd());
}

TEST(CharacterStream, getValues_5characterString) {
	std::string test = "abcdfghijkl";
	std::stringstream ss(test);
	Parser::CharacterStream cs(ss, "filename");
	auto a = cs.getValues(test.size());
	bool testFail = false;
	a.modify([&test](std::vector<Parser::Character> vec) {
		EXPECT_EQ(vec.size(), test.size());
		for (int i = 0; i < vec.size();i++) {
			EXPECT_EQ(vec[i].value, test[i]);
		}
	}).onError([&testFail](Errors::Error er) -> Errors::Error {
		testFail = true;
		return er;
	});
	ASSERT_FALSE(testFail);
}

TEST(CharacterStream, getValue_times8) {
	std::string test = "012345678";
	std::stringstream ss(test);
	Parser::CharacterStream cs(ss, "filename");
	int index = 0;
	while (! cs.isEnd()) {
		cs.getValue().modify([&index](Parser::Character ch) {
			EXPECT_EQ(ch.value, '0' + index++);
		});
		cs.popValue(1);
	}
}