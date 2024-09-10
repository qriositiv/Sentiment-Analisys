#include "IOManager.h"
#include "Utilities.h"
#include "Arguments.h"
#include "PdfSettings.h"

#include <cmath>
#include <algorithm>
#include <iostream>

void errorHandler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data)
{
	std::wcerr << L"LibHaru error: " << errno << ", detail_no: " << detail_no << "\n";
}

PdfOutput::PdfOutput(std::wstring path)
{
	this->path = wstrToStr(path);
	createFile();
	pageNr = 1;
	startNewPage();
	writeHeadline(L"Sentiment Analysis Results");
	writeMessage(L"\n");
}

void PdfOutput::writeMessage(std::wstring message)
{
	if (message == L"\n")
	{
		if (linesLeft <= 0)
			startNewPage();
		HPDF_Page_MoveTextPos(page, 0, -1 * lineSpacing);
		linesLeft--;
		return;
	}

	std::vector<std::wstring> lines = textIntoLines(message, width - sideMargin);
	int n = lines.size();

	for (int i = 0; i < n; i++)
	{
		if (linesLeft <= 0)
			startNewPage();

		HPDF_Page_MoveTextPos(page, 0, -1 * lineSpacing);
		HPDF_Page_ShowText(page, wstrToStr(lines[i]).c_str());

		linesLeft--;
	}
}

void PdfOutput::writeMessage(Text& text)
{
	std::wstringstream stream;
	if (ArgumentHandler::isFlagRaised(resultsParagraphFlag))
	{
		std::shared_ptr<std::vector<Paragraph>> paragraphs = text.getParagraphs();

		size_t n = paragraphs->size();
		for (int i = 0; i < n; i++)
		{
			writeMessage(L"Paragraph " + std::to_wstring(i + 1) + L"\n");
			writeMessage((*paragraphs)[i].getScore());
			linesLeft++;
			writeMessage((*paragraphs)[i].getScore().formatCombinations(L"paragraph") + L"\n");
			linesLeft--;
		}

	}
	else if (ArgumentHandler::isFlagRaised(resultsParagraphAndTextFlag))
	{
		for (auto& paragraph : *text.getParagraphs())
		{
			writeMessage(paragraph.printParagraph() + L"\n\n");
			writeMessage(paragraph.getScore());
			linesLeft++;
			writeMessage(paragraph.getScore().formatCombinations(L"paragraph") + L"\n");
			linesLeft--;
		}
	}
	else if (ArgumentHandler::isFlagRaised(resultsSentenceAndTextFlag))
	{
		for (auto& paragraph : *text.getParagraphs())
		{
			for (auto& sentence : *paragraph.getSentences())
			{
				writeMessage(sentence.printSentence() + L"\n\n");
				writeMessage(sentence.getScore());
				linesLeft++;
				writeMessage(sentence.getScore().formatCombinations(L"sentence") + L"\n");
				linesLeft--;
			}
		}
	}
	else
	{
		writeMessage(text.getScore());
		linesLeft++;
		writeMessage(text.getScore().formatCombinations(L"text") + L"\n");
		linesLeft--;
	}
}

void PdfOutput::writeMessage(EmotionalScore score)
{
	if (linesLeft < emotionCount)
		startNewPage();

	HPDF_Point pos = HPDF_Page_GetCurrentTextPos(page);

	for (int i = 0; i < emotionCount; i++) // draw color squares next to emotions
		drawSquare(i, leftMargin + resultOffset - 15, pos.y - (i+1) * lineSpacing, fontSize*0.75f);

	HPDF_Page_MoveTextPos(page, resultOffset, 0); // move text start to right so that graph is before text
	writeMessage(score.formatResults());
	HPDF_Page_MoveTextPos(page, -1*resultOffset, 0);
	pos = HPDF_Page_GetCurrentTextPos(page);

	float sum = 0;
	for (int i = 0; i < emotionCount; i++)
		sum += score.getScore(i);

	if (sum == 0)
		drawCircle(greyColor, leftMargin + pieRadius, pos.y + pieRadius, pieRadius); // draw grey circle
	else
		drawPieChart(score, leftMargin + pieRadius, pos.y + pieRadius); // draw pie chart for emotions

	startText(leftMargin, pos.y);
}

void PdfOutput::writeMessage(std::vector<EmotionalScore> flow)
{
	float max = 0;
	for (EmotionalScore seg : flow)
	{
		std::vector<float> score = seg.getAllScores();
		float tempMax = *max_element(score.begin(), score.end());
		if (tempMax > max)
			max = tempMax;
	}

	if (max == 0 || flow.size() < 2)
		return;

	startNewPage();

	writeHeadline(L"Emotional Flow");
	writeMessage(L"\n");
	writeMessage(L"This graph showcases how the emotions changed throughout the text. The text is separated into " + strToWstr(std::to_string(flow.size())) + L" equal sized segments, with each segment having its own score.");
	writeMessage(L"\n");
	HPDF_Point pos = HPDF_Page_GetCurrentTextPos(page);
	float chartWidth = width - sideMargin - valueOffset;
	pos.x += valueOffset;

	drawChartOutline(pos.x, pos.y, chartWidth, max, flow.size());

	float segmentWidth = chartWidth / flow.size();
	pos.x += segmentWidth / 2;
	pos.y -= chartHeight;

	for (int i = 0; i < emotionCount; i++)
	{
		float x = pos.x;
		float y = 0;
		float lastY = 0;
		float lastX = 0;
		bool first = true;
		for (EmotionalScore& segment : flow)
		{
			y = pos.y + (segment.getScore(i) / max) * chartHeight;
			drawCircle(chartColors[i], x, y, fontSize / 4);
			if (!first)
				drawLine(lastX, lastY, x, y, chartColors[i], fontSize/8);
			lastX = x;
			lastY = y;
			x += segmentWidth;
			first = false;
		}
	}
}

