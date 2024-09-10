#include "Analyser.h"
#include "IOManager.h"
#include "Arguments.h"

EmotionalScore BowAnalyser::analyse(Text& text)
{
	calculateScore(text);

	return text.getScore();
}

EmotionalScore BowAnalyser::calculateScore(Text& text)
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

EmotionalScore BowAnalyser::calculateScore(Paragraph& paragraph)
{
	EmotionalScore score = EmotionalScore();

	writeLog(L"=========================Paragraph=========================");
	writeLog(L"\"" + paragraph.printParagraph() + L"\"");

	for (auto& sentence : *paragraph.getSentences())
	{
		score += calculateScore(sentence);
	}

	writeLog(L"--------------------------------------");
	writeLog(L"Total paragraph score: " + score.toString());

	paragraph.setScore(score);
	return score;
}

EmotionalScore BowAnalyser::calculateScore(Sentence& sentence)
{
	writeLog(L"---------------Sentence---------------");
	writeLog(L"\"" + sentence.printSentence() + L"\"");

	EmotionalScore score = calculateScore(*sentence.getAllLowerWords());
	writeLog(L"Total sentence score: " + score.toString());
	sentence.setScore(score);

	return score;
}

EmotionalScore BowAnalyser::calculateScore(std::vector<std::wstring>& words)
{
	EmotionalScore score;

	for (std::wstring& word : words)
	{
		Keyword foundKey = database->findKeyword(word);
		if (foundKey == Keyword::empty)
			continue;

		writeLog(L"Detected: \"" + foundKey.getWord() + L"\" in \"" + word + L"\" " + foundKey.getAllEmotions().toString());

		score += foundKey.getAllEmotions();
	}

	return score;
}