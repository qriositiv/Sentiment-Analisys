#include "Analyser.h"
#include "IOManager.h"
#include "Arguments.h"

EmotionalScore VaderAnalyser::analyse(Text& text)
{
	calculateScore(text);

	return text.getScore();
}

EmotionalScore VaderAnalyser::calculateScore(Text& text)
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

EmotionalScore VaderAnalyser::calculateScore(Paragraph& paragraph)
{
	EmotionalScore score = EmotionalScore();

	writeLog(L"=========================Paragraph=========================");
	writeLog(L"\"" + paragraph.printParagraph() + L"\"");

	for (Sentence& sentence : *paragraph.getSentences())
	{
		score += calculateScore(sentence);
	}

	writeLog(L"--------------------------------------");
	writeLog(L"Total paragraph score: " + score.toString());

	paragraph.setScore(score);
	return score;
}

EmotionalScore VaderAnalyser::calculateScore(Sentence& sentence)
{
	writeLog(L"---------------Sentence---------------");
	writeLog(L"\"" + sentence.printSentence() + L"\"");

	EmotionalScore score = calculateScore(*sentence.getAllLowerWords());
	writeLog(L"Total sentence score: " + score.toString());
	sentence.setScore(score);

	return score;
}

EmotionalScore VaderAnalyser::calculateScore(std::vector<std::wstring>& words)
{
	EmotionalScore totalScore;

	int n = words.size();
	for (int i = 0; i < n; i++)
	{
		if (database->isConnectingWord(words[i]))
			continue;

		Keyword foundKey = database->findKeyword(words[i]);
		if (foundKey == Keyword::empty)
			continue;

		writeLog(L"Detected: \"" + foundKey.getWord() + L"\" in \"", false);

		EmotionalScore keyScore = foundKey.getAllEmotions();

		keyScore *= database->findAffix(words[i], foundKey.getWord());

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

Multiplier VaderAnalyser::findMultiplier(std::vector<std::wstring>& words, int index)
{
	std::vector<std::wstring> found;
	Multiplier mult(L"", 1);
	int n = 2;
	for (int j = 1; j <= n; j++)
	{
		if (index - j < 0)
			break;

		if (database->isConnectingWord(words[index - j]))
		{
			found.insert(found.begin(), words[index - j]);
			n++;
			continue;
		}

		float amplifier = database->findAmplifier(words[index - j]);
		if (amplifier != 1)
		{
			found.insert(found.begin(), words[index - j]);
			mult *= amplifier;
		}
		else
		{
			float negation = database->findNegation(words[index - j]);
			if (negation != 1)
			{
				found.insert(found.begin(), words[index - j]);
				mult *= negation;
			}
		}
	}

	for (std::wstring f : found)
		writeLog(f + L" ", false);

	return mult;
}