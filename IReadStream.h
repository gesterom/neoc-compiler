#pragma once
#include "Result.h"

template<typename ValueType>
class IReadStream {
public:
	virtual bool isEnd(int64_t) const = 0;
	virtual Result<ValueType>
		getValue() const = 0;
	virtual Result<std::vector<ValueType>> getValues(int numberOfElementsToGet) const = 0;
	virtual void popValue(int numberOfElementsToPop) = 0;
};