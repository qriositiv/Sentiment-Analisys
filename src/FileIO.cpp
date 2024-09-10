#include <fstream>
#include <sstream>
#include <string>
#include <locale>
#include <codecvt>
#include <sys/stat.h>
#include <iostream>

#include "IOManager.h"
#include "Utilities.h"
#include "Arguments.h"

FileInput::FileInput(std::wstring filepath)
{
	open(filepath);
}

FileInput::~FileInput()
{
	close();
}

std::wstring FileInput::getInput()
{
	std::wstringstream input;
	input << file.rdbuf();

	return input.str();
}

std::wstring FileInput::getInput(std::wstring message)
{
	return getInput();
}

void FileInput::open(std::wstring filepath)
{
	file.open(wstrToStr(filepath), std::wifstream::in);

	file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>()));

	if (file.fail())
	{
		throw std::wstring(L"Unable to open input file: " + filepath);
	}
}

void FileInput::close()
{
	file.close();
}



FileOutput::FileOutput(std::wstring filepath)
{
	open(filepath);
}

FileOutput::~FileOutput()
{
	close();
}

FileOutput* FileOutput::log = nullptr;
bool FileOutput::logStarted = false;

void FileOutput::startLog()
{
	if (logStarted)
		return;

#ifdef _WIN32
	_wmkdir(L"logs");
#else
	mkdir("logs", 0777);
#endif
	log = new FileOutput(L"logs/" + formatCurrentTime() + L".log");
	logStarted = true;
}

void FileOutput::writeMessage(std::wstring message)
{
	file << message;
	file.flush();
}

void FileOutput::writeMessage(Text& text)
{
	writeMessage(text.formatFinalResults());
}

void FileOutput::writeMessage(std::vector<EmotionalScore> flow)
{
	writeMessage(Text::formatFlow(flow));
}

void FileOutput::writeMessageRef(std::wstring& message)
{
	file << message;
	file.flush();
}

void FileOutput::open(std::wstring filepath)
{
	file.open(wstrToStr(filepath), std::wfstream::out);

	file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>()));

	if (file.fail())
	{
		throw std::wstring(L"Unable to open output file: " + filepath);
	}
}

void FileOutput::writeToLog(std::wstring message, bool endLine)
{
	startLog();
	log->writeMessage(L"[" + formatCurrentTime() + L"] " + message);
	if (endLine)
		log->writeMessage(L"\n");
}

void FileOutput::writeToLogNoFormatting(std::wstring message)
{
	startLog();
	log->writeMessage(message);
}

void FileOutput::close()
{
	file.close();
}