#define CURL_STATICLIB

#include <stdlib.h>
#include <fstream>
#include <fcntl.h>
#include <iostream>

#include "IOManager.h"
#include "Database.h"
#include "Analyser.h"
#include "Arguments.h"
#include "Utilities.h"
#include "Topic.h"

#include "CURL/curl.h"

#ifdef _WIN32
#include <io.h>
#else //Linux
#include <sys/stat.h>
#define wmain main2
#endif

void cleanExit(int exitCode);
int wmain(int argc, wchar_t* argv[]);

// Linux does not support wmain
#ifdef __linux__
int main(int argc, char* argv[])
{
	return wmain(argc, charArrayToWcharArray(argc, argv));
}
#endif

int wmain(int argc, wchar_t* argv[])
{
	FileOutput::startLog();
#ifdef _WIN32
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
#endif

	curl_global_init(CURL_GLOBAL_DEFAULT);
	InputManager* in = new TerminalInput();
	OutputManager* out = new TerminalOutput();
	std::wstring input = L"";
	std::wstring databasePath = L"";

	try 
	{
		ArgumentHandler::handleArguments(argc, argv);

		if (ArgumentHandler::isFlagRaised(helpFlag))
		{
			ArgumentHandler::printHelp();
			cleanExit(EXIT_SUCCESS);
		}

		if (ArgumentHandler::isFlagRaised(fileFlag))
			in = new FileInput(ArgumentHandler::getFlagParameter(fileFlag));
		if (ArgumentHandler::isFlagRaised(urlFlag))
			in = new WebInput(ArgumentHandler::getFlagParameter(urlFlag));
		if (ArgumentHandler::isFlagRaised(outputFlag))
			out = new FileOutput(ArgumentHandler::getFlagParameter(outputFlag));
		if (ArgumentHandler::isFlagRaised(pdfFlag))
			out = new PdfOutput(ArgumentHandler::getFlagParameter(pdfFlag));
		if (ArgumentHandler::isFlagRaised(databaseFlag))
			databasePath = ArgumentHandler::getFlagParameter(databaseFlag);

		if (ArgumentHandler::isFlagRaised(fileFlag) + ArgumentHandler::isFlagRaised(urlFlag) != 0)
		{
			FileOutput::writeToLog(L"Input was from a command line argument", 1);
		}
		else
		{
			FileOutput::writeToLog(L"Input was given during runtime", 1);
		}
	}
	catch (std::wstring e)
	{
		std::wcerr << e << "\n";
		cleanExit(EXIT_FAILURE);
	}

	Database database(databasePath);

	try
	{
		// Get input from terminal/file/url automatically
		input = InputManager::getInput(in, input);

		//FileOutput::writeToLog(L"Input: " + input, 1);
	}
	catch (std::wstring e)
	{
		std::wcerr << "Invalid input: " << e << "\n";
		cleanExit(EXIT_FAILURE);
	}
	
	try
	{
		Analyser* analyser;

		FileOutput::writeToLog(L"Input seperation: ", 0);

		Text text = InputManager::separateText(input);
		EmotionalScore results;
		Topic* topic;

		if (ArgumentHandler::isFlagRaised(topicFlag))
		{
			topic = new Topic(Topic::createTopic(ArgumentHandler::getFlagParameter((topicFlag))));
			analyser = new TopicAnalyser(&database, *topic);
		}
		else if (ArgumentHandler::isFlagRaised(strongTopicFlag))
		{
			if (ArgumentHandler::isFlagRaised(weakTopicFlag))
			{
				topic = new Topic(Topic::createTopic(ArgumentHandler::getFlagParameter(strongTopicFlag), ArgumentHandler::getFlagParameter(weakTopicFlag)));
			}
			else
			{
				topic = new Topic(Topic::createTopic(ArgumentHandler::getFlagParameter(strongTopicFlag)));
			}
			analyser = new TopicAnalyser(&database, *topic);
		}
		else if (ArgumentHandler::isFlagRaised(bowFlag))
		{
			FileOutput::writeToLog(L"Analyser used: BoW", 1);
			
			analyser = new BowAnalyser(&database);
		}
		else
		{
			FileOutput::writeToLog(L"Analyser used: VADER", 1);
			
			analyser = new VaderAnalyser(&database);
		}
		
		results = analyser->analyse(text);
		out->writeMessage(text);
		
		if (ArgumentHandler::isFlagRaised(flowFlag) || ArgumentHandler::isFlagRaised(pdfFlag))
		{
			out->writeMessage(text.calculateFlow());
		}

		if (ArgumentHandler::isFlagRaised(pdfFlag)) // finish writing to pdf
		{
			((PdfOutput*)out)->saveFile();
		}
	}
	catch (std::wstring e)
	{
		std::wcerr << e;
		FileOutput::writeToLog(L"Error message: " + e, 1);
	}
	catch (...)
	{
		FileOutput::writeToLog(L"Others errors caught!", 1);
		cleanExit(EXIT_FAILURE);
	}

	cleanExit(EXIT_SUCCESS);
	return 0;
}

void cleanExit(int exitCode)
{
	if (exitCode == EXIT_FAILURE)
	{
		FileOutput::writeToLog(L"Exit failure", 1);
	}
	else
	{
		FileOutput::writeToLog(L"Exit success", 1);
	}
		
	curl_global_cleanup();
	exit(exitCode);
}
