#pragma once
#include <string>
#include <map>

enum FlagsEnum
{
	helpFlag,
	fileFlag,
	urlFlag,
	outputFlag,
	databaseFlag,
	bowFlag,
	vaderFlag,
	resultsParagraphFlag,
	resultsParagraphAndTextFlag,
	resultsSentenceAndTextFlag,
	verboseUrlFlag,
	progressFlag,
	pdfFlag,
	csvFlag,
	topicFlag,
	strongTopicFlag,
	weakTopicFlag,
	flowFlag
};

// DO NOT FORGET TO UPDATE THIS
const static int flagsCount = 17;

class Flag;

class ArgumentHandler
{
private:
	static bool initialised;
	static std::map<int, Flag> flags;

	static void Initialise();

public:
	static void handleArguments(int argc, wchar_t* argv[]);
	static void printHelp();

	/// <summary>
	/// gets pointer to the flag
	/// </summary>
	/// <param name="flag">enum number of a flag searched</param>
	/// <returns>a pointer to the flag</returns>
	static Flag* getFlag(int flag);

	/// <summary>
	/// finds a flag by its name
	/// </summary>
	/// <param name="flag">flag name from enum table</param>
	/// <returns>a pointer to the flag</returns>
	static Flag* findFlag(std::wstring flag);
	static bool isFlagRaised(int flag);
	static std::wstring getFlagParameter(int flag);

	/// <summary>
	/// resets all flag status for testing porpuses
	/// </summary>
	static void reset();

	/// <summary>
	/// gets flag and supplied parameters
	/// </summary>
	/// <returns>std::wstring flag + '\t' + supplied parameters + '|'</returns>
	static std::wstring formatFlagStatus();
};

class Flag
{
private:
	std::wstring flag;
	std::wstring parameter;
	std::wstring description;
	bool mustHaveParameter, raised;
public:
	// Do not use empty default constructor. Added only because removing it creates
	// problems with Utility.h templates, even if they are not used.
	Flag();
	Flag(std::wstring flag, bool mustHaveParameter, std::wstring description);

	std::wstring getFlag();
	std::wstring getParameter();
	std::wstring getDescription();
	bool isRaised();
	void raise(std::wstring parameter = L"");
};