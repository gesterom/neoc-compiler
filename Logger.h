#pragma once
#include <iostream>

class Logger {
	int counter = 0;
	Logger() {}
	std::string tabPadding() {
		std::string res;
		for (int i = 0; i < counter; i++) {
			res += "\t";
		}
		return res;
	}
public:
	static Logger& getLogger() {
		static Logger log;
		return log;
	}
	void log(std::string filename, int lineNumber, std::string var) {
		std::cout << tabPadding() << "[" << filename << "] : {" << lineNumber << "}  " << (var) << std::endl;
	}
	void logLine(std::string filename, int lineNumber, std::string functionName) {
		std::cout << tabPadding() << "[" << filename << "] : {" << lineNumber << "}  " << functionName << std::endl;
	}
	template<typename T>
	void logVar(std::string varibleName, T varibleValue) {
		std::cout << tabPadding() << "VAR : " << (varibleName) << " { " << (varibleValue) << " }" << std::endl;
	}
	void logBool(std::string varibleName, bool varibleValue) {
		std::cout << tabPadding() << "VAR : " << (varibleName) << " { " << ((varibleValue) ? "True" : "False") << " }" << std::endl;
	}
	void logPreCall(std::string filename, int lineNumber) {
		std::cout << tabPadding() << "[" << filename << "] : {" << lineNumber << "}" << std::endl;
		counter++;
	}
	void logPostCall(std::string filename, int lineNumber) {
		counter--;
		std::cout << tabPadding() << "[" << filename << "] : {" << lineNumber << "}" << std::endl;
	}
};

#define debug(var)			do{Logger::getLogger().log			(__FILE__,__LINE__,(var));}while(false)
#define debugLine()			do{Logger::getLogger().logLine		(__FILE__,__LINE__,__FUNCSIG__);}while(false)
#define debugCall(var)		do{Logger::getLogger().logPreCall	(__FILE__,__LINE__);(var);Logger::getLogger().logPostCall(__FILE__,__LINE__);}while(false)
#define debugPreCall()		do{Logger::getLogger().logPreCall	(__FILE__, __LINE__);}while(false)
#define debugPostCall()		do{Logger::getLogger().logPostCall	(__FILE__, __LINE__);}while(false)
#define debugVar(var)		do{Logger::getLogger().logVar		((#var),(var));}while(false)
#define debugBool(var)		do{Logger::getLogger().logBool		((#var),(var));}while(false)