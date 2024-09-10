#include "Analyser.h"
#include "Arguments.h"
#include "Utilities.h"

TopicAnalyser::TopicAnalyser(Database* database, Topic topic) : VaderAnalyser(database, L"topic")
{
	this->topic = topic;
}

EmotionalScore TopicAnalyser::analyse(Text& text)
{
	writeLog(L"Strong topic words: " + vectorToWstring(topic.getStrongWords()));
	writeLog(L"Weak topic words: " + vectorToWstring(topic.getWeakWords()));
	calculateScore(text);
	return text.getScore();
}

EmotionalScore TopicAnalyser::calculateScore(Text& text)
{
	EmotionalScore score = EmotionalScore();
	int i = 0;

	for (auto& paragraph : *text.getParagraphs())
	{
		score += calculateScore(paragraph);

		if (ArgumentHandler::isFlagRaised(progressFlag))
			TerminalOutput::writeProgress(i++, text.getParagraphSize());
	}

	writeLog(L"===========================================================");
	writeLog(L"Total text score: " + score.toString());

	text.setScore(score);
	return score;
}

EmotionalScore TopicAnalyser::calculateScore(Paragraph& paragraph)
{
	EmotionalScore score = EmotionalScore();

	writeLog(L"=========================Paragraph=========================");
	writeLog(L"\"" + paragraph.printParagraph() + L"\"");

	if (!paragraph.containsTopic(topic))
	{
		writeLog(L"--------------------------------------");
		writeLog(L"No strong topic words detected in the paragraph");
		return EmotionalScore();
	}

	bool lastHadTopic = false;

	for (auto& sentence : *paragraph.getSentences())
	{
		writeLog(L"---------------Sentence---------------");
		writeLog(L"\"" + sentence.printSentence() + L"\"");

		bool containsTopic = sentence.containsTopic(topic, true);

		if (containsTopic || lastHadTopic)
		{
			lastHadTopic = containsTopic;
			score += calculateScore(sentence);
		}
		else
		{
			writeLog(L"No strong/weak topic words detected in this or previous sentence");
		}
	}

	writeLog(L"--------------------------------------");
	writeLog(L"Total paragraph score: " + score.toString());

	paragraph.setScore(score);
	return score;
}

EmotionalScore TopicAnalyser::calculateScore(Sentence& sentence)
{
	EmotionalScore score = calculateScore(*sentence.getAllLowerWords());

	writeLog(L"Total sentence score: " + score.toString());
	sentence.setScore(score);

	return score;
}

EmotionalScore TopicAnalyser::calculateScore(std::vector<std::wstring>& words)
{
	EmotionalScore totalScore;

	int n = words.size();
	for (int i = 0; i < n; i++)
	{
		Keyword foundKey = database->findKeyword(words[i]);
		if (foundKey == Keyword::empty)
			continue;

		writeLog(L"Detected: \"" + foundKey.getWord() + L"\" in \"", false);

		EmotionalScore keyScore = foundKey.getAllEmotions();
		
		keyScore *= database->findAffix(words[i], foundKey.getWord());;

		keyScore *= findMultiplier(words, i);

		writeLog(words[i] + L"\" ", false);
		writeLog(keyScore.toString());

		totalScore += keyScore;
	}

	if (words[n - 1] == L"!")
	{
		totalScore *= 1.5f;
		writeLog(L"Detected punctuation: \"!\" 1.5");
	}

	return totalScore;
}


