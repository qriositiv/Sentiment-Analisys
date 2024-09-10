#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>

std::string wstrToStr(std::wstring wstr);
std::wstring strToWstr(std::string str);

wchar_t** charArrayToWcharArray(int arraySize, char* charArray[]);
wchar_t* charToWchar(const char* input);

// Implemented in header due to template requirements
template<typename T>
std::wstring vectorToWstring(std::vector<T> input)
{
	std::wstringstream stream;
	int vectorSize = input.size();
	stream << "[";
	for (int i = 0; i <vectorSize; i++)
	{
		stream << input.at(i);
		if (i != vectorSize - 1)
			stream << ", ";
	}
	stream << "]";
	return stream.str();
}

template<typename T>
bool compareVectorsExact(std::vector<T> first, std::vector<T> second)
{
	if (first.size() != second.size())
		return false;

	int vectorSize = first.size();
	for (int i = 0; i < vectorSize; i++)
	{
		if (first[i] != second[i])
			return false;
	}

	return true;
}

// Sort indexes based on original values
template<typename T>
std::vector<size_t> sortIndex(std::vector<T> vec, bool asc = true)
{
	std::vector<size_t> indices(vec.size());
	iota(indices.begin(), indices.end(), 0);

	if(asc)
	{
		std::sort(indices.begin(), indices.end(),
			[&](size_t i, size_t j) { return vec[i] < vec[j]; });
	}
	else
	{
		std::sort(indices.begin(), indices.end(),
			[&](size_t i, size_t j) { return vec[i] > vec[j]; });
	}

	return indices;
}

/// <summary>
/// Formats time in YYYY-MM-DD-HH-MM-SS style
/// </summary>
/// <returns>std::wstring of time when funtion was called seperated with '-'</returns>
std::wstring formatCurrentTime();

/// <summary>
/// Writes 50 symbols of user's choice
/// </summary>
/// <param name="seperationSymbol">char of your choice</param>
/// <returns>returns std::wstring with 50 of supplied symbol</returns>
std::wstring formatSeperationLine(char seperationSymbol);
