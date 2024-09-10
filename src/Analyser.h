#pragma once
#include <vector>
#include <string>

#include "Database.h"
#include "Emotions.h"
#include "Text.h"
#include "IOManager.h"
#include "Topic.h"

class Analyser
{
protected:
	Database* database;
	FileOutput* log;

	void writeLog(std::wstring message, bool newLine = true);
public:
	Analyser(Database* database, std::wstring type);

	virtual EmotionalScore analyse(Text& text) = 0;

	virtual EmotionalScore calculateScore(Text& text) = 0;
	virtual EmotionalScore calculateScore(Paragraph& paragraph) = 0;
	virtual EmotionalScore calculateScore(Sentence& sentence) = 0;
	virtual EmotionalScore calculateScore(std::vector<std::wstring>& words) = 0;
};

class BowAnalyser : public Analyser
{
public:
	BowAnalyser(Database* database) : Analyser(database, L"bow") {};

	EmotionalScore analyse(Text& text);

	EmotionalScore calculateScore(Text& text);
	EmotionalScore calculateScore(Paragraph& paragraph);
	EmotionalScore calculateScore(Sentence& sentence);
	EmotionalScore calculateScore(std::vector<std::wstring>& words);
};

class VaderAnalyser : public Analyser
{
public:
	VaderAnalyser(Database* database) : Analyser(database, L"vader") {};
	VaderAnalyser(Database* database, std::wstring type) : Analyser(database, type) {};

	EmotionalScore analyse(Text& text);

	virtual EmotionalScore calculateScore(Text& text);
	virtual EmotionalScore calculateScore(Paragraph& paragraph);
	virtual EmotionalScore calculateScore(Sentence& sentence);
	virtual EmotionalScore calculateScore(std::vector<std::wstring>& words);

protected:
	Multiplier findMultiplier(std::vector<std::wstring>& words, int index);
};

class TopicAnalyser : public VaderAnalyser
{
private:
	Topic topic;

public:
	TopicAnalyser(Database* database, Topic topic);

	EmotionalScore analyse(Text& text);

	EmotionalScore calculateScore(Text& text);
	EmotionalScore calculateScore(Paragraph& paragraph);
	EmotionalScore calculateScore(Sentence& sentence);
	EmotionalScore calculateScore(std::vector<std::wstring>& words);
};