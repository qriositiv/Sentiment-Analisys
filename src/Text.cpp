#include "Text.h"
#include "IOManager.h"
#include "Emotions.h"
#include "Arguments.h"

#include <sstream>
#include <cmath> 
#include <algorithm>

Sentence::Sentence(std::shared_ptr<std::vector<std::wstring>> words)
{
	this->words = words;
	this->lcWords = std::make_shared<std::vector<std::wstring>>(*words);
	for (auto& word : *lcWords)
	{
		transform(word.begin(), word.end(), word.begin(), ::tolower);
	}
}

Sentence::Sentence(std::initializer_list<std::wstring> wordList)
{
	this->words = std::make_shared<std::vector<std::wstring>>(wordList);
	this->lcWords = std::make_shared<std::vector<std::wstring>>(wordList);
	for (auto& word : *lcWords)
	{
		transform(word.begin(), word.end(), word.begin(), ::tolower);
	}
}

void Sentence::setScore(EmotionalScore score)
{
	this->score = score;
}

EmotionalScore Sentence::getScore()
{
	return score;
}

std::shared_ptr<std::vector<std::wstring>> Sentence::getAllWords()
{
	return words;
}

std::shared_ptr<std::vector<std::wstring>> Sentence::getAllLowerWords()
{
	return lcWords;
}

std::wstring Sentence::printSentence()
{
	std::wstringstream stream;

	int sentenceLength = words->size();

	for (int i = 0; i < sentenceLength - 2; i++)
	{
		stream << (*words)[i] << L" ";
	}

	if (sentenceLength >= 2)
	{
		stream << (*words)[sentenceLength - 2];
		if ((*words)[sentenceLength - 1].length() != 1) //check if punctuation
			stream << L" ";
		stream << (*words)[sentenceLength - 1];
	}
	else if (sentenceLength == 1) stream << (*words)[sentenceLength - 1];

	return stream.str();
}

bool Sentence::containsTopic(Topic& topic, bool checkWeakWords)
{
	if (containsTopic(topic.getStrongWords(), false))
		return true;
	if (checkWeakWords && containsTopic(topic.getWeakWords(), true))
		return true;
	return false;
}

bool Sentence::containsTopic(std::vector<std::wstring>& topicWords, bool weakWords)
{
	for (auto& word : *lcWords)
	{
		for (auto& topicWord : topicWords)
		{
			if (weakWords && word == topicWord)
				return true;
			if (!weakWords && word.find(topicWord) != std::wstring::npos)
				return true;
		}
	}
	return false;
}



Paragraph::Paragraph(std::shared_ptr<std::vector<Sentence>> sentences)
{
	this->sentences = sentences;
}

Paragraph::Paragraph(std::initializer_list<Sentence> sentenceList)
{
	this->sentences = std::make_shared<std::vector<Sentence>>(sentenceList);
}

EmotionalScore Paragraph::getScore()
{
	return score;
}

void Paragraph::setScore(EmotionalScore score)
{
	this->score = score;
}

std::shared_ptr<std::vector<Sentence>> Paragraph::getSentences()
{
	return sentences;
}

std::wstring Paragraph::printParagraph()
{
	std::wstringstream stream;

	size_t paragraphLength = sentences->size();

	int n = sentences->size();
	for (int i = 0; i<n; i++)
	{
		stream << (*sentences)[i].printSentence();
		if (i != n - 1)
			stream << L" ";
	}

	return stream.str();
}

bool Paragraph::containsTopic(Topic& topic)
{
	for (auto& sentence : *sentences)
	{
		if (sentence.containsTopic(topic, false))
			return true;
	}
	return false;
}



Text::Text(std::shared_ptr<std::vector<Paragraph>> paragraphs)
{
	this->paragraphs = paragraphs;
}

Text::Text(std::initializer_list<Paragraph> paragraphList)
{
	this->paragraphs = std::make_shared<std::vector<Paragraph>>(paragraphList);
}

EmotionalScore Text::getScore()
{
	return score;
}

void Text::setScore(EmotionalScore score)
{
	this->score = score;
}

int Text::getParagraphSize()
{
	return paragraphs->size();
}

std::shared_ptr<std::vector<Paragraph>> Text::getParagraphs()
{
	return paragraphs;
}

