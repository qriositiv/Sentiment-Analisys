#include "Topic.h"
#include "IOManager.h"
#include "Arguments.h"

Topic::Topic()
{
	strongWords = std::vector<std::wstring>();
	weakWords = std::vector<std::wstring>();
}

Topic::Topic(std::vector<std::wstring> strongWords, std::vector<std::wstring> weakWords)
{
	this->strongWords = strongWords;
	this->weakWords = weakWords;
}

void Topic::readTopicFromFile(std::wstring filePath)
{
    std::wstring strongWords, weakWords;
    InputManager* in = new FileInput(filePath);
    std::wstring input = in->getInput();

    std::vector<std::wstring> words = InputManager::separateTextIntoWords(input);

    for (int i = 0; i < words.size(); i++)
    {
        if (words[i] == L"-ts" || words[i] == L"-tw")
        {
            bool isStrong = (words[i] == L"-ts");

            i++;

            while (i < words.size() && words[i] != L"-ts" && words[i] != L"-tw")
            {
                (isStrong ? strongWords : weakWords) += words[i] + L" ";
                i++;
            }

            i--;
        }
    }

    strongWords = strongWords.substr(0, strongWords.find_last_not_of(L" ") + 1);
    weakWords = weakWords.substr(0, weakWords.find_last_not_of(L" ") + 1);

	if (strongWords != L"")
	{
		ArgumentHandler::getFlag(strongTopicFlag)->raise(strongWords);

		if (weakWords != L"")
			ArgumentHandler::getFlag(weakTopicFlag)->raise(weakWords);
	}
}

Topic Topic::createTopic(std::wstring parameter)
{
	std::vector<std::wstring> strongWords = {};
	std::vector<std::wstring> weakWords = {};

	if (InputManager::detectInputType(parameter) == TextFile)
	{
		readTopicFromFile(parameter);
	}
	if (ArgumentHandler::isFlagRaised(strongTopicFlag))
		strongWords = InputManager::separateTextIntoWords(ArgumentHandler::getFlagParameter(strongTopicFlag));
	else if (ArgumentHandler::isFlagRaised(topicFlag))
		strongWords = InputManager::separateTextIntoWords(parameter);
	if (ArgumentHandler::isFlagRaised(weakTopicFlag))
		weakWords = InputManager::separateTextIntoWords(ArgumentHandler::getFlagParameter(weakTopicFlag));

	return Topic(strongWords, weakWords);
}

Topic Topic::createTopic(std::wstring strongParameter, std::wstring weakParameter)
{
	std::vector<std::wstring> strongWords = InputManager::separateTextIntoWords(strongParameter);
	std::vector<std::wstring> weakWords = InputManager::separateTextIntoWords(weakParameter);

	return Topic(strongWords, weakWords);
}

bool Topic::isStrongWord(std::wstring word)
{
	for (std::wstring& sw : strongWords)
	{
		if (word.find(sw) != std::wstring::npos)
			return true;
	}
	return false;
}

bool Topic::isWeakWord(std::wstring word)
{
	for (std::wstring& sw : strongWords)
	{
		if (word.find(sw) == 0)
			return true;
	}
	return false;
}

std::vector<std::wstring>& Topic::getStrongWords()
{
	return strongWords;
}

std::vector<std::wstring>& Topic::getWeakWords()
{
	return weakWords;
}

Topic& Topic::operator=(const Topic& other)
{
	this->strongWords = other.strongWords;
	this->weakWords = other.weakWords;
	return *this;
}
