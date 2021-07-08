#pragma once

#include <memory>

#include "Errors.h"	
#include "Logger.h"

template<typename ResultType>
class Result {
	std::shared_ptr<ResultType> res;
	Errors::Error error;
	bool isOk = false;
public:
	Result() : Result(Errors::Error{}) {} 
	Result(ResultType value) {
		res = std::make_shared<ResultType>(value);
		assert(res != nullptr);
		//*res = value;
		isOk = true;
		//error = nullptr;
	}
	Result(ResultType* value) {
		assert(value != nullptr);
		res.reset(value);
		isOk = true;
		//error = nullptr;
	}
	Result(Errors::Error value) {
		res = nullptr;
		isOk = false;
		error = value;
	}

	Result(const Result<ResultType>& other){
		if (other.isOk == false) {
			this->res = nullptr;
			this->error = other.error;
			this->isOk = false;
			return;
		}
		else if(other.res != nullptr){
			res = other.res;
			assert(res != nullptr);
			isOk = true;
		}
		else {
			//INTERNAL
			exit(-2);
		}
	}
	Result<ResultType>& operator=(const Result<ResultType>& other) {
		if (other.isOk == false) {
			this->error = other.error;
			this->isOk = false;
			this->res = nullptr;
			return *this;
		}
		else if (other.res != nullptr) {
			res = other.res;
			assert(res != nullptr);
			isOk = true;
			return *this;
		}
		else {
			//INTERNAL
			exit(-2);
		}
	}

	Result(Result<ResultType>&& other) noexcept {
		if (this == &other) return;
		this->res = other.res;
		this->error = other.error;
		this->isOk = other.isOk;
		other.res = nullptr;
	}

	Result<ResultType>& operator=(Result<ResultType>&& other) noexcept {
		if (this == &other) return *this;
		this->res = other.res;
		this->error = other.error;
		this->isOk = other.isOk;
		other.res = nullptr;
		return *this;
	}

	bool isError() const { return !isOk; }

	template<typename T>
	Result<T> execute(std::function<T(const ResultType&)> function) {
		if (this->isError() == false) {
			return Result<T>(function(*(this->res)));
		}
		return Result<T>{ this->error };
	}

	template<typename T>
	Result<T> operator&(std::function<T(const ResultType&)> function) {
		return this->execute<T>(function);
	}

	template<typename T>
	Result<T> flatExecute(std::function<Result<T>(const ResultType&)> function) {
		if (this->isError() == false) {
			return (function(this->res));
		}
		return Result<T>{ this->error };
	}

	Result<ResultType>& modify(std::function<void(ResultType&)> func) {
		if (this->isError() == false) {
			(func(*this->res));
			return *this;
		}
		return *this;
	}

	template<typename T>
	T execute(std::function<T(const ResultType&)> function, std::function<T(Errors::Error)> errorFunction) {
		if (this->isError()) {
			return errorFunction(this->error);
		}
		else {
			return function(*(this->res));
		}
	}

	Result<ResultType>& onError(std::function<Errors::Error(Errors::Error)> function) {
		if (this->isError() == true) {
			this->error = function(this->error);
			return *this;
		}
		return *this;
	}

	template<typename T>
	Result<T> carryError() {
		assert(this->isError());
		return Result<T>(this->error);
	}

	template<typename T, typename R>
	Result<T> combine(std::function<T(const ResultType&, R)> func, Result<R> right) {
		return right.execute< Result<T> >(
			[this, func](R& rv) {
			if (this->isError()) return this->carryError<T>();
			else return Result<T>(func(this->res, rv));
		},
			[this](Errors::Error er) {
			if (this->isError()) return Result<T>(Errors::CombineError(this->error, er));
			else return Result<T>(er);
		});
	}

	Result<ResultType>& createError(std::function<Errors::Error(const ResultType&)> func) {
		if (this->isError())
			return *this;
		else
		{
			this->error = (func(*(this->res)));
			this->isOk = false;
			return *this;
		}
	}


	//ResultType* valueOr(ResultType* defaultValue) {
	//	if (this->isError()) return defaultValue;
	//	else return (this->res);
	//}

	ResultType& unpack() {
		if (this->isError()) {
			//internall
			exit(-1);
		}
		else {
			return *(this->res);
		}
	}
	std::shared_ptr<ResultType>& unpackVirtual() {
		if (this->isError()) {
			//internall
			exit(-2);
		}
		else {
			return this->res;
		}
	}

	template<typename T, typename R>
	Result<T> flatCombine(std::function<Result<T>(const ResultType&, R)> func,const Result<R>& right) {
		if (this->isError() and right.isError()) {
			return Result<T>(Errors::CombineError(this->error, right.error));
		}
		if (this->isError()) {
			return Result<T>(this->error);
		}
		if (right.isError()) {
			return Result<T>(right.error);
		}
		return (func(*(this->res), *(right.res)));
	}
	Result<ResultType> select(const Result<ResultType>& right, std::function<ResultType(const ResultType&,const ResultType&)> func) {
		if (this->isError() and right.isError()) {
			return Result<ResultType>(Errors::CombineError(this->error, right.error));
		}
		if (this->isError()) {
			return Result<ResultType>(right.res);
		}
		if (right.isError()) {
			return Result<ResultType>(this->res);
		}
		return Result<ResultType>(func(*(this->res), *(right.res)));
	}
	Result<ResultType> flatSelect(const Result<ResultType>& right, std::function<Result<ResultType>(const ResultType&,const ResultType&)> func) {
		if (this->isError() and right.isError()) {
			return Result<ResultType>(Errors::CombineError(this->error, right.error));
		}
		if (this->isError()) {
			return right;
		}
		if (right.isError()) {
			return *this;
		}
		return (func(*(this->res),*( right.res)));
	}
	Result<ResultType> solveError(std::function<ResultType()> func) {
		if (this->isError()) {
			return Result<ResultType>(func());
		}
		return *this;
	}



	bool operator==(const ResultType& other) const {
		if (this->isError()) return false;
		else
			return this->res == other;
	}
	std::string toString() const {
		if (res->isError()) {
			return res->error.toString();
		}
		else {
			return res->res;
		}
	}

	operator bool() const {
		return (this->isOk);
	}

};
