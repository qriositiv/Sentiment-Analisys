#include <regex>
#include "IOManager.h"

std::wstring InputManager::getInput(InputManager* in, std::wstring input)
{
	while (true)
	{
		if (input == L"")
		{
			input = in->getInput(L"Enter text/file/url you want to analyse");
		}
		int type = InputManager::detectInputType(input);
		if (type == TextFile)
		{
			in = new FileInput(input);
			input = L"";
			FileOutput::writeToLog(L"Input type: Text file", 1);
			continue;
		}
		if (type == Url)
		{
			in = new WebInput(input);
			input = in->getInput();
			FileOutput::writeToLog(L"Input type: Url", 1);
			break;
		}

		FileOutput::writeToLog(L"Input type: Text", 1);
		break;
	}
	return input;
}

int InputManager::detectInputType(std::wstring input)
{
	std::wregex https(L"[Hh]ttps://.*");
	if (regex_match(input, https))
		return Url;

	std::wregex http(L"http.*");
	std::wregex www(L"www.*");
	if (regex_match(input, http) || regex_match(input, www))
		throw std::wstring(L"Invalid url: Must start with \"https://\"!");

	std::wregex textFilePath(L".*\\.txt$");
	if (regex_match(input, textFilePath))
		return TextFile;

	std::wregex filePath(L"[A-Z]:[\\\\/].*");
	if (regex_match(input, filePath))
		throw std::wstring(L"Invalid input file: Must be .txt file!");
	
	return PlainText;
}



std::vector<std::wstring> InputManager::separateTextIntoWords(std::wstring input)
{
	std::vector<std::wstring> words;
	std::wstring currentWord;

	replace(input.begin(), input.end(), L'\n', L' ');

	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] == L' ')
		{
			while (i + 1 < input.length() && input[i + 1] == L' ')
				i++;

			if (!currentWord.empty())
			{
				transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::towlower);
				words.push_back(currentWord);
				currentWord.clear();
			}
		}
		else
		{
			currentWord += input[i];
		}
	}

	if (!currentWord.empty())
	{
		transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::towlower);

		words.push_back(currentWord);
	}

	return words;
}

#include <iostream>
Text InputManager::separateText(const std::wstring& input)
{
	auto words = std::make_shared<std::vector<std::wstring>>();
	auto sentences = std::make_shared<std::vector<Sentence>>();
	auto paragraphs = std::make_shared<std::vector<Paragraph>>();
	std::wstring currentWord;

	int n = input.length();
	int lastIndex = 0;
	for (int i = 0; i < n; i++)
	{
		char c = input[i];
		if (isWordEnd(c) || isSentenceEnd(c) || isParagraphEnd(c))
		{
			if (i - lastIndex > 0)
			{
				// Add the word to vector of words
				std::wstring temp = input.substr(lastIndex, i - lastIndex);
				words->push_back(temp);
			}
			lastIndex = i + 1;

			if (isSentenceEnd(c) || isParagraphEnd(c))
			{
				if (!words->empty())
				{
					// Add sentence ending symbol as a separate word
					if (isSentenceEnd(c))
						words->push_back(std::wstring(1, c));

					// Construct a sentence and add it to sentence vector
					sentences->push_back(Sentence(words));
					words = std::make_shared<std::vector<std::wstring>>();
				}

				if (isParagraphEnd(c))
				{
					if (!sentences->empty())
					{
						// Construct a paragraph and add it to paragraphs vector
						paragraphs->push_back(Paragraph(sentences));
						// Clear sentences. Do not use .clear() since this is a shared pointer.
						sentences = std::make_shared<std::vector<Sentence>>();
					}
				}
			}
		}
	}

	if (lastIndex != n)
		words->push_back(input.substr(lastIndex));

	if (!words->empty())
		sentences->push_back(Sentence(words));

	if (!sentences->empty())
		paragraphs->push_back(Paragraph(sentences));

	return Text(paragraphs);
}


bool InputManager::isParagraphEnd(wchar_t c)
{
	return c == L'\n' || c == L'\r';
}

bool InputManager::isSentenceEnd(wchar_t c)
{
	return c == L'.' || c == L'!' || c == L'?';
}

bool InputManager::isWordEnd(wchar_t c)
{
	return c == L' ' || c == L'\t';
}


