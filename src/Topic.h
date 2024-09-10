#pragma once
#include <vector>
#include <string>

class Topic
{
private:
	std::vector<std::wstring> strongWords;
	std::vector<std::wstring> weakWords;

public:
	Topic();
	Topic(std::vector<std::wstring> strongWords, std::vector<std::wstring> weakWords);

	static void readTopicFromFile(std::wstring filePath);
	static Topic createTopic(std::wstring parameter);
	static Topic createTopic(std::wstring strongParameter, std::wstring weakParameter);

	bool isStrongWord(std::wstring word);
	bool isWeakWord(std::wstring word);

	std::vector<std::wstring>& getStrongWords();
	std::vector<std::wstring>& getWeakWords();

	Topic& operator=(const Topic& other);
};

