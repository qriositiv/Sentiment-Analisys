#pragma once
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

#include "Text.h"
#include "haru/hpdf.h"

enum InputType
{
	PlainText,
	TextFile,
	Url,
};

class InputManager
{
public:
	virtual std::wstring getInput() = 0;
	virtual std::wstring getInput(std::wstring message) = 0;

	static std::wstring getInput(InputManager* in, std::wstring input = L"");
	static int detectInputType(std::wstring input);
	static std::vector<std::wstring> separateTextIntoWords(std::wstring input);

	/// <summary>
	/// Seperates text into paragraphs and sentences
	/// </summary>
	/// <param name="text">that you want to get tokenized</param>
	/// <returns>Text object that stores seperated text</returns>
	static Text separateText(const std::wstring& text);

private:
	static bool isParagraphEnd(wchar_t c);
	static bool isSentenceEnd(wchar_t c);
	static bool isWordEnd(wchar_t c);
};

class OutputManager
{
public:
	virtual void writeMessage(std::wstring message) = 0;
	virtual void writeMessage(Text& text) = 0;
	virtual void writeMessage(std::vector<EmotionalScore> flow) = 0;
};



class TerminalInput : public InputManager
{
public:
	std::wstring getInput();
	std::wstring getInput(std::wstring message);
};

class TerminalOutput : public OutputManager
{
public:
	void writeMessage(std::wstring message);
	void writeMessage(Text& text);
	void writeMessage(std::vector<EmotionalScore> flow);
	static void writeProgress(int current, int total);
};



class FileInput : public InputManager
{
private:
	std::wifstream file;
public:
	FileInput(std::wstring filePath);
	~FileInput();
	std::wstring getInput();
	std::wstring getInput(std::wstring message);

	void open(std::wstring filepath);
	void close(); // For testing mostly.
};

class FileOutput : public OutputManager
{
private:
	std::wofstream file;
	static FileOutput* log;
	static bool logStarted;

public:
	FileOutput(std::wstring filepath);
	~FileOutput();
	static void startLog();
	/// <summary>
	/// writes to general log file with timestamp. 
	/// </summary>
	/// <param name="message">you want to be printed</param>
	/// <param name="endLine">whether you want endline after your message to be inserted</param>
	static void writeToLog(std::wstring message, bool endLine);

	/// <summary>
	/// writes to general log with no formatting.
	/// </summary>
	/// <param name="message">you want to printed</param>
	static void writeToLogNoFormatting(std::wstring message);
	void writeMessage(std::wstring message);
	void writeMessage(Text& text);
	void writeMessage(std::vector<EmotionalScore> flow);
	void writeMessageRef(std::wstring& message);
	void open(std::wstring filepath);
	void close();
};



class HtmlSegment;

class WebInput : public InputManager
{
private:
	std::wstring url;
public:
	WebInput(std::wstring url);
	std::wstring getInput();
	std::wstring getInput(std::wstring messsage);
	static std::wstring downloadHTML(std::wstring url);
	static std::wstring extractRelevantText(std::wstring html);

	static std::vector<HtmlSegment> extractTextByTags(std::wstring startTag, std::wstring endTag, std::wstring& html);
	static std::wstring removeUnnecessarySegments(std::wstring html);
	static std::wstring removeTags(std::wstring text);

	/// <summary>
	/// Removes tags and text between supplied tags
	/// </summary>
	/// <param name="startTag">tag from where you want to cut</param>
	/// <param name="endTag">tag to what you want to cut to</param>
	/// <param name="html">std::wstring you want to get tags cut from</param>
	/// <returns>std::wstring with tags and text between tags cut</returns>
	static std::wstring cutHtml(std::wstring startTag, std::wstring endTag, std::wstring html);
private:
	static size_t write(void* buffer, size_t size, size_t nmemb, void* param);

};

class HtmlSegment
{
private:
	size_t pos;
	std::wstring text;
public:
	HtmlSegment(size_t pos, std::wstring text);
	size_t getPos();
	std::wstring getText();

	static bool compareByPos(HtmlSegment& a, HtmlSegment& b);
};



class PdfOutput : public OutputManager
{
	HPDF_Doc pdf;
	HPDF_Page page;
	std::string path;
	float width, height, linesLeft;
	int pageNr;
	bool textStarted = false;

public:
	PdfOutput(std::wstring path);
	void writeMessage(std::wstring message);
	void writeMessage(Text& text);
	void writeMessage(EmotionalScore score);
	void writeMessage(std::vector<EmotionalScore> flow);
	void saveFile();

private:
	void drawPieChart(EmotionalScore score, float centerX, float centerY);
	void drawCircle(const float color[3], float centerX, float centerY, float radius);
	void drawSquare(int color, float x, float y, float a);
	void drawLine(float x1, float y1, float x2, float y2, const float color[3], float width = 1);
	void drawChartOutline(float x, float y, float width, float max, int segmentCount);
	void writeHeadline(std::wstring text);

	std::vector<std::wstring> textIntoLines(std::wstring text, int linewidth);

	void createFile();
	void startNewPage();
	void startText(int x, int y, float fontSize = 12);
	void endText();
};