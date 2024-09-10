#pragma once
#include <string>
#include <vector>

#include "Emotions.h"

class Keyword
{
private:
	EmotionalScore emotions;
	std::wstring word;

public:
	Keyword();
	Keyword(std::wstring word, std::vector<float> emotions);
	Keyword(std::wstring word, EmotionalScore emotions);

	std::wstring& getWord();
	float getEmotion(Emotion emotion);
	float getEmotion(int emotion);
	EmotionalScore getAllEmotions();

	// For testing
	// bool operator==(const Keyword& other);
	// Google Tests require operator to be non-member
	friend bool operator==(const Keyword& first, const Keyword& second);
	friend bool operator!=(const Keyword& first, const Keyword& second);

	static Keyword empty;
};

bool operator==(const Keyword& first, const Keyword& second);
bool operator!=(const Keyword& first, const Keyword& second);


class Multiplier
{
private:
	std::wstring word;
	float multiplier;

public:
	Multiplier();
	Multiplier(std::wstring word, float multiplier);

	std::wstring getWord();
	float getMultiplier();
	// For testing
	friend bool operator==(const Multiplier& first, const Multiplier& second);
	friend bool operator!=(const Multiplier& first, const Multiplier& second);
	friend float operator*(const Multiplier& first, const Multiplier& second);
	friend float operator*(const Multiplier& first, float second);

	// For multiplying with EmotionalScore
	operator float() const;

	Multiplier& operator*=(float other);

	static Multiplier empty;
};


bool operator==(const Multiplier& first, const Multiplier& second);
bool operator!=(const Multiplier& first, const Multiplier& second);
float operator*(const Multiplier& first, const Multiplier& second);
float operator*(const Multiplier& first, float second);