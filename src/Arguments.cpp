#include "Arguments.h"
#include "IOManager.h"
#include <iostream>
#include <sstream>

bool ArgumentHandler::initialised = false;
std::map<int, Flag> ArgumentHandler::flags;

void ArgumentHandler::Initialise()
{
	if (initialised)
		return;

	flags[helpFlag] = Flag(L"-h", false, L"show possible flags and their parameters.");
	flags[fileFlag] = Flag(L"-f", true, L"[file] indicate input file to read.");
	flags[urlFlag] = Flag(L"-u", true, L"[url] indicate website to scrape and use as input.");
	flags[outputFlag] = Flag(L"-o", true, L"[file] indicate output file to write to.");
	flags[databaseFlag] = Flag(L"-d", true, L"[file] indicate database file to read.");
	flags[bowFlag] = Flag(L"-bow", false, L"use BoW model for sentiment analysis.");
	flags[vaderFlag] = Flag(L"-vader", false, L"use VADER model for sentiment analysis.");
	flags[resultsParagraphFlag] = Flag(L"-vp", false, L"show results of each paragraph.");
	flags[resultsParagraphAndTextFlag] = Flag(L"-vvp", false, L"show each paragraph and its results.");
	flags[resultsSentenceAndTextFlag] = Flag(L"-vs", false, L"show each sentence and its results.");
	flags[verboseUrlFlag] = Flag(L"-vu", false, L"verbose url, show curl output.");
	flags[progressFlag] = Flag(L"-p", false, L"show progress bar while calculating the score.");
	flags[pdfFlag] = Flag(L"-pdf", true, L"[file] write results to a pdf file.");
	flags[csvFlag] = Flag(L"-csv", false, L"output results in csv");
	flags[topicFlag] = Flag(L"-t", true, L"[file] or [\"word1 word2 ...\"] indicate topic file to read or list of strong words.");
	flags[strongTopicFlag] = Flag(L"-ts", true, L"[\"word1 word2 ...\"] input list of strong words.");
	flags[weakTopicFlag] = Flag(L"-tw", true, L"[\"word1 word2 ...\"] input list of weak words.");
	flags[flowFlag] = Flag(L"-flow", false, L"Shows the analysed text flow");

	initialised = true;
}

void ArgumentHandler::handleArguments(int argc, wchar_t* argv[])
{
	Initialise();

	if (argc < 2)
		return;

	if (argv[1][0] != L'-')
	{
		int type;
		try
		{
			type = InputManager::detectInputType(argv[1]);
		}
		catch (std::wstring e)
		{
			throw std::wstring(L"Invalid argument: " + e);
		}

		if (type == Url)
			flags[urlFlag].raise(argv[0]);
		if (type == TextFile)
			flags[fileFlag].raise(argv[0]);
	}

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] != L'-')
			continue;

		Flag* flag = findFlag(argv[i]);
		if (flag == nullptr)
		{
			throw std::wstring(L"Invalid argument: " + (std::wstring)argv[i] + L" flag not recognised!");
		}

		if (i + 1 != argc && argv[i + 1][0] != '-')
		{
			flag->raise(argv[i + 1]);
			i++;
		}
		else
		{
			flag->raise(L"");
		}
	}

	FileOutput::writeToLog(L"Raised flags and parameters: " + formatFlagStatus(), 1);
	
	if (flags[fileFlag].isRaised() + flags[urlFlag].isRaised() > 1)
		throw std::wstring(L"Only one of -f and -u flags can be raised at the same time!");

	if (flags[bowFlag].isRaised() + flags[vaderFlag].isRaised() > 1)
		throw std::wstring(L"Only one of --bow and --vader flags can be raised at the same time!");
		
	if (flags[resultsParagraphFlag].isRaised() + flags[resultsParagraphAndTextFlag].isRaised() + flags[resultsSentenceAndTextFlag].isRaised() > 1)
		throw std::wstring(L"Only one of -vp, -vvp, -vs flags can be raised at the same time!");

	if (flags[outputFlag].isRaised() + flags[pdfFlag].isRaised() > 1)
		throw std::wstring(L"Only one of -o, -pdf flags can be raised at the same time!");

	if (flags[csvFlag].isRaised() + flags[resultsParagraphAndTextFlag].isRaised() > 1)
		throw std::wstring(L"Can not use -csv and -vvp at the same time! Try -csv with -vp");

	if (flags[topicFlag].isRaised() + flags[strongTopicFlag].isRaised() > 1)
		throw std::wstring(L"Can not use -t and -ts at the same time! Try -ts and -tw");

	if (flags[topicFlag].isRaised() + flags[weakTopicFlag].isRaised() > 1)
		throw std::wstring(L"Can not use -t and -tw at the same time! Try -ts and -tw");

	if (flags[topicFlag].isRaised() + flags[strongTopicFlag].isRaised() < 1 && flags[weakTopicFlag].isRaised())
		throw std::wstring(L"-tw flag can not be rased without -ts flag!");

	if (flags[bowFlag].isRaised() && flags[topicFlag].isRaised() + flags[strongTopicFlag].isRaised() + flags[weakTopicFlag].isRaised() > 1)
		throw std::wstring(L"--bow flag cannot be raised with -t, -ts or -tw");
}

void ArgumentHandler::printHelp()
{
	for (std::map<int, Flag>::iterator i = flags.begin(); i != flags.end(); i++)
	{
		std::wcout << "\t" << i->second.getFlag() << "\t " << i->second.getDescription() << "\n\n";
	}
}

Flag* ArgumentHandler::getFlag(int flag)
{
	Initialise();

	return &flags[flag];
}

Flag* ArgumentHandler::findFlag(std::wstring flag)
{
	for (std::map<int, Flag>::iterator i = flags.begin(); i != flags.end(); i++)
	{
		if (i->second.getFlag() == flag)
			return &(i->second);
	}
	return nullptr;
}

bool ArgumentHandler::isFlagRaised(int flag)
{
	return getFlag(flag)->isRaised();
}

std::wstring ArgumentHandler::getFlagParameter(int flag)
{
	return getFlag(flag)->getParameter();
}

void ArgumentHandler::reset()
{
	initialised = false;
	Initialise();
}

std::wstring ArgumentHandler::formatFlagStatus()
{
	std::wstringstream stream;
	
	for (std::map<int, Flag>::iterator i = flags.begin(); i != flags.end(); i++)
	{
		if (i->second.isRaised())
		{
			stream << i->second.getFlag() << "\t " << i->second.getParameter() + L" | ";
		}
	}

	return stream.str();
}

Flag::Flag()
{
	this->flag = L"";
	this->parameter = L"";
	this->description = L"";
	this->raised = false;
	this->mustHaveParameter = false;
}

Flag::Flag(std::wstring flag, bool mustHaveParameter, std::wstring description)
{
	this->flag = flag;
	this->parameter = L"";
	this->description = description;
	this->raised = false;
	this->mustHaveParameter = mustHaveParameter;
}

void Flag::raise(std::wstring parameter)
{
	if (mustHaveParameter && parameter == L"")
		throw std::wstring(L"Invalid argument: " + flag + L" flag must have a parameter!");

	if (raised && mustHaveParameter)
		throw std::wstring(L"Invalid argument: " + flag + L" raised several times with different parameters");

	raised = true;
	this->parameter = parameter;
}

bool Flag::isRaised()
{
	return raised;
}

std::wstring Flag::getFlag()
{
	return flag;
}

std::wstring Flag::getParameter()
{
	return parameter;
}

std::wstring Flag::getDescription()
{
	return description;
}