std::wstring Text::formatFinalResults()
{
	std::wstringstream stream;

	if (ArgumentHandler::isFlagRaised(resultsParagraphFlag))
	{		
		if (ArgumentHandler::isFlagRaised(csvFlag))
		{
			stream << L"paragraphNr";
			for (int i = 0; i < emotionCount; i++)
			{
				stream << EmotionalScore::getName(i);
				if (i != emotionCount - 1)
					stream << L",";
			}
			stream << L"\n";
		}

		size_t n = paragraphs->size();
		for (int i = 0; i < n; i++)
		{
			if (ArgumentHandler::isFlagRaised(csvFlag))
			{
				stream << i + 1 << ",";
				stream << (*paragraphs)[i].getScore().formatResults() << L"\n";
			}
			else
			{
				stream << L"Paragraph " << i + 1 << L"\n";
				stream << (*paragraphs)[i].getScore().formatResults() << L"\n\n";
				stream << (*paragraphs)[i].getScore().formatCombinations(L"paragraph") << L"\n";
			}		
		}		
	}
	else if (ArgumentHandler::isFlagRaised(resultsParagraphAndTextFlag))
	{
		size_t n = paragraphs->size();
		for (int i = 0; i < n; i++)
		{
			stream << (*paragraphs)[i].printParagraph() << L"\n";
			stream << (*paragraphs)[i].getScore().formatResults() << L"\n\n";
			stream << (*paragraphs)[i].getScore().formatCombinations(L"paragraph") << L"\n";
		}
	}
	else if (ArgumentHandler::isFlagRaised(resultsSentenceAndTextFlag))
	{
		if (ArgumentHandler::isFlagRaised(csvFlag))
		{
			stream << L"paragraphNr,sentenceNr,";
			for (int i = 0; i < emotionCount; i++)
			{
				stream << EmotionalScore::getName(i);
				if (i != emotionCount - 1)
					stream << L",";
			}
			stream << L"\n";
		}

		size_t n = paragraphs->size();
		for (int i = 0; i < n; i++)
		{
			size_t m = (*paragraphs)[i].getSentences()->size();
			for (int j = 0; j < m; j++)
			{
				if (ArgumentHandler::isFlagRaised(csvFlag))
				{
					stream << i + 1<< "," << j + 1 << ",";
					stream << (*(*paragraphs)[i].getSentences())[j].getScore().formatResults() << L"\n";
				}
				else
				{
					stream << (*(*paragraphs)[i].getSentences())[j].printSentence() << L"\n";
					stream << (*(*paragraphs)[i].getSentences())[j].getScore().formatResults() << L"\n\n";
					stream << (*(*paragraphs)[i].getSentences())[j].getScore().formatCombinations(L"sentence") << L"\n";
				}	
			}
		}
	}
	else
	{
		if (ArgumentHandler::isFlagRaised(csvFlag))
		{
			for (int i = 0; i < emotionCount; i++)
			{
				stream << EmotionalScore::getName(i);
				if (i != emotionCount - 1)
					stream << L",";
			}
			stream << L"\n";
			stream << score.formatResults() << L"\n";
		}
		else
		{
			stream << score.formatResults() << L"\n\n";
			stream << score.formatCombinations(L"text");
		}		
	}

	return stream.str();
}


const int Text::DEFAULT_NUMBER_OF_SEGMENTS = 10;

float Text::setSegmentSize()
{
	if ((getParagraphs()->size()) <= DEFAULT_NUMBER_OF_SEGMENTS)
		return 1.0;

	return (float)(getParagraphs()->size()) / (float)DEFAULT_NUMBER_OF_SEGMENTS;
}

std::vector<std::vector<int>> Text::distributeSegments()
{
	std::vector<std::vector<int>> segments;
	std::vector<int> indexes;
	float segmentSize = setSegmentSize();
	float correlation = 0.0;

	size_t n = paragraphs->size();
	for (int i = 0; i < n; i++)
	{
		indexes.push_back(i);

		if (indexes.size() + correlation >= segmentSize - 0.001)
		{
			correlation = indexes.size() - segmentSize + correlation;
			if (correlation >= 1.0)
				correlation--;

			segments.push_back(indexes);
			indexes.clear();
		}
	}

	if (!indexes.empty())
	{
		segments.push_back(indexes);
	}

	return segments;
}

std::vector<EmotionalScore> Text::calculateFlow()
{
	std::vector<std::vector<int>> segments = Text::distributeSegments();

	return calculateFlow(segments);
}

std::vector<EmotionalScore> Text::calculateFlow(std::vector<std::vector<int>>& segments)
{
	std::vector<EmotionalScore> flow;

	int lowerBound = floor(setSegmentSize());
	int upperBound = ceil(setSegmentSize());
	float multiplier = (float)(upperBound) / (float)(lowerBound);

	for (std::vector<int>& segment : segments)
	{
		EmotionalScore score;

		for (int paragraphIndex : segment)
		{
			;
			score = score + (*paragraphs)[paragraphIndex].getScore();
		}

		if (segment.size() < setSegmentSize())
			score *= multiplier;

		flow.push_back(score);
	}

	return flow;
}

std::wstring Text::formatFlow(std::vector<EmotionalScore> flow)
{
	std::wstringstream stream;

	stream << L"Flow results of " << flow.size() << " segments:" << "\n";

	stream << "1. [";
	for (int j = 0; j < emotionCount; j++)
	{
		stream << flow.at(0).getScore(j);

		if (j != emotionCount - 1)
			stream << ",\t";
	}
	stream << "]" << "\n";

	for (int i = 1; i < flow.size(); i++)
	{
		EmotionalScore previousFlow = flow.at(i - 1);
		EmotionalScore currentFlow = flow.at(i);

		stream << i + 1 << ". ";

		stream << "[";
		for (int j = 0; j < emotionCount; j++)
		{
			float difference = round((currentFlow.getScore(j) - previousFlow.getScore(j)) * 10.0) / 10.0;
			if (difference > 0)
				stream << currentFlow.getScore(j) << L"(+" << difference << L")";
			else 
				stream << currentFlow.getScore(j) << L"(" << difference << L")";

			if (j != emotionCount - 1)
				stream << ",\t";
		}
		stream << "]" << "\n";

	}

	return stream.str();
}