void PdfOutput::saveFile()
{
	HPDF_SaveToFile(pdf, path.c_str());
	HPDF_Free(pdf);
}


void PdfOutput::drawPieChart(EmotionalScore score, float x, float y)
{
	bool wasText = textStarted;
	HPDF_Point ogPos;
	if (wasText)
		ogPos = HPDF_Page_GetCurrentTextPos(page);
	endText();

	std::vector<float> scores = score.getAllScores();
	std::vector<size_t> indexes = sortIndex(scores, false);

	float totalScore = 0;
	for (int i = 0; i < emotionCount; i++)
		totalScore += scores[i];

	float startAngle = 0.0;
	float endAngle;

	HPDF_Point pos;
	pos.x = x;
	pos.y = y;
	for (size_t i : indexes) // go through in sorted order (done like this for color choosing)
	{
		if (scores[i] == totalScore)
		{
			drawCircle(chartColors[i], x, y, pieRadius); // draw circle of single color
			break;
		}
		endAngle = startAngle + scores[i]/totalScore * 360.0;
		HPDF_Page_SetRGBFill(page, chartColors[i][0]/255, chartColors[i][1]/255, chartColors[i][2]/255); // set color
		HPDF_Page_MoveTo(page, x, y); // move to center
		HPDF_Page_LineTo(page, pos.x, pos.y); // draw line from center to edge
		HPDF_Page_Arc(page, x, y, pieRadius, startAngle, endAngle); // draw outside arc
		pos = HPDF_Page_GetCurrentPos(page); // save current outside position for next arc drawing
		HPDF_Page_LineTo(page, x, y); // draw line back to center
		HPDF_Page_Fill(page); // fill up the part

		startAngle = endAngle;
	}
	HPDF_Page_SetRGBFill(page, 0, 0, 0); // reset color to black

	if (wasText)
		startText(leftMargin, ogPos.y);
}

void PdfOutput::drawCircle(const float color[3], float x, float y, float radius)
{
	bool wasText = textStarted;
	HPDF_Point ogPos;
	if(wasText)
		ogPos = HPDF_Page_GetCurrentTextPos(page);
	endText();

	HPDF_Page_SetRGBFill(page, color[0] / 255, color[1]/ 255, color[2] / 255); // set color
	HPDF_Page_Circle(page, x, y, radius);
	HPDF_Page_Fill(page); // fill up the part


	HPDF_Page_SetRGBFill(page, 0, 0, 0); // reset color to black

	if (wasText)
		startText(leftMargin, ogPos.y);
}

void PdfOutput::drawSquare(int color, float x, float y, float a)
{
	bool wasText = textStarted;
	HPDF_Point ogPos = HPDF_Page_GetCurrentTextPos(page);
	endText();

	HPDF_Page_SetRGBFill(page, chartColors[color][0] / 255, chartColors[color][1] / 255, chartColors[color][2] / 255);
	HPDF_Page_Rectangle(page, x, y, a, a);
	HPDF_Page_Fill(page);

	HPDF_Page_SetRGBFill(page, 0, 0, 0);

	if (wasText)
		startText(leftMargin, ogPos.y);
}

void PdfOutput::drawLine(float x1, float y1, float x2, float y2, const float color[3], float width)
{
	bool wasText = textStarted;
	HPDF_Point ogPos = HPDF_Page_GetCurrentTextPos(page);
	endText();

	HPDF_Page_SetRGBStroke(page, color[0] / 255, color[1] / 255, color[2] / 255);
	HPDF_Page_SetLineWidth(page, width);
	HPDF_Page_MoveTo(page, x1, y1);
	HPDF_Page_LineTo(page, x2, y2);
	HPDF_Page_Stroke(page);

	HPDF_Page_SetRGBStroke(page, 0, 0, 0);

	if (wasText)
		startText(leftMargin, ogPos.y);
}

