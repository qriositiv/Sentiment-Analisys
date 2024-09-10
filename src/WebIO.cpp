#define CURL_STATICLIB
#include "CURL/curl.h"

#include <locale>
#include <codecvt>
#include <sstream>

#include "IOManager.h"
#include "Utilities.h"
#include "Arguments.h"

WebInput::WebInput(std::wstring url)
{
	this->url = url;
}

std::wstring WebInput::getInput()
{
	std::wstring html = downloadHTML(url);
	return extractRelevantText(html);
}

std::wstring WebInput::getInput(std::wstring message)
{
	return getInput();
}

std::wstring WebInput::downloadHTML(std::wstring url)
{
	CURL* curl = curl_easy_init();
	CURLcode res;
	std::wstringstream html;
	if (curl)
	{
		std::string ansiUrl = wstrToStr(url);
		curl_easy_setopt(curl, CURLOPT_URL, ansiUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);

		// checks for verbose url flag
		if (ArgumentHandler::isFlagRaised(verboseUrlFlag))
		{
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		}

		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);

		if (CURLE_OK != res)
		{
			throw (std::wstring)L"Could not download HTML from: " + url;
		}
	}
	return html.str();
}

size_t WebInput::write(void* buffer, size_t size, size_t nmemb, void* param)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstringstream& text = *static_cast<std::wstringstream*>(param);
	size_t totalSize = size * nmemb;
	std::string utf8Data(static_cast<char*>(buffer), totalSize);
	std::wstring wideData;
	try
	{
		wideData = converter.from_bytes(utf8Data);
		text << wideData;
	}
	catch (...)
	{
		wideData = strToWstr(utf8Data);
	}

	return totalSize;
}

// Scraping

std::wstring WebInput::extractRelevantText(std::wstring html)
{
	const unsigned int TAGS_SIZE = 11;
	const std::wstring tags[TAGS_SIZE] = { L"title", L"p", L"h1", L"h2", L"h3", L"h4", L"h5", L"h6", L"ul", L"ol", L"li"};
	std::vector<HtmlSegment> segments;
	std::wstring text;

	html = removeUnnecessarySegments(html);

	for (int i = 0; i < TAGS_SIZE; i++)
	{
		std::vector<HtmlSegment> temp;
		std::wstring startTag = L"<" + tags[i] + L">", endTag = L"</" + tags[i] + L">";
		temp = extractTextByTags(startTag, endTag, html);
		segments.insert(segments.end(), temp.begin(), temp.end());
		startTag = L"<" + tags[i] + L" ";
		temp = extractTextByTags(startTag, endTag, html);
		segments.insert(segments.end(), temp.begin(), temp.end());
	}

	sort(segments.begin(), segments.end(), HtmlSegment::compareByPos);

	for (int i = 0; i < segments.size(); i++)
	{
		text += segments.at(i).getText() + L"\n";
	}

	return removeTags((text.substr(0, text.size() - 1)));
}

std::vector<HtmlSegment> WebInput::extractTextByTags(std::wstring startTag, std::wstring endTag, std::wstring& html)
{
	std::vector<HtmlSegment> segments;
	size_t startpos = 0, endpos = 0;

	while (true)
	{
		startpos = html.find(startTag, endpos);
		endpos = html.find(endTag, startpos);

		if (startpos != std::wstring::npos && endpos != std::wstring::npos)
		{
			segments.push_back(HtmlSegment(startpos, html.substr(startpos, endpos - startpos + endTag.length())));

			html.erase(startpos, endpos - startpos + endTag.length());

			endpos = startpos;
		}
		else
		{
			return segments;
		}
	}
}

std::wstring WebInput::removeUnnecessarySegments(std::wstring html)
{
	const unsigned int TAGS_SIZE = 5;
	std::wstring tags[TAGS_SIZE] = { L"a", L"link", L"meta", L"style type = \"text/css\"", L"nav"};

	for (int i = 0; i < TAGS_SIZE; i++)
	{
		std::wstring startTag = L"<" + tags[i] + L">", endTag = L"</" + tags[i] + L">";
		html = cutHtml(startTag, endTag, html);
		startTag = L"<" + tags[i] + L" ";
		html = cutHtml(startTag, endTag, html);
	}

	return html;
}

std::wstring WebInput::removeTags(std::wstring text)
{
	while (text.find(L"<") != std::wstring::npos)
	{
		size_t startpos = text.find(L"<");
		size_t endpos = text.find(L">") + 1;

		if (endpos != std::wstring::npos)
		{
			text.erase(startpos, endpos - startpos);
		}
	}

	return text;
}

std::wstring WebInput::cutHtml(std::wstring startTag, std::wstring endTag, std::wstring html)
{
	size_t startpos = 0, endpos = 0;

	while (true)
	{
		startpos = html.find(startTag, endpos);
		endpos = html.find(endTag, startpos);

		if (startpos != std::wstring::npos && endpos != std::wstring::npos)
		{
			html = html.erase(html.find(startTag), html.find(endTag) - html.find(startTag) + endTag.length());
		}
		else
		{
			return html;
		}
	}
}



HtmlSegment::HtmlSegment(size_t pos, std::wstring text)
{
	this->pos = pos;
	this->text = text;
}

size_t HtmlSegment::getPos()
{
	return pos;
}

std::wstring HtmlSegment::getText()
{
	return text;
}

bool HtmlSegment::compareByPos(HtmlSegment& a, HtmlSegment& b)
{
	return a.getPos() < b.getPos();
}
