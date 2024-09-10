#include <iostream>
#include <locale>
#include <codecvt>

#include "Database.h"
#include "Emotions.h"
#include "IOManager.h"
#include "Utilities.h"

Database::Database(std::wstring path)
{
	path = getDatabasePath(path);

	FileOutput::writeToLog(L"Database path: " + path, 1);

	datafile.open(wstrToStr(path), std::wifstream::in);

	datafile.imbue(std::locale(datafile.getloc(), new std::codecvt_utf8<wchar_t>()));

	if (datafile.fail())
		throw (std::wstring)L"Unable to open database file: " + path;

	try {
		jsonData = nlohmann::json::parse(std::wstring(std::istreambuf_iterator<wchar_t>(datafile), {}));
	}
	catch (std::exception e) {
		std::cerr << "Error parsing JSON: " << e.what() << "\n";
	}

	keywords = readKeywords();
	affixes = readMultipliers("affixes", "affix");
	negations = readIntoMap("negations", "negation");
	amplifiers = readIntoMap("amplifiers", "word");
	connectingWords = readIntoSet("connecting_words", "words");
	generalPrefixes = readIntoSet("general_prefixes", "prefixes");

	FileOutput::writeToLog(L"Database readed successfully.", 1);
}

Database::Database(nlohmann::json jsonData)
{
	this->jsonData = jsonData;

	keywords = readKeywords();
	affixes = readMultipliers("affixes", "affix");
	negations = readIntoMap("negations", "negation");
	amplifiers = readIntoMap("amplifiers", "word");
	connectingWords = readIntoSet("connecting_words", "words");
	generalPrefixes = readIntoSet("general_prefixes", "prefixes");
}

Database::Database(std::vector<Keyword> keywords, std::vector<Multiplier> affixes, std::unordered_map<std::wstring, float> negations, std::unordered_map<std::wstring, float> amplifiers)
{
	this->keywords = keywords;
	this->affixes = affixes;
	this->negations = negations;
	this->amplifiers = amplifiers;
	this->connectingWords = std::unordered_set<std::wstring>();
	this->generalPrefixes = std::unordered_set<std::wstring>();
}

Database::Database(std::vector<Keyword> keywords, std::vector<Multiplier> affixes, std::unordered_map<std::wstring, float> negations, std::unordered_map<std::wstring, float> amplifiers, std::unordered_set<std::wstring> connectingWords)
{
	this->keywords = keywords;
	this->affixes = affixes;
	this->negations = negations;
	this->amplifiers = amplifiers;
	this->connectingWords = connectingWords;
	this->generalPrefixes = std::unordered_set<std::wstring>();
}

Database::Database(std::vector<Keyword> keywords, std::vector<Multiplier> affixes, std::unordered_map<std::wstring, float> negations, std::unordered_map<std::wstring, float> amplifiers, std::unordered_set<std::wstring> connectingWords, std::unordered_set<std::wstring> generalPrefixes)
{
	this->keywords = keywords;
	this->affixes = affixes;
	this->negations = negations;
	this->amplifiers = amplifiers;
	this->connectingWords = connectingWords;
	this->generalPrefixes = generalPrefixes;
}

std::vector<Keyword> Database::getKeywords()
{
	return keywords;
}

std::vector<Multiplier> Database::getAffixes()
{
	return affixes;
}

std::unordered_map<std::wstring, float> Database::getNegations()
{
	return negations;
}

std::unordered_map<std::wstring, float> Database::getAmplifiers()
{
	return amplifiers;
}

std::unordered_set<std::wstring> Database::getConnectingWords()
{
	return connectingWords;
}

std::unordered_set<std::wstring> Database::getGeneralPrefixes()
{
	return generalPrefixes;
}

Keyword Database::findKeyword(const std::wstring& word)
{
	for (Keyword& keyword : keywords)
	{
		size_t rootPos = word.find(keyword.getWord()); //check speed with reference return
		if (rootPos != std::wstring::npos)
		{
			if(isRealRoot(word, rootPos))
				return keyword;
		}
	}
	return Keyword::empty;
}

float Database::findAffix(const std::wstring& word, std::wstring foundKey)
{
	for (Multiplier& affix : affixes)
	{
		size_t pos = word.find(affix.getWord());
		if (foundKey.find(affix.getWord()) == 0) // so that 'im' does not mess up words like 'impress'
			continue;
		if (pos == 0 || (pos == word.length() - affix.getWord().length() && pos != std::wstring::npos))
			return affix;
	}
	return 1;
}

float Database::findNegation(const std::wstring& word)
{
	auto it = negations.find(word);
	if (it != negations.end())
		return it->second;

	return 1;
}

float Database::findAmplifier(const std::wstring& word)
{
	auto it = amplifiers.find(word);
	if (it != amplifiers.end())
		return it->second;

	return 1;
}

bool Database::isConnectingWord(const std::wstring& word)
{
	return connectingWords.find(word) != connectingWords.end();
}

bool Database::isRealRoot(const std::wstring& word, int rootPos)
{
	if (rootPos == 0)
		return true;

	return generalPrefixes.find(word.substr(0, rootPos)) != generalPrefixes.end();
}

std::wstring Database::getDatabasePath(std::wstring path)
{
	std::wifstream datafile;
	if (path.empty())
	{
		datafile.open("database.json", std::wifstream::in);
		if (!datafile.fail())
		{
			FileOutput::writeToLog(L"Default database location used", 1);
			return L"database.json";
		}
		else
		{
			TerminalInput in;
			path = in.getInput(L"Default database not detected, enter path to a database file");
			FileOutput::writeToLog(L"Default database location not used", 1);
		}
	}

	datafile.open(wstrToStr(path), std::wistream::in);
	if (datafile.fail())
		throw (std::wstring)L"Could not find/open database file: " + path;
	return path;
}

std::vector<Keyword> Database::readKeywords()
{
	std::vector<Keyword> keywords;

	nlohmann::json keywordsArray = jsonData["keywords"];

	for (const auto& keywordData : keywordsArray)
	{
		std::wstring root = strToWstr(keywordData["root"]);
		std::vector<float> scores;

		nlohmann::json scoresArray = keywordData["scores"];
		for (const auto& score : scoresArray)
			scores.push_back(score.get<float>());

		keywords.emplace_back(root, scores);
	}

	return keywords;
}

std::vector<Multiplier> Database::readMultipliers(std::string section, std::string unit)
{
	std::vector<Multiplier> multipliers;

	nlohmann::json sectionArray = jsonData[section];

	for (const auto& sectionData : sectionArray)
	{
		std::wstring _unit = strToWstr(sectionData[unit]);

		float multiplier = sectionData["multiplier"];

		multipliers.emplace_back(_unit, multiplier);
	}

	return multipliers;
}

std::unordered_map<std::wstring, float> Database::readIntoMap(std::string section, std::string unit)
{
	std::unordered_map<std::wstring, float> map;

	nlohmann::json sectionArray = jsonData[section];

	for (const auto& sectionData : sectionArray)
	{
		std::wstring _unit = strToWstr(sectionData[unit]);

		float multiplier = sectionData["multiplier"];

		map[_unit] = multiplier;
	}

	return map;
}

std::unordered_set<std::wstring> Database::readIntoSet(std::string section, std::string unit)
{
	std::unordered_set<std::wstring> set;

	nlohmann::json sectionArray = jsonData[section];

	for (const auto& sectionData : sectionArray)
	{
		nlohmann::json wordsArray = sectionData[unit];
		for (const auto& word : wordsArray)
			set.insert(strToWstr(word));
	}

	return set;
}
