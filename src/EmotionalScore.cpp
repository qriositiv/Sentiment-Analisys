#include "Emotions.h"
#include "Arguments.h"
#include "Text.h"
#include "Utilities.h"

#include <vector>
#include <string>
#include <sstream>
#include <cmath>

std::vector<std::wstring> EmotionalScore::names = { L"Joy", L"Sadness", L"Trust", L"Disgust", L"Fear", L"Anger", L"Surprise", L"Anticipation" };

EmotionalScore::EmotionalScore()
{
	for (int i = 0; i < emotionCount; i++)
	{
		scores.push_back(0);
	}
};

EmotionalScore::EmotionalScore(std::vector<float> weights)
{
	for (int i = 0; i < emotionCount; i++)
	{
		scores.push_back(weights.at(i));
	}
	invertValues();
};

// Checks wether there are negative values, if so, switches it arround
void EmotionalScore::invertValues()
{
	scores = invertValues(scores);
}

std::vector<float> EmotionalScore::invertValues(std::vector<float> values)
{
	std::vector<float> newValues(emotionCount, 0);

	for (int i = 0; i < emotionCount; i++)
	{
		if (values[i] >= 0)
		{
			newValues[i] += values[i];
			continue;
		}

		if (i % 2 == 0)
		{
			newValues[i + 1] += values[i] * (-1);
		}
		else
		{
			newValues[i - 1] += values[i] * (-1);
		}
	}
	return newValues;
};

float EmotionalScore::getScore(int emotion)
{
	if (emotion < emotionCount)
	{
		return scores.at(emotion);
	}
	else
	{
		throw std::wstring(L"There is no such emotion in the list");
	}
};

float EmotionalScore::getScore(Emotion emotion)
{
	int tempEmotion = emotion;
	return getScore(tempEmotion);
}
float EmotionalScore::getSum()
{
	float sum = 0;
	for (float f : scores)
		sum += f;
	return sum;
}
;

void EmotionalScore::addScore(int emotion, float value)
{
	if (emotion < emotionCount)
	{
		scores[emotion] += value;
	}
	else
	{
		throw std::wstring(L"There is no such emotion in the list");
	}
	invertValues();
};

void EmotionalScore::addScore(Emotion emotion, float value)
{
	return addScore((int)emotion, value);
};

std::wstring	EmotionalScore::getName(int emotion)
{
	return names[emotion];
};

std::wstring	EmotionalScore::getName(Emotion emotion)
{
	return getName((int)emotion);
}

std::pair<int, int> EmotionalScore::findTwoMaxEmotions(EmotionalScore score)
{
	float max1 = score.getScore(0), max2 = score.getScore(1);
	int e1 = 0, e2 = 1;

	for (int i = 1; i < emotionCount; i++)
	{
		if (score.getScore(i) > max1)
		{
			max1 = score.getScore(i);
			e1 = i;
		}
	}

	for (int i = 1; i < emotionCount; i++)
	{
		if (score.getScore(i) > max2)
		{
			if (e1 == i)
				continue;
			max2 = score.getScore(i);
			e2 = i;
		}
	}

	return { e1, e2 };
}

EmotionalScore EmotionalScore::operator+(const EmotionalScore& other)
{
	std::vector<float> newScores = this->scores;
	for (int i = 0; i < emotionCount; i++)
	{
		newScores[i] += other.scores[i];
	}

	return EmotionalScore(newScores);
}

EmotionalScore EmotionalScore::operator-(const EmotionalScore& other)
{
	std::vector<float> newScores = this->scores;
	for (int i = 0; i < emotionCount; i++)
	{
		newScores[i] -= other.scores[i];
	}

	return EmotionalScore(newScores);
}

EmotionalScore& EmotionalScore::operator+=(const EmotionalScore& other)
{
	for (int i = 0; i < emotionCount; i++)
		scores[i] += other.scores[i];

	return *this;
}

EmotionalScore& EmotionalScore::operator-=(const EmotionalScore& other)
{
	for (int i = 0; i < emotionCount; i++)
		scores[i] -= other.scores[i];

	return *this;
}

EmotionalScore& EmotionalScore::operator*=(float other)
{
	for (int i = 0; i < emotionCount; i++)
		scores[i] *= other;

	scores = invertValues(scores);
	return *this;
}

std::vector<float> EmotionalScore::getAllScores()
{
	return scores;
}

bool EmotionalScore::empty()
{
	std::vector<float> emptyScore = { 0, 0, 0, 0, 0, 0, 0, 0 };
	return scores == emptyScore;
}

std::wstring EmotionalScore::toString()
{
	return vectorToWstring(scores);
}

std::wstring EmotionalScore::formatResults()
{
	return formatResults(*this);
}

std::wstring EmotionalScore::formatCombinations(std::wstring segmentName)
{
	return formatCombinations(*this, segmentName);
}

std::wstring EmotionalScore::formatResults(EmotionalScore results)
{
	std::wstringstream stream;
	
	if (ArgumentHandler::isFlagRaised(csvFlag))
	{
		stream << formatResultsCSV(results);
	}
	else
	{
		stream << formatResultsSimple(results);
	}

	return stream.str();
}

std::wstring EmotionalScore::formatResultsCSV(EmotionalScore results)
{
	std::wstringstream stream;

	for (int i = 0; i < emotionCount; i++)
	{
		stream << results.scores[i];
		if (i != emotionCount - 1)
			stream << L",";
	}

	return stream.str();
}

std::wstring EmotionalScore::formatResultsSimple(EmotionalScore results)
{
	float sum = 0;
	for (int i = 0; i < emotionCount; i++)
		sum += results.scores[i];

	std::wstringstream stream;

	for (int i = 0; i < emotionCount; i++)
	{
		stream << getName(i) << ": " << results.scores[i] << " " << (sum != 0 ? (round(results.scores[i] / sum * 100)) : 0) << "%";
		if (i != emotionCount - 1)
			stream << "\n";
	}

	return stream.str();
}

std::wstring EmotionalScore::formatCombinations(EmotionalScore results, std::wstring segmentName)
{
	std::wstringstream stream;
	std::pair<int, int> maxEmotions = findTwoMaxEmotions(results);
	EmotionCombination combination = EmotionCombination::getCombination(results);
	if (combination.getCombinationName() != L"")
	{
		stream << getName(maxEmotions.first) << L" and " << getName(maxEmotions.second) <<
			L" are two most prevalent emotions in this text, indicating that this " + segmentName + L"'s emotion could be " <<
			combination.getCombinationName() << L"." << "\n";
	}
	else if (results.getScore(maxEmotions.first) > 0.0)
	{
		stream << L"Dominating emotion in this " + segmentName + L" is " << getName(maxEmotions.first) << L"." << "\n";
	}
	else stream << L"No emotions were detected in this " + segmentName << "\n";
	return stream.str();
}


bool operator==(const EmotionalScore& first, const EmotionalScore& second)
{
	return first.scores == second.scores;
}

bool operator!=(const EmotionalScore& first, const EmotionalScore& second)
{
	return first.scores != second.scores;
}

EmotionalScore operator*(const EmotionalScore& first, float second)
{
	return second * first;
}

EmotionalScore operator*(float first, const EmotionalScore& second)
{
	std::vector<float> newScores = second.scores;
	for (int i = 0; i < emotionCount; i++)
		newScores[i] *= first;
	return EmotionalScore(newScores);
}
