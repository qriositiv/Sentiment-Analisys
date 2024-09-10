#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Emotions.h"
#include "Topic.h"

class Sentence
{
private:
	std::shared_ptr<std::vector<std::wstring>> words;
	std::shared_ptr<std::vector<std::wstring>> lcWords; //lower case
	EmotionalScore score;

public:
	Sentence(std::shared_ptr<std::vector<std::wstring>> words);
	Sentence(std::initializer_list<std::wstring> wordList); // mostly for tests
	EmotionalScore getScore();
	void setScore(EmotionalScore score);

	std::shared_ptr<std::vector<std::wstring>> getAllWords();
	std::shared_ptr<std::vector<std::wstring>> getAllLowerWords();
	std::wstring printSentence();

	/// <summary>
	/// Goes through every word and checks if at least one of the topic words is present.
	/// </summary>
	/// <param name="topic">the topic to check for .</param>
	/// <param name="checkWeakWords">if true, also checks for weak words.</param>
	/// <returns>true if at least one of topic words is present in the sentence.</returns>
	bool containsTopic(Topic& topic, bool checkWeakWords);

	/// <summary>
	/// Goes through every word and checks if at least one of the topic words is present.
	/// </summary>
	/// <param name="topicWords">words, for which to search for.</param>
	/// <param name="weakWords">are the topic words weak. If true, matches the full word instead of root.</param>
	/// <returns>true if at least one of topic words is present in the sentence.</returns>
	bool containsTopic(std::vector<std::wstring>& topicWords, bool weakWords);
};

class Paragraph
{
private:
	std::shared_ptr<std::vector<Sentence>> sentences;
	EmotionalScore score;

public:
	Paragraph(std::shared_ptr<std::vector<Sentence>> sentences);
	Paragraph(std::initializer_list<Sentence> sentenceList);
	EmotionalScore getScore();
	void setScore(EmotionalScore score);
	std::shared_ptr<std::vector<Sentence>> getSentences();
	std::wstring printParagraph();

	/// <summary>
	/// Goes through every word and checks if at least one of the topic words is present.
	/// </summary>
	/// <param name="topic">the topic to check for .</param>
	/// <returns>true if at least one of topic words is present in the sentence.</returns>
	bool containsTopic(Topic& topic);
};

class Text
{
private:
	std::shared_ptr<std::vector<Paragraph>> paragraphs;
	EmotionalScore score;
	static const int DEFAULT_NUMBER_OF_SEGMENTS;

public:
	Text(std::shared_ptr<std::vector<Paragraph>> paragraphs);
	Text(std::initializer_list<Paragraph> paragraphList);
	EmotionalScore getScore();
	void setScore(EmotionalScore score);
	int getParagraphSize();
	std::shared_ptr<std::vector<Paragraph>> getParagraphs();

	/// <summary>
	/// Formats final results based on a raised flag (-vs, -vp, -vvp, -csv...)
	/// </summary>
	/// <returns>std::wstring with text (depends on flags) and score attached</returns>
	std::wstring formatFinalResults();

	/// <summary>
	/// Calculate how much Paragraphs should be inside one segment.
	/// </summary>
	/// <returns>float number of segment size.</returns>
	float setSegmentSize();

	/// <summary>
	/// Stores Paragraphs, which are separated into segments, indexes.
	/// </summary>
	/// <returns>Vector of std::vectors of integer values.</returns>
   std::vector<std::vector<int>> distributeSegments();

	/// <summary>
	/// Calculates emotional flow of every Text segment.
	/// </summary>
	/// <returns>Vector of Emotional Score of each Text segment.</returns>
   std::vector<EmotionalScore> calculateFlow();

	/// <summary>
	/// Calculates emotional flow of every Text segment.
	/// </summary>
	/// <param name="segments">Text segments represented Paragraphs indexes.</param>
	/// <returns>Vector of Emotional Score.</returns>
	std::vector<EmotionalScore> calculateFlow(std::vector<std::vector<int>>& segments);

	/// <summary>
	/// Formating segments emotional flow into user-friendly view.
	/// </summary>
	/// <param name="flow">Emotional Scores of every segment.</param>
	/// <returns>Wstring with formated emotions flow results inside.</returns>
	static std::wstring formatFlow(std::vector<EmotionalScore> flow);
};
