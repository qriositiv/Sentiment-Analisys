#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Keyword.h"
#include "nlohmann/json.hpp"

class Database
{
private:
	std::wifstream datafile;
	nlohmann::json jsonData;
	std::vector<Keyword> keywords;
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	std::unordered_set<std::wstring> connectingWords;
	std::unordered_set<std::wstring> generalPrefixes;

public:
	Database(std::wstring path);
	// For testing
	Database(nlohmann::json jsonData);
	Database(std::vector<Keyword> keywords, std::vector<Multiplier> affixes, std::unordered_map<std::wstring, float> negations, std::unordered_map<std::wstring, float> amplifiers);
	Database(std::vector<Keyword> keywords, std::vector<Multiplier> affixes, std::unordered_map<std::wstring, float> negations, std::unordered_map<std::wstring, float> amplifiers, std::unordered_set<std::wstring> connectingWords);
	Database(std::vector<Keyword> keywords, std::vector<Multiplier> affixes, std::unordered_map<std::wstring, float> negations, std::unordered_map<std::wstring, float> amplifiers, std::unordered_set<std::wstring> connectingWords, std::unordered_set<std::wstring> generalPrefixes);


	std::vector<Keyword> getKeywords();
	std::vector<Multiplier> getAffixes();
	std::unordered_map<std::wstring, float> getNegations();
	std::unordered_map<std::wstring, float> getAmplifiers();
	std::unordered_set<std::wstring> getConnectingWords();
	std::unordered_set<std::wstring> getGeneralPrefixes();

	Keyword findKeyword(const std::wstring& word);
	float findAffix(const std::wstring& word, std::wstring foundKey);
	float findNegation(const std::wstring& word);
	float findAmplifier(const std::wstring& word);
	bool isConnectingWord(const std::wstring& word);
	bool isRealRoot(const std::wstring& word, int rootPos);

	static std::wstring getDatabasePath(std::wstring path);

private:
	std::vector<Keyword> readKeywords();
	std::vector<Multiplier> readMultipliers(std::string section, std::string unit);
	std::unordered_map<std::wstring, float> readIntoMap(std::string section, std::string unit);
	std::unordered_set<std::wstring> readIntoSet(std::string section, std::string unit);
};