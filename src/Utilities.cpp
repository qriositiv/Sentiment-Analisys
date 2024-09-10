#include "Utilities.h"
#include <sys/stat.h>
#include <ctime>


#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <cstring>
#include <ctime> 

std::string wstrToStr(std::wstring wstr)
{
	return std::string(wstr.begin(), wstr.end());
}

std::wstring strToWstr(std::string str)
{
	return std::wstring(str.begin(), str.end());
}

wchar_t** charArrayToWcharArray(int arraySize, char* charArray[])
{
	wchar_t** wcharArray = (wchar_t**)malloc(arraySize * sizeof(wchar_t*));
	if (wcharArray == NULL) 
{
		return NULL;
	}

	for (int i = 0; i < arraySize; i++) 
	{
		wcharArray[i] = charToWchar(charArray[i]);
		if (wcharArray[i] == NULL) 
		{
			for (int j = 0; j < i; j++) 
			{
				free(wcharArray[j]);
			}
			free(wcharArray);
			return NULL;
		}
	}

	return wcharArray;
}

wchar_t* charToWchar(const char* input) 
{
	size_t len = strlen(input);
	wchar_t* output = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
#ifdef __linux__
	if (mbstowcs(output, input, len + 1) == (size_t)-1) 
	{
		free(output);
		return NULL;  // Conversion failed
	}
#endif
	return output;
}

std::wstring formatCurrentTime()
{
	std::wstringstream stream;

	time_t rawtime; 
	struct tm timeinfo; 
	char buffer[80];

	time(&rawtime);


#ifdef _WIN32
	localtime_s(&timeinfo, &rawtime);
#else
	localtime_r(&rawtime, &timeinfo);
#endif

	strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S", &timeinfo);

	stream << buffer;

	return stream.str();
}

std::wstring formatSeperationLine(char seperationSymbol)
{
	std::wstringstream stream;
	
	stream << L"\n";

	for (int i = 0; i < 50; i++)
		stream << seperationSymbol;

	stream << L"\n\n";

	return stream.str();
}
