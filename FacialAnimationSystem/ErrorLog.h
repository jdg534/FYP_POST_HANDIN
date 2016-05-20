#ifndef _ERROR_LOG_H_
#define _ERROR_LOG_H_

#include <string>
// #include <fstream>
#include <queue>

class ErrorLog
{
public:
	static ErrorLog * getInstance();

	~ErrorLog();

	void logMinorError(std::string err); // program keeps running
	void logFatalError(std::string err); // program closes

	void shutdown();
private:
	ErrorLog();

	bool m_initialised;
	// std::ofstream * m_logFile;

	std::queue<std::string> m_log;

	//unsigned int m_errorsLogged;

	//void lazyInit();
};


#endif