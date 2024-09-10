#include "Keyword.h"
#include "Emotions.h"

Keyword::Keyword()
{
	word = L"";
	emotions = EmotionalScore();
}

Keyword::Keyword(std::wstring word, std::vector<float> emotions)
{
	this->word = word;
	this->emotions = EmotionalScore(emotions);
}

Keyword::Keyword(std::wstring word, EmotionalScore emotions)
{
	this->word;
	this->emotions = emotions;
}

std::wstring& Keyword::getWord()
{
	return word;
}

float Keyword::getEmotion(int emotion)
{
	return emotions.getScore(emotion);
}

float Keyword::getEmotion(Emotion emotion)
{
	int tempEmotion = emotion;
	return emotions.getScore(tempEmotion);
}

EmotionalScore Keyword::getAllEmotions()
{
	return emotions;
}

bool operator==(const Keyword& first, const Keyword& second)
{
	return !wcscmp(first.word.c_str(), second.word.c_str()) && first.emotions == second.emotions;
}

bool operator!=(const Keyword& first, const Keyword& second)
{
	return wcscmp(first.word.c_str(), second.word.c_str()) || first.emotions != second.emotions;
}

Keyword Keyword::empty = Keyword();


Multiplier::Multiplier()
{
	word = L"";
	multiplier = 1;
}

Multiplier::Multiplier(std::wstring word, float multiplier)
{
	this->word = word;
	this->multiplier = multiplier;
}

std::wstring Multiplier::getWord()
{
	return word;
}

float Multiplier::getMultiplier()
{
	return multiplier;
}

Multiplier::operator float() const
{
	return multiplier;
}

Multiplier& Multiplier::operator*=(float other)
{
	this->multiplier *= other;
	return *this;
}


bool operator==(const Multiplier& first, const Multiplier& second)
{
	//wcscmp returns 0 (false) when equal ???
	return !wcscmp(first.word.c_str(), second.word.c_str()) && first.multiplier == second.multiplier;
}

bool operator!=(const Multiplier& first, const Multiplier& second)
{
	return wcscmp(first.word.c_str(), second.word.c_str()) || first.multiplier != second.multiplier;
}

float operator*(const Multiplier& first, const Multiplier& second)
{
	return first.multiplier * second.multiplier;
}

float operator*(const Multiplier& first, float second)
{
	return first.multiplier * second;
}

Multiplier Multiplier::empty = Multiplier(L"", 1);
