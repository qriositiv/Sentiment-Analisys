#include <cmath>
#include <iostream>

#include "IOManager.h"

std::wstring TerminalInput::getInput()
{
	return getInput(L"");
}

std::wstring TerminalInput::getInput(std::wstring message)
{
	while (true)
	{
		fflush(stdin);

		std::wcout << message;
		if (message != L"")
			std::wcout << L": ";

		std::wstring input;
		std::getline(std::wcin, input);

		if (!input.empty())
			return input;
	}
	return L"";
}

void TerminalOutput::writeMessage(std::wstring message)
{
	std::wcout << message;
}

void TerminalOutput::writeMessage(Text& text)
{
	writeMessage(text.formatFinalResults());
}

void TerminalOutput::writeMessage(std::vector<EmotionalScore> flow)
{
	writeMessage(Text::formatFlow(flow));
}

void TerminalOutput::writeProgress(int current, int total)
{
	const int stepCount = 50;
	current = current + 1;
	int step = (float)current / total * stepCount;
	float percentage = (float)current / total;

	std::wcout << L"[";
	for (int j = 0; j < stepCount; j++)
	{
		if (j < step)
			std::wcout << L"=";
		else
			std::wcout << L" ";
	}
	std::wcout << L"] " << round(percentage * 100) << L"% " << current << L"/" << total  << L"\r";
	std::wcout.flush();
	if (current == total)
		std::wcout << "\n";
}
