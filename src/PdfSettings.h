#pragma once

#include "haru/hpdf.h"
#include "Emotions.h"

const std::string fontName = "Times-Roman";
const float fontSize = 12;
const float headlineFontSize = 32;
const float lineSpacing = fontSize * 1.3f;
const float headlineSpacing = headlineFontSize * 1.3f;
const float headlineHeight = headlineSpacing / lineSpacing;

const float leftMargin = 65;
const float rightMargin = 40;
const float sideMargin = leftMargin + rightMargin;
const float topMargin = 50;
const float bottomMargin = 50;
const float heightMargin = topMargin + bottomMargin;

const float pieRadius = (emotionCount * fontSize + (emotionCount - 1) * (lineSpacing - fontSize)) / 2;
const float resultOffset = pieRadius * 2 + 40;

const float chartHeight = (emotionCount * fontSize + (emotionCount - 1) * (lineSpacing - fontSize)) * 1.5;
const float valueOffset = fontSize * 1.75;

const HPDF_PageSizes pageSize = HPDF_PAGE_SIZE_A4;
const HPDF_PageDirection pageDirection = HPDF_PAGE_PORTRAIT;

const float chartColors[8][3] =
{
	{ 94.6, 248.6, 227.7 }, // light blue
	{ 248.6, 94.6, 114.4 }, // red
	{ 94.6, 174, 248.6 }, // blue
	{ 248.6, 150.7, 94.6 }, // orange
	{ 94.6, 114.4, 248.6 }, // dark blue
	{ 248.6, 227.7, 94.6 }, // yellow
	{ 227.7, 94.6, 248.6 }, // purple
	{ 114.4, 248.6, 94.6 }, // green
};

const float greyColor[3] = { 200, 200, 200 };

std::pair<float, float> pageSizeDimensions()
{
	if (pageSize == HPDF_PAGE_SIZE_LETTER) return std::pair<float, float>(612, 792);
	if (pageSize == HPDF_PAGE_SIZE_LEGAL) return std::pair<float, float>(612, 1008);
	if (pageSize == HPDF_PAGE_SIZE_A3) return std::pair<float, float>(841.89, 1199.551);
	if (pageSize == HPDF_PAGE_SIZE_A4) return std::pair<float, float>(595.276, 841.89);
	if (pageSize == HPDF_PAGE_SIZE_A5) return std::pair<float, float>(419.528, 595.276);
	if (pageSize == HPDF_PAGE_SIZE_B4) return std::pair<float, float>(708.661, 1000.63);
	if (pageSize == HPDF_PAGE_SIZE_B5) return std::pair<float, float>(498.898, 708.661);
	if (pageSize == HPDF_PAGE_SIZE_EXECUTIVE) return std::pair<float, float>(522, 756);
	if (pageSize == HPDF_PAGE_SIZE_US4x6) return std::pair<float, float>(288, 432);
	if (pageSize == HPDF_PAGE_SIZE_US4x8) return std::pair<float, float>(288, 576);
	if (pageSize == HPDF_PAGE_SIZE_US5x7) return std::pair<float, float>(360, 504);
	if (pageSize == HPDF_PAGE_SIZE_COMM10) return std::pair<float, float>(297, 684);
	return std::pair<float, float>(0, 0);
}

//const int maxLines = 45;

int getMaxlines()
{
	std::pair<float, float> dimensions = pageSizeDimensions();
	if (pageDirection == HPDF_PAGE_PORTRAIT)
	{
		return (dimensions.second - heightMargin) / lineSpacing - 1;
	}
	else
	{
		return (dimensions.first - sideMargin) / lineSpacing - 1;
	}
}
