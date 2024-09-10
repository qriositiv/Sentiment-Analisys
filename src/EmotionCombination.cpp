#include "Emotions.h"

bool EmotionCombination::initialised = false;
const float EmotionCombination::rangePercent = 0.05f;
std::map<std::pair<int, int>, std::wstring> EmotionCombination::emotionCombinations;

void EmotionCombination::Initialise()
{
    if (initialised)
        return;

    // Dyads (Combinations):
    emotionCombinations[{joy, trust}] = L"love";
    emotionCombinations[{joy, fear}] = L"guilt";
    emotionCombinations[{joy, surprise}] = L"delight";
    emotionCombinations[{trust, fear}] = L"submission";
    emotionCombinations[{trust, surprise}] = L"curiosity";
    emotionCombinations[{trust, sadness}] = L"sentimentally";
    emotionCombinations[{fear, surprise}] = L"awe";
    emotionCombinations[{fear, sadness}] = L"despair";
    emotionCombinations[{fear, disgust}] = L"shame";
    emotionCombinations[{surprise, sadness}] = L"disapproval";
    emotionCombinations[{surprise, disgust}] = L"unbelief";
    emotionCombinations[{surprise, anger}] = L"outrage";
    emotionCombinations[{sadness, disgust}] = L"remorse";
    emotionCombinations[{sadness, anger}] = L"envy";
    emotionCombinations[{sadness, anticipation}] = L"pessimism";
    emotionCombinations[{disgust, anger}] = L"contempt";
    emotionCombinations[{disgust, anticipation}] = L"cynicism";
    emotionCombinations[{disgust, joy}] = L"morbidness";
    emotionCombinations[{anger, anticipation}] = L"aggressiveness";
    emotionCombinations[{anger, joy}] = L"pride";
    emotionCombinations[{anger, trust}] = L"dominance";
    emotionCombinations[{anticipation, joy}] = L"optimism";
    emotionCombinations[{anticipation, trust}] = L"hope";
    emotionCombinations[{anticipation, fear}] = L"anxiety";

    // Opposite combinations:
    emotionCombinations[{joy, sadness}] = L"bittersweetness";
    emotionCombinations[{trust, disgust}] = L"ambivalence";
    emotionCombinations[{fear, anger}] = L"frozenness";
    emotionCombinations[{surprise, anticipation}] = L"confusion";

    initialised = true;
}

EmotionCombination::EmotionCombination(int e1, int e2, std::wstring combinationName)
{
    this->e1 = e1;
    this->e2 = e2;
    this->combinationName = combinationName;
}

std::wstring EmotionCombination::getCombinationName()
{
    return combinationName;
}

EmotionCombination EmotionCombination::getCombination(int e1, int e2)
{
    Initialise();

    std::wstring combinationName = L"";

    std::pair<int, int> keyToFind = { e1, e2 };
    auto it = emotionCombinations.find(keyToFind);

    if (it != emotionCombinations.end())
    {
        combinationName = it->second;
    }
    else
    {
        keyToFind = { e2, e1 };
        it = emotionCombinations.find(keyToFind);

        if (it != emotionCombinations.end())
        {
            combinationName = it->second;
        }
        else;
    }

    EmotionCombination newEmotionCombination(-1, -1, combinationName);
    return newEmotionCombination;
}

EmotionCombination EmotionCombination::getCombination(EmotionalScore score)
{
    std::pair<int, int> maxEmotions = EmotionalScore::findTwoMaxEmotions(score);

    if (score.getScore(maxEmotions.first) - score.getScore(maxEmotions.second) < score.getScore(maxEmotions.first) * rangePercent)
    {
        EmotionCombination newEmotionCombination = getCombination(maxEmotions.first, maxEmotions.second);
        return newEmotionCombination;
    }
    else
    {
        EmotionCombination newEmotionCombination(-1, -1, L"");
        return newEmotionCombination;
    }
}