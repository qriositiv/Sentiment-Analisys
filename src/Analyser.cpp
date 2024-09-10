#include <ctime>
#include <sstream>
#include <sys/stat.h>

#include "Analyser.h"
#include "Utilities.h"

void Analyser::writeLog(std::wstring message, bool newLine)
{
	if (log != nullptr)
	{
		log->writeMessageRef(message);
		if (newLine)
			log->writeMessage(L"\n");
	}
}

Analyser::Analyser(Database* database, std::wstring type)
{
	this->database = database;
	log = nullptr;

#ifdef _WIN32
	_wmkdir(L"logs");
#else
	mkdir("logs", 0777);
#endif

	log = new FileOutput(L"logs/" + formatCurrentTime() + L"_" + type + L"_analysis.log");
}