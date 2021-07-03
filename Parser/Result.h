#pragma once

#include "Errors.h"	

template<typename ResultType>
class Result {
	ResultType res;
	Errors::Error error;
	bool isOk;
	Result() = delete;
public:
	Result(ResultType value) {
		res = value;
		isOk = true;
		//error = nullptr;
	}
	Result(Errors::Error value) {
		//res = nullptr;
		isOk = false;
		error = value;
	}

	bool isError() { return ! isOk; }

	template<typename T>
	Result<T> execute(std::function<T(ResultType)> function) {
		if (this->isError() == false) {
			return Result<T>(function(this->res));
		}
		return Result<T>{ this->error };
	}
	template<typename T>
	Result<T> flatExecute(std::function<Result<T>(ResultType)> function) {
		if (this->isError() == false) {
			return (function(this->res));
		}
		return Result<T>{ this->error };
	}

	Result<ResultType> modify(std::function<void(ResultType&)> func) {
		if (this->isError() == false) {
			(func(this->res));
			return *this;
		}
		return *this;
	}

	template<typename T>
	T execute(std::function<T(ResultType)> function, std::function<T(Errors::Error)> errorFunction) {
		if (this->isError()) {
			return errorFunction(this->error);
		}
		else {
			return function(this->res);
		}
	}
	Result<ResultType> onError(std::function<Errors::Error(Errors::Error)> function) {
		if (this->isError() == true) {
			return Result<ResultType>(function(this->error));
		}
		return  this->res;
	}
	template<typename T>
	Result<T> carryError() {
		return Result<T>(this->error);
	}
	template<typename T, typename R>
	Result<T> combine(std::function<T(ResultType, R)> func, Result<R> right) {
		return right.execute< Result<T> >(
			[this, func](R rv) {
			if (this->isError()) return this->carryError<T>();
			else return Result<T>(func(this->res, rv));
		},
			[this](Errors::Error er) {
			if (this->isError()) return Result<T>(Errors::CombineError(this->error, er));
			else return Result<T>(er);
		});
	}

	Result<ResultType> createError(std::function<Errors::Error(ResultType)> func) {
		if (this->isError())
			return *this;
		else
			return Result<ResultType>(func(this->res));
	}

	ResultType unpack() {
		if (this->isError()) {
			exit(-1);
		}
		else {
			return this->res;
		}
	}

	template<typename T, typename R>
	Result<T> flatCombine(std::function<Result<T>(ResultType, R)> func, Result<R> right) {
		if (this->isError() and right.isError()) {
			return Result<T>(Errors::CombineError(this->error, right.error));
		}
		if (this->isError()) {
			return Result<T>(this->error);
		}
		if (right.isError()) {
			return Result<T>(right.error);
		}
		return (func(this->res, right.res));
	}
	Result<ResultType> select(Result<ResultType> right, std::function<ResultType(ResultType, ResultType)> func) {
		if (this->isError() and right.isError()) {
			return Result<ResultType>(Errors::CombineError(this->error, right.error));
		}
		if (this->isError()) {
			return Result<ResultType>(right.res);
		}
		if (right.isError()) {
			return Result<ResultType>(this->res);
		}
		return Result<ResultType>(func(this->res, right.res));
	}
	Result<ResultType> flatSelect(Result<ResultType> right, std::function<Result<ResultType>(ResultType, ResultType)> func) {
		if (this->isError() and right.isError()) {
			return Result<ResultType>(Errors::CombineError(this->error, right.error));
		}
		if (this->isError()) {
			return Result<ResultType>(right.res);
		}
		if (right.isError()) {
			return Result<ResultType>(this->res);
		}
		return (func(this->res, right.res));
	}
	Result<ResultType> solveError(std::function<ResultType()> func) {
		if (this->isError()) {
			return Result<ResultType>(func());
		}
		return *this;
	}
	ResultType valueOr(ResultType defaultValue) {
		if (this->isError()) return defaultValue;
		else return this->res;
	}
	bool operator==(ResultType other) {
		if (this->isError()) return false;
		else
			return this->res == other;
	}
	std::string toString() {
		if (res->isError()) {
			return res->error.toString();
		}
		else {
			return res->res;
		}
	}
};