void PdfOutput::drawChartOutline(float x, float y, float width, float max, int segmentCount)
{
	drawLine(x + fontSize * 0.5f, y, x + fontSize * 0.5f, y - chartHeight - fontSize * 0.5f, greyColor);
	drawLine(x, y - chartHeight, x + width, y - chartHeight, greyColor);

	int gridCount = 6;
	endText();
	for (int i = 0; i < gridCount + 1; i++)
	{
		float y2 = y - chartHeight / gridCount * i;
		startText(x - valueOffset, y2 - fontSize / 4);
		int value = max * (gridCount - i) / gridCount;
		int spaceCount = std::to_string((int)max).size() - std::to_string(value).size();
		std::string text = "";
		for (int i = 0; i < spaceCount; i++)
			text += "  ";
		text += std::to_string(value);
		HPDF_Page_ShowText(page, text.c_str());
		endText();
		drawLine(x, y2, x + width, y2, greyColor);
	}
		
	float segmentWidth = width / segmentCount;
	for (int i = 0; i < segmentCount; i++)
	{
		float x2 = x + segmentWidth * (i + 1);
		drawLine(x2, y - chartHeight - fontSize / 2, x2, y - chartHeight + fontSize / 2, greyColor);
		startText(x + segmentWidth * i + segmentWidth / 2 - fontSize/6, y - chartHeight);
		writeMessage(strToWstr(std::to_string(i + 1)));
		endText();
	}
	
	float y2 = y - chartHeight - fontSize*3;
	float x2 = x;
	std::wstringstream names;
	for (int i = 0; i < 8; i++)
	{
		drawSquare(i, x2, y2, fontSize * 0.75);
		startText(x2 + fontSize * 1.25, y2 + lineSpacing);
		writeMessage(EmotionalScore::getName(i));
		x2 += HPDF_Page_TextWidth(page, wstrToStr(EmotionalScore::getName(i)).c_str());
		x2 += HPDF_Page_TextWidth(page, "         ");
	}

	linesLeft = 0;
}

void PdfOutput::writeHeadline(std::wstring text)
{
	HPDF_Point pos = HPDF_Page_GetCurrentTextPos(page);
	HPDF_Point ogPos = pos;
	endText();
	startText(pos.x - leftMargin, pos.y-headlineFontSize, headlineFontSize);

	std::vector<std::wstring> lines = textIntoLines(text, width - sideMargin);

	int n = lines.size();
	for (int i = 0; i < n; i++)
	{
		float lineWidth = HPDF_Page_TextWidth(page, wstrToStr(lines[i]).c_str());
		float lineOffset = (width - lineWidth) / 2;
		HPDF_Page_MoveTextPos(page, lineOffset, 0);
		HPDF_Page_ShowText(page, wstrToStr(lines[i]).c_str());
		HPDF_Page_MoveTextPos(page, -lineOffset, -1 * headlineSpacing);

		linesLeft -= headlineHeight;
	}
	pos = HPDF_Page_GetCurrentTextPos(page);
	endText();
	startText(ogPos.x, pos.y+headlineFontSize);
}

std::vector<std::wstring> PdfOutput::textIntoLines(std::wstring text, int lineWidth)
{
	std::vector<std::wstring> lines;
	std::wistringstream iss(text);
	std::wstring line;
	std::wstring word;

	while (getline(iss, word, L' '))
	{
		if (word.empty())
			continue;

		size_t pos;
		while ((pos = word.find(L'\n')) != std::wstring::npos)
		{
			if (line.empty())
				lines.push_back(word.substr(0, pos));
			else
				lines.push_back(line + L" " + word.substr(0, pos));

			line.clear();
			word.erase(0, pos + 1);
		}

		if (HPDF_Page_TextWidth(page, wstrToStr(line + L" " + word).c_str()) <= lineWidth)
		{
			if (word == L"" || word == L" ")
				continue;
			if (!line.empty())
				line += L" ";
			line += word;
		}
		else
		{
			lines.push_back(line);
			line = word;
		}
	}

	if (!line.empty())
		lines.push_back(line);

	return lines;
}


void PdfOutput::createFile()
{
	pdf = HPDF_New(errorHandler, NULL);
	if (!pdf)
	{
		HPDF_Free(pdf);
		throw (std::wstring)L"Pdf file could not be created";
	}

	//HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_OUTLINE);
	//HPDF_SetCurrentEncoder(pdf, "UTF-8");
	//HPDF_GetFont(pdf, HPDF_LoadTTFontFromFile(pdf, "arial.ttf", HPDF_TRUE), "UTF-8");
}

void PdfOutput::startNewPage()
{
	endText();

	page = HPDF_AddPage(pdf);
	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

	width = HPDF_Page_GetWidth(page);
	height = HPDF_Page_GetHeight(page);

	drawLine(leftMargin, bottomMargin, width - rightMargin, bottomMargin, greyColor, 0.75);

	startText(0, 0);
	startText(width - rightMargin - HPDF_Page_TextWidth(page, std::to_string(pageNr).c_str()), bottomMargin / 2);
	HPDF_Page_ShowText(page, std::to_string(pageNr++).c_str());

	startText(leftMargin, height-topMargin);

	linesLeft = getMaxlines();
}

void PdfOutput::startText(int x, int y, float fontSize)
{
	if (textStarted)
		endText();

	textStarted = true;
	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, fontName.c_str(), "CP1250"), fontSize);

	HPDF_Page_MoveTextPos(page, x, y);
}

void PdfOutput::endText()
{
	if (!textStarted)
		return;

	textStarted = false;
	HPDF_Page_EndText(page);
}
