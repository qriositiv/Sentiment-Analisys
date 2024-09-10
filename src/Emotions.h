#pragma once

#include <vector>
#include <string>
#include <map>

enum Emotion
{
	joy,
	sadness,
	trust,
	disgust,
	fear,
	anger,
	surprise,
	anticipation
};

const static int emotionCount = 8;

class EmotionalScore;

class EmotionCombination
{
private:
	std::wstring combinationName;
	int e1, e2;

	static bool initialised;
	static const float rangePercent;
	static std::map<std::pair<int, int>, std::wstring> emotionCombinations;

	static void Initialise();

public:
	EmotionCombination(int e1, int e2, std::wstring combinationName);
	std::wstring getCombinationName();

	static EmotionCombination getCombination(int e1, int e2);
	static EmotionCombination getCombination(EmotionalScore score);
};



class EmotionalScore
{
private:
	std::vector<float> scores;
	static std::vector<std::wstring> names;

public:
	EmotionalScore();
	EmotionalScore(std::vector<float> weights);

	void invertValues();
	static std::vector<float> invertValues(std::vector<float> values);

	float getScore(int emotion);
	float getScore(Emotion emotion);
	float getSum();
	void addScore(int emotion, float value);
	void addScore(Emotion emotion, float value);
	std::vector<float> getAllScores();
	bool empty();

	std::wstring toString();
	std::wstring formatResults();
	std::wstring formatCombinations(std::wstring segmentName = L"text");
	static std::wstring formatResults(EmotionalScore results);
	static std::wstring formatResultsCSV(EmotionalScore results);
	static std::wstring formatResultsSimple(EmotionalScore results);
	static std::wstring formatCombinations(EmotionalScore results, std::wstring segmentName = L"text");

	static std::wstring getName(int emotion);
	static std::wstring getName(Emotion emotion);
	static std::pair<int, int> findTwoMaxEmotions(EmotionalScore score);

	friend bool operator==(const EmotionalScore& first, const EmotionalScore& second);
	friend bool operator!=(const EmotionalScore& first, const EmotionalScore& second);
	friend EmotionalScore operator*(const EmotionalScore& first, float second);
	friend EmotionalScore operator*(float first, const EmotionalScore& second);

	EmotionalScore operator+(const EmotionalScore& other);
	EmotionalScore operator-(const EmotionalScore& other);
	EmotionalScore& operator+=(const EmotionalScore& other);
	EmotionalScore& operator-=(const EmotionalScore& other);
	EmotionalScore& operator*=(float other);
};

bool operator==(const EmotionalScore& first, const EmotionalScore& second);
bool operator!=(const EmotionalScore& first, const EmotionalScore& second);
EmotionalScore operator*(const EmotionalScore& first, float second);
EmotionalScore operator*(float first, const EmotionalScore& second);