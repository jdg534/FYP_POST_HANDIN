#include "ErrorLog.h"
#include <sstream>
#include <fstream>

#include <Windows.h>


ErrorLog * f_FileScopeErrLogPtr = nullptr;


ErrorLog * ErrorLog::getInstance()
{
	if (f_FileScopeErrLogPtr == nullptr)
	{
		f_FileScopeErrLogPtr = new ErrorLog();
	}
	return f_FileScopeErrLogPtr;
}

ErrorLog::~ErrorLog()
{
	f_FileScopeErrLogPtr = nullptr;
}

void ErrorLog::logMinorError(std::string err)
{
	//lazyInit();

	//m_errorsLogged++;
	std::stringstream s; 
	s << "Error " << m_log.size() + 1 << " minor: " << err << std::endl;
	m_log.push(s.str());
}

void ErrorLog::logFatalError(std::string err)
{
	//lazyInit();
	// m_errorsLogged++;
	std::stringstream s;
	s << "Error " << m_log.size() + 1 << " minor: " << err << std::endl;
	m_log.push(s.str());
	MessageBox(NULL, L"A fatal error has occured, see: errorLog.txt for details.",L"Fatal Error",  MB_OK);
	shutdown();
	exit(-1);
}

void ErrorLog::shutdown()
{
	// just create and use to file output here

	std::ofstream out("errorLog.txt");

	out << "ERROR_LOG_START" << std::endl;
	while (!m_log.empty())
	{
		std::string tmp = m_log.front();
		out << tmp;
		m_log.pop();
	}
	out << "ERROR_LOG_END" << std::endl;

	//(*m_logFile) << "ERROR_LOG_END" << std::endl;
	// m_logFile->close();
}

ErrorLog::ErrorLog()
{
	
}