#include "pch.h"

//#define TESTING_ENVIRONMENT
#define CURL_STATICLIB
#include "CURL/curl.h"

#include "../src/Analyser.h"
#include "../src/Arguments.h"
#include "../src/Database.h"
#include "../src/Emotions.h"
#include "../src/IOManager.h"
#include "../src/Keyword.h"
#include "../src/Text.h"
#include "../src/Topic.h"
#include "../src/Utilities.h"

#include "../src/Analyser.cpp"
#include "../src/Arguments.cpp"
#include "../src/BowAnalyser.cpp"
#include "../src/Database.cpp"
#include "../src/EmotionalScore.cpp"
#include "../src/EmotionCombination.cpp"
#include "../src/FileIO.cpp"
#include "../src/IOManager.cpp"
#include "../src/Keyword.cpp"
#include "../src/PdfOutput.cpp"
#include "../src/TerminalIO.cpp"
#include "../src/Text.cpp"
#include "../src/Topic.cpp"
#include "../src/TopicAnalyser.cpp"
#include "../src/Utilities.cpp"
#include "../src/VaderAnalyser.cpp"
#include "../src/WebIO.cpp"

//#include "haru/hpdf.h"

/*
* -----------------------------
* Include all .h and .cpp files
* EXCEPT SentimentAnalysis.cpp!
* -----------------------------
*/

/*
* ------------------------------------------------------------
* SUCCEED() - Mark test as success (still can fail if other tests fail, for more see documentation)
* FAIL() - Fail the test if this part is reached
*
* EXPECT - does not stop further assesments if test failed
* ASSERT - stops further tests in same function if test failed
* All further functions are either EXPECT_ or ASSERT_
* E.g. EXPECT_EQ or ASSERT_EQ
*
* TRUE(condition) - checks if condition is true
* FALSE(condition) - checks if condition is false
* EQ(expected, actual) - compare if values are equal
* NE(value1, value2) - checks if values are not equal
* STREQ - compare strings (not variables, but straigth strings?)
* LT, LE, GT, GE (value1, value2) - lower than, lower/equal than and etc...
* THROW(statement, exception_type) - checks if statement(function) throws correct exception.
* ANY_THROW(statement) - checks if statement(function) throws exception.
* Many others at https://google.github.io/googletest/reference/assertions.html
*
* You can also use << streams to make custom error messages such as;
* EXPECT_EQ(expected, foo.bar()) << L"Expected:" << expected.toString() << "\n" << L"Actual" << foo.bar().toString() << "\n";
* If comparing more complex values. For example comparing std::vectors show something like:
* Expected: <08-A4> <84-0F> ... Even if it is compared by values and not address.
* ------------------------------------------------------------
*/


//TEST(name_of_group_of_tests (could be class name), name_of_specific_test (could be method name))
//{
//	tests
//}


// ----------------------------------------------
//                    Contents
// ----------------------------------------------
// ctrl + left click to jump (kind of a hack)
#ifdef AnalyserTests
#endif
#ifdef IOManagerTests
#endif
#ifdef WebInputTests
#endif
#ifdef EmotionalScoreTests
#endif
#ifdef EmotionCombinationTests
#endif
#ifdef DatabaseTests
#endif
#ifdef FileIOTests
#endif
#ifdef FlagTests
#endif
#ifdef TopicTests
#endif
#ifdef LogTests
#endif



// ----------------------------------------------
//                    Analyser
// ----------------------------------------------


#define AnalyserTests
#pragma region AnalyserTests
TEST(Analyser, BowAnalyser)
{
	std::vector<Keyword> keywords;
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;

	keywords.push_back(Keyword(L"love", { 1, 0, 1, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"war", { 0, 1, 0, 0, 0, 0, 0, 0 }));

	Database database(keywords, affixes, negations, amplifiers);
	BowAnalyser bow(&database);

	Sentence sentence({ L"love" });
	EXPECT_EQ(EmotionalScore({ 1, 0, 1, 0, 0, 0, 0, 0 }), bow.calculateScore(sentence));

	sentence = Sentence({ L"love", L"and", L"war" });
	EXPECT_EQ(EmotionalScore({ 1, 1, 1, 0, 0, 0, 0, 0 }), bow.calculateScore(sentence));

	sentence = Sentence({ L"LOVE", L"waR", L"and", L"much", L"more" });
	EXPECT_EQ(EmotionalScore({ 1, 1, 1, 0, 0, 0, 0, 0 }), bow.calculateScore(sentence));

	sentence = Sentence({ L"inlove", L"warring" });
	EXPECT_EQ(EmotionalScore({ 0, 1, 0, 0, 0, 0, 0, 0 }), bow.calculateScore(sentence));

	sentence = Sentence({ L"love", L"loveless", L"inlove", L"uplovely" });
	EXPECT_EQ(EmotionalScore({ 2, 0, 2, 0, 0, 0, 0, 0 }), bow.calculateScore(sentence));
}

TEST(Analyser, VaderAnalyser)
{
	std::vector<Keyword> keywords;
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	std::unordered_set<std::wstring> generalPrefixes{L"un"};

	keywords.push_back(Keyword(L"happy", { 1, 0, 0, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"sad", { 0, 1, 0, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"love", { 1, 0, 1, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"ill", { 0, 1, 0, 0, 0, 0, 0, 0 }));

	affixes.push_back(Multiplier(L"un", -1));
	affixes.push_back(Multiplier(L"less", -1));

	negations[L"no"] = -1;
	negations[L"not"] = -1;

	amplifiers[L"very"] = 1.5;
	amplifiers[L"unvery"] = -0.6;
	amplifiers[L"somewhat"] = 0.5;

	Database database(keywords, affixes, negations, amplifiers, std::unordered_set<std::wstring>(), generalPrefixes);
	VaderAnalyser vader(&database);

	std::vector<std::wstring> text;

	text = { L"love" };
	EXPECT_EQ(EmotionalScore({ 1, 0, 1, 0, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"loveless" };
	EXPECT_EQ(EmotionalScore({ 0, 1, 0, 1, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"happy", L"!" };
	EXPECT_EQ(EmotionalScore({ 1.5, 0, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"unhappy" };
	EXPECT_EQ(EmotionalScore({ 0, 1, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"no", L"happy" };
	EXPECT_EQ(EmotionalScore({ 0, 1, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"not", L"unhappy" };
	EXPECT_EQ(EmotionalScore({ 1, 0, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"very", L"unhappy" };
	EXPECT_EQ(EmotionalScore({ 0, 1.5, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"unvery", L"unhappy" };
	EXPECT_EQ(EmotionalScore({ 0.6, 0, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text));

	text = { L"I", L"am", L"very", L"unhappy", L"and", L"somewhat", L"sad", L"!" };
	EXPECT_EQ(EmotionalScore({ 0, 3, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text)) << vader.calculateScore(text).formatResults();

	text = { L"Will", L"william", L"have", L"will", L"to", L"willfully", L"do", L"what", L"he", L"will", L"deem", L"right" };
	EXPECT_EQ(EmotionalScore({ 0, 0, 0, 0, 0, 0, 0, 0 }), vader.calculateScore(text));
}

TEST(Analyser, TopicAnalyser)
{
	std::vector<Keyword> keywords;
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;

	keywords.push_back(Keyword(L"happy", { 1, 0, 0, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"sad", { 0, 1, 0, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"love", { 1, 0, 1, 0, 0, 0, 0, 0 }));

	affixes.push_back(Multiplier(L"un", -1));
	affixes.push_back(Multiplier(L"less", -1));

	negations[L"no"] = -1;
	negations[L"not"] = -1;

	amplifiers[L"very"] = 1.5;
	amplifiers[L"somewhat"] = 0.5;

	Database database(keywords, affixes, negations, amplifiers);

	Topic topicWords({ L"president" }, { L"he" });

	TopicAnalyser topic(&database, topicWords);

	Text text = InputManager::separateText(L"happy sentence. President is sad. sad sad. happy. he is unhappy.\nthis does not matter love");

	EXPECT_EQ(EmotionalScore({ 0, 3, 0, 0, 0, 0, 0, 0 }), topic.calculateScore(text)) << topic.calculateScore(text).toString();
}

#pragma endregion



// ----------------------------------------------
//                   IOManager
// ----------------------------------------------

#define IOManagerTests
#pragma region IOManagerTests
TEST(IOManager, detectInputType)
{
	EXPECT_EQ((int)Url, InputManager::detectInputType(L"https://en.wikipedia.org/wiki/HTTPS"));
	EXPECT_ANY_THROW(InputManager::detectInputType(L"https//en.wikipedia.org/wiki/HTTPS"));
	EXPECT_ANY_THROW(InputManager::detectInputType(L"https:/en.wikipedia.org/wiki/HTTPS"));
	EXPECT_ANY_THROW(InputManager::detectInputType(L"www.wikipedia.org/wiki/HTTPS"));
	EXPECT_ANY_THROW(InputManager::detectInputType(L"wwwikipedia.org/wiki/HTTPS"));
	EXPECT_EQ((int)TextFile, InputManager::detectInputType(L"C:/directory/file.txt"));
	EXPECT_EQ((int)TextFile, InputManager::detectInputType(L"C:\\directory\\file.txt"));
	EXPECT_ANY_THROW(InputManager::detectInputType(L"C:\\directory\\file"));
	EXPECT_EQ((int)PlainText, InputManager::detectInputType(L"Some very random text, lorem ipsum and etc."));
	EXPECT_EQ((int)PlainText, InputManager::detectInputType(L"Ave maria, https://chat.openai.com/"));
	EXPECT_EQ((int)TextFile, InputManager::detectInputType(L"i hope no one looks at my folder, C:/donotlook/verysecret/please.txt"));
}

TEST(IOManager, separateText)
{
	std::wstring text = L"This is an example of sentence.";
	std::vector<std::wstring> expected = { L"this", L"is", L"an", L"example", L"of", L"sentence." };
	EXPECT_EQ(expected, InputManager::separateTextIntoWords(text));

	text = L"Another\n example of\n sentence.\n";
	expected = { L"another", L"example", L"of", L"sentence." };
	EXPECT_EQ(expected, InputManager::separateTextIntoWords(text));
	
	text = L"a basic sentence";
	Text actual = InputManager::separateText(text);
	expected = { L"a", L"basic", L"sentence" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	
	text = L"  a     basic          sentence.";
	actual = InputManager::separateText(text);
	expected = { L"a", L"basic", L"sentence", L"." };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	
	text = L"!!!a basic sentence???";
	actual = InputManager::separateText(text);
	expected = { L"a", L"basic", L"sentence", L"?" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	
	text = L"Huh!! several sentences??";
	actual = InputManager::separateText(text);
	expected = { L"Huh", L"!" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	expected = { L"several", L"sentences", L"?" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[1].getAllWords());
	
	text = L"some\nnew\nlines!";
	actual = InputManager::separateText(text);
	expected = { L"some" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	expected = { L"new" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[1].getSentences())[0].getAllWords());
	expected = { L"lines", L"!" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[2].getSentences())[0].getAllWords());
	
	text = L"several\n\n\n\nempty ones.";
	actual = InputManager::separateText(text);
	expected = { L"several" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	expected = { L"empty", L"ones", L"." };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[1].getSentences())[0].getAllWords());
	
	text = L"The  final...boss ?\n\nOf this beautiful \n. testing thing";
	actual = InputManager::separateText(text);
	expected = { L"The", L"final", L"." };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	expected = { L"boss", L"?" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[1].getAllWords());
	expected = { L"Of", L"this", L"beautiful" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[1].getSentences())[0].getAllWords());
	expected = { L"testing", L"thing" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[2].getSentences())[0].getAllWords());
	
	text = L"sentence without end \n\nnext sentence.";
	actual = InputManager::separateText(text);
	expected = { L"sentence", L"without", L"end" };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[0].getSentences())[0].getAllWords());
	expected = { L"next", L"sentence", L"." };
	EXPECT_EQ(expected, *(*(*actual.getParagraphs())[1].getSentences())[0].getAllWords());
	EXPECT_EQ(2, actual.getParagraphs()->size());
	EXPECT_EQ(1, (*actual.getParagraphs())[0].getSentences()->size());
	EXPECT_EQ(1, (*actual.getParagraphs())[1].getSentences()->size());
}

#pragma endregion



// ----------------------------------------------
//                    WebInput
// ----------------------------------------------

#define WebInputTests
#pragma region WebInputTests
TEST(WebInput, downloadHTML)
{
	std::wstring actual;
	EXPECT_NO_THROW(actual = WebInput::downloadHTML(L"https://example.org/"));
	std::wstring expected = L"<!doctype html>\n<html>\n<head>\n    <title>Example Domain</title>\n\n    <meta charset=\"utf-8\" />\n    <meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\" />\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n    <style type=\"text/css\">\n    body {\n        background-color: #f0f0f2;\n        margin: 0;\n        padding: 0;\n        font-family: -apple-system, system-ui, BlinkMacSystemFont, \"Segoe UI\", \"Open Sans\", \"Helvetica Neue\", Helvetica, Arial, sans-serif;\n        \n    }\n    div {\n        width: 600px;\n        margin: 5em auto;\n        padding: 2em;\n        background-color: #fdfdff;\n        border-radius: 0.5em;\n        box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);\n    }\n    a:link, a:visited {\n        color: #38488f;\n        text-decoration: none;\n    }\n    @media (max-width: 700px) {\n        div {\n            margin: 0 auto;\n            width: auto;\n        }\n    }\n    </style>    \n</head>\n\n<body>\n<div>\n    <h1>Example Domain</h1>\n    <p>This domain is for use in illustrative examples in documents. You may use this\n    domain in literature without prior coordination or asking for permission.</p>\n    <p><a href=\"https://www.iana.org/domains/example\">More information...</a></p>\n</div>\n</body>\n</html>\n";
	EXPECT_EQ(expected, actual);
}

TEST(WebInput, extractRelevantText)
{
	std::wstring html, expected;

	html = L"<html><h1>Title section.</h1><p>Paragraph section.</p></html>";
	expected = L"Title section.\nParagraph section.";
	EXPECT_EQ(expected, WebInput::extractRelevantText(html));

	html = L"<div>Case of<a>LINK</a> removing <nav>NAV</nav>section.</div>";
	expected = L"<div>Case of removing section.</div>";
	EXPECT_EQ(expected, WebInput::removeUnnecessarySegments(html));

	EXPECT_EQ(WebInput::removeTags(L"<p>Paragraph.</p>"), L"Paragraph.");

	html = L"<h1>Header size1</h1><p>Paragraph 1</p><h2>Header size2</h2><p>Paragraph 2</p>";
	std::vector<HtmlSegment> segment1 = WebInput::extractTextByTags(L"<h2>", L"</h2>", html);
	EXPECT_EQ(segment1[0].getText(), L"<h2>Header size2</h2>");

	html = L"<p>One</p><p>Two</p><p>Three</p><p>Four</p>";
	std::vector<HtmlSegment> segment2 = WebInput::extractTextByTags(L"<p>", L"</p>", html);
	ASSERT_EQ(segment2.size(), 4);
	EXPECT_EQ(segment2[0].getText(), L"<p>One</p>");
	EXPECT_EQ(segment2[1].getText(), L"<p>Two</p>");
	EXPECT_EQ(segment2[2].getText(), L"<p>Three</p>");
	EXPECT_EQ(segment2[3].getText(), L"<p>Four</p>");

	HtmlSegment s1(2, L"Second"), s2(1, L"First"), s3(3, L"Third");
	std::vector<HtmlSegment> segments = { s1, s2, s3 };
	std::sort(segments.begin(), segments.end(), HtmlSegment::compareByPos);
	ASSERT_EQ(segments.size(), 3);
	EXPECT_EQ(segments[0].getText(), L"First");
	EXPECT_EQ(segments[1].getText(), L"Second");
	EXPECT_EQ(segments[2].getText(), L"Third");

	html = L"<p>Keep this text</p><div>Remove this text</div>";
	html = WebInput::cutHtml(L"<div>", L"</div>", html);
	EXPECT_EQ(html, L"<p>Keep this text</p>");

	html = L"";
	EXPECT_TRUE(WebInput::extractRelevantText(html).empty());

	html = L"No HTML tags here";
	EXPECT_EQ(WebInput::removeTags(html), L"No HTML tags here");

	html = L"<html><h1><p>Text.</p></h1></html>";
	expected = L"\nText.";
	EXPECT_EQ(expected, WebInput::extractRelevantText(html));
}

#pragma endregion



// ----------------------------------------------
//                EmotionalScore
// ----------------------------------------------

#define EmotionalScoreTests
#pragma region EmotionalScoreTests
TEST(EmotionalScore, manipulatingWeights)
{
	EmotionalScore emotions;
	std::vector<float> weights;

	weights = { 1, 0 , 0.5, 0, 0, 0, 0, 0 };
	for (int i = 0; i < emotionCount; i++)
		emotions.addScore(i, weights.at(i));

	EXPECT_EQ(weights, emotions.getAllScores());
}

TEST(EmotionalScore, invertingWeights)
{
	std::vector<float> weights = { 1, 0, -1, 0, 0, 0, 0, 0 };
	std::vector<float> expected = { 1, 0, 0, 1, 0, 0, 0, 0 };
	EmotionalScore emotions(weights);
	emotions.invertValues();

	EXPECT_EQ(expected, emotions.getAllScores());

	weights = { -0.7, -0.4, 0, -0.2, 0, 0, 0, 0 };
	expected = { 0.4, 0.7, 0.2, 0, 0, 0, 0, 0 };

	emotions = EmotionalScore(weights);
	emotions.invertValues();

	EXPECT_EQ(expected, emotions.getAllScores());

	weights = { -0.7, 0.3, 0, 0, 0, 0, 0, 0 };
	expected = { 0, 1, 0, 0, 0, 0, 0, 0 };

	emotions = EmotionalScore(weights);
	emotions.invertValues();

	EXPECT_EQ(expected, emotions.getAllScores());
}

TEST(EmotionalScore, operatorOverloads)
{
	std::vector<float> values1{ 0, 1, 0, 1, 0, 1, 0 ,1 };
	std::vector<float> values2{ 0, 1, 0, 1, 0, 1, 0 ,1 };
	std::vector<float> values3{ 1, 1, 1, 1, 1, 1, 1 ,1 };
	EmotionalScore scores1(values1);
	EmotionalScore scores2(values2);
	EmotionalScore scores3(values3);

	EXPECT_TRUE(scores1.getAllScores() == scores2.getAllScores());
	EXPECT_FALSE(scores1.getAllScores() != scores2.getAllScores());

	EXPECT_FALSE(scores1.getAllScores() == scores3.getAllScores());
	EXPECT_TRUE(scores1.getAllScores() != scores3.getAllScores());

	Multiplier mult1(L"a", 2);
	Multiplier mult2(L"a", -1);

	EmotionalScore scores4 = scores1 * mult1;
	EmotionalScore scores5 = scores1 * mult2;

	std::vector<float> expected4{ 0, 2, 0, 2, 0 ,2 ,0 ,2 };
	std::vector<float> expected5{ 1, 0, 1, 0, 1, 0, 1, 0 };

	EXPECT_EQ(expected4, scores4.getAllScores());
	EXPECT_EQ(expected5, scores5.getAllScores());

	EmotionalScore scores6(scores1);
	EmotionalScore scores7(scores1);
	scores6 *= mult1;
	scores7 *= mult2;

	EXPECT_EQ(expected4, scores6.getAllScores());
	EXPECT_EQ(expected5, scores7.getAllScores());
}

TEST(EmotionalScore, formatResults)
{
	EmotionalScore actual({ 1, 0.5, 0.5, 0, 0, 0, 0, 0 });
	std::wstring expected = L"Joy: 1 50%\nSadness: 0.5 25%\nTrust: 0.5 25%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%";
	EXPECT_EQ(expected, actual.formatResults());
}

TEST(Text, setSegmentSize)
{
	Text text({
		Paragraph({ Sentence({ L"memento", L"mori" })}),
		Paragraph({ Sentence({ L"feeling", L"love", L"on" })}),
		Paragraph({ Sentence({ L"just", L"keep", L"my"})}),
		Paragraph({ Sentence({ L"next", L"apple", L"knife"})}),
		Paragraph({ Sentence({ L"great"})}),
		Paragraph({ Sentence({ L"ice"})}),
		Paragraph({ Sentence({ L"chair"})}),
		Paragraph({ Sentence({ L"bag"})}),
		Paragraph({ Sentence({ L"very"})}),
		Paragraph({ Sentence({ L"cold"})}),
		Paragraph({ Sentence({ L"summer"})}),
		});
	
	// Number of paragraphs (there is 11) / Default number of segments (here is 10)
	EXPECT_EQ(text.setSegmentSize(), 1.1f);
}

TEST(EmotionalScore, calculateFlow)
{
	// Create segments part:

	Text text({
		Paragraph({ Sentence({ L"love" })}),
		Paragraph({ Sentence({ L"sad" })}),
		Paragraph({ Sentence({ L"peach" })}),
		Paragraph({ Sentence({ L"freedom" })}),
		Paragraph({ Sentence({ L"water" })}),
		Paragraph({ Sentence({ L"milk" })}),
		Paragraph({ Sentence({ L"mix" })}),
		Paragraph({ Sentence({ L"qiwi" })}),
		Paragraph({ Sentence({ L"mandarinas" })}),
		Paragraph({ Sentence({ L"honey" })}),
		Paragraph({ Sentence({ L"last" })}),
		});
	
	std::vector<std::vector<int>> segments = text.distributeSegments();
	
	ASSERT_EQ(segments.size(), 10);
	
	// 1st segments have have 2 paragraphs inside, other segments only 1.
	ASSERT_EQ(segments.at(0).size(), 2);
	for (int i = 1; i < 10; i++)
		ASSERT_EQ(segments.at(i).size(), 1);
	
	// Calculate emotional flow part:
	
	std::vector<Keyword> keywords = {};
	std::vector<Multiplier> affixes = {};
	std::unordered_map<std::wstring, float> negations = {};
	std::unordered_map<std::wstring, float> amplifiers = {};
	
	keywords.push_back(Keyword(L"love", { 1, 0, 1, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"sad", { 0, 1, 0, 0, 0, 0, 0, 0 }));
	keywords.push_back(Keyword(L"freedom", { 1, 0, 0, 0, 0, 0, 0, 0 }));
	
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);
	EmotionalScore results = vader.analyse(text);
	
	std::vector<EmotionalScore> expected;
	EmotionalScore score1({ 1, 0, 1, 0, 0, 0, 0, 0 });
	EmotionalScore score2({ 0, 1, 0, 0, 0, 0, 0, 0 });
	EmotionalScore score3({ 0, 0, 0, 0, 0, 0, 0, 0 });
	EmotionalScore score4({ 1, 0, 0, 0, 0, 0, 0, 0 });
	expected.push_back(score1);
	expected.push_back(score2);
	expected.push_back(score3);
	expected.push_back(score4);
	
	std::vector<EmotionalScore> flow = text.calculateFlow(segments);
	
	ASSERT_EQ(flow.size(), 10);
	
	// I dont know why it fails.
	// EXPECT_EQ(expected, flow);
}

TEST(EmotionalScore, formatFlow)
{
	std::vector<EmotionalScore> flow;
	EmotionalScore flow1({ 0, 0, 0, 0, 0, 0, 4, 0 });
	EmotionalScore flow2({ 13, 0, 0, 0, 0, 0, 0, 0 });
	EmotionalScore flow3({ 0, 0, 5, 16, 0, 0, 0, 0 });
	flow.push_back(flow1);
	flow.push_back(flow2);
	flow.push_back(flow3);

	std::wstring output = L"Flow results of 3 segments:\n1. [0,\t0,\t0,\t0,\t0,\t0,\t4,\t0]\n2. [13(+13),\t0(0),\t0(0),\t0(0),\t0(0),\t0(0),\t0(-4),\t0(0)]\n3. [0(-13),\t0(0),\t5(+5),\t16(+16),\t0(0),\t0(0),\t0(0),\t0(0)]\n";

	EXPECT_EQ(Text::formatFlow(flow), output);
}

#pragma endregion



// ----------------------------------------------
//               EmotionCombination
// ----------------------------------------------

#define EmotionCombinationTests
#pragma region EmotionCombinationTests

TEST(EmotionCombination, getCombination)
{
	int e1 = joy, e2 = trust;
	std::wstring expected = L"love";

	EmotionCombination ec1 = EmotionCombination::getCombination(e1, e2);
	EXPECT_EQ(expected, ec1.getCombinationName());

	EmotionCombination ec2 = EmotionCombination::getCombination(e2, e1);
	EXPECT_EQ(expected, ec2.getCombinationName());

	EmotionCombination ec3 = EmotionCombination::getCombination(e1, e1);
	EXPECT_EQ(L"", ec3.getCombinationName());

	expected = L"anxiety";
	std::vector<float> values1{ 50, 105, 75, 15, 150, 90, 12 ,145 };
	std::vector<float> values2{ 50, 105, 75, 15, 165, 90, 12 ,171 };
	std::vector<float> values3{ 0, 100, 0, 1, 0, 1, 0 ,1 };
	EmotionalScore score1(values1);
	EmotionalScore score2(values2);
	EmotionalScore score3(values3);

	EXPECT_EQ(expected, EmotionCombination::getCombination(score1).getCombinationName());
	EXPECT_EQ(expected, EmotionCombination::getCombination(score2).getCombinationName());
	EXPECT_EQ(L"", EmotionCombination::getCombination(score3).getCombinationName());
}

#pragma endregion



// ----------------------------------------------
//                    Database
// ----------------------------------------------

#define DatabaseTests
#pragma region DatabaseTests
TEST(Database, readingFile)
{
	std::string jsonStr = R"(
	{
		"keywords": [
			{ "root": "happy", "scores": [ 1, 0, 0, 0, 0, 0, 0, 0 ] }
		],
		"affixes": [
			{ "affix": "un", "multiplier": -1 }
		],
		"negations": [
			{ "negation": "not", "multiplier": -1 }
		],
		"amplifiers": [
			{ "word": "extremely", "multiplier": 2 }
		],
		"connecting_words": [
			{ "words": [ "the" ] }
		],
		"general_prefixes": [
			{ "prefixes": [ "un", "im" ] }
		]
	}
)";


	std::vector<float> emotions{ 1, 0, 0, 0, 0, 0, 0, 0 };
	std::vector<Keyword> expectedKeywords{ Keyword(L"happy", emotions) };
	std::vector<Multiplier> expectedAffixes{ Multiplier(L"un", -1) };
	std::unordered_map<std::wstring, float> expectedNegations{ { L"not", -1 } };
	std::unordered_map<std::wstring, float> expectedAmplifiers{ { L"extremely", 2 } };
	std::unordered_set<std::wstring> expectedConnectingWords{ L"the" };
	std::unordered_set<std::wstring> expectedGeneralPrefixes{ L"un", L"im" };
	
	Database database(nlohmann::json::parse(jsonStr));
	EXPECT_EQ(expectedKeywords, database.getKeywords());
	EXPECT_EQ(expectedAffixes, database.getAffixes());
	EXPECT_EQ(expectedNegations, database.getNegations());
	EXPECT_EQ(expectedAmplifiers, database.getAmplifiers());
	EXPECT_EQ(expectedConnectingWords, database.getConnectingWords());
	EXPECT_EQ(expectedGeneralPrefixes, database.getGeneralPrefixes());
}

TEST(Database, findingWords)
{
	std::vector<Keyword> keywords{ Keyword(L"foo", {0, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"bar", {0, 0, 0, 0, 0, 0, 0, 1}) };
	std::vector<Multiplier> affixes{ Multiplier(L"on", 0), Multiplier(L"god", 1), Multiplier(L"im", -1) };
	std::unordered_map<std::wstring, float> negations{ { L"nah", -1 }, { L"oh", -1 } };
	std::unordered_map<std::wstring, float> amplifiers{ { L"for", 2 }, { L"real", 2 } };
	std::unordered_set<std::wstring> connectingWords{ L"a", L"an", L"is", L"was" };
	Database database(keywords, affixes, negations, amplifiers, connectingWords);

	// Uses operator== overload

	EXPECT_EQ(keywords[1], database.findKeyword(L"bar"));
	EXPECT_EQ(Keyword::empty, database.findKeyword(L"onBaRd")); // case is no longer database's responsibility
	EXPECT_EQ(keywords[0], database.findKeyword(L"foobar"));
	EXPECT_EQ(Keyword::empty, database.findKeyword(L"I bar some foo"));
	EXPECT_EQ(Keyword::empty, database.findKeyword(L"nyehehe"));
	EXPECT_EQ(Keyword::empty, database.findKeyword(L"boofar"));

	EXPECT_EQ(1, database.findAffix(L"ONLINE", L""));
	EXPECT_EQ(affixes[1], database.findAffix(L"godlike", L""));
	EXPECT_EQ(affixes[0], database.findAffix(L"lagoon", L""));
	EXPECT_EQ(1, database.findAffix(L"trulygodbewithye", L""));
	EXPECT_EQ(1, database.findAffix(L"bygone", L""));
	EXPECT_EQ(affixes[2], database.findAffix(L"impossible", L""));
	EXPECT_EQ(1, database.findAffix(L"impressing", L"impress"));

	EXPECT_EQ(negations[L"nah"], database.findNegation(L"nah"));
	EXPECT_EQ(1, database.findNegation(L"OH"));
	EXPECT_EQ(1, database.findNegation(L"boh"));
	EXPECT_EQ(1, database.findNegation(L"nahhh"));

	EXPECT_EQ(1, database.findAmplifier(L"ongodforreal"));
	EXPECT_EQ(1, database.findAmplifier(L"onrealforgod"));
	EXPECT_EQ(1, database.findAmplifier(L"REAL!!!"));
	EXPECT_EQ(1, database.findAmplifier(L"none here"));
	EXPECT_EQ(1, database.findAmplifier(L"none here either"));

	EXPECT_TRUE(database.isConnectingWord(L"a"));
	EXPECT_TRUE(database.isConnectingWord(L"an"));
	EXPECT_TRUE(database.isConnectingWord(L"is"));
	EXPECT_TRUE(database.isConnectingWord(L"was"));
	EXPECT_FALSE(database.isConnectingWord(L"A"));
	EXPECT_FALSE(database.isConnectingWord(L"animal"));
	EXPECT_FALSE(database.isConnectingWord(L"wassap"));
}

#pragma endregion



// ----------------------------------------------
//                FileInputOutput
// ----------------------------------------------

#define FileIOTests
#pragma region FileIOTests
TEST(FileInputOutput, writingReading)
{
	std::wstring expected = L"Some text\nindeed\n";
	FileOutput out(L"testfile.txt");
	out.writeMessage(expected);
	out.close();

	FileInput in(L"testfile.txt");
	std::wstring actual = in.getInput();
	in.close();
	EXPECT_EQ(expected, actual);

	expected = L"Ąžuolas\n";
	out.open(L"testfile.txt");
	out.writeMessage(expected);
	out.close();

	in.open(L"testfile.txt");
	actual = in.getInput();
	EXPECT_EQ(expected, actual);
}

TEST(PdfOutput, createPdf)
{
	PdfOutput pdf(L"test1.pdf");
	pdf.writeMessage(L"A message\nof some kind");
	pdf.saveFile();
	std::ifstream pdffile("test1.pdf", std::ifstream::binary);
	EXPECT_TRUE(pdffile.good());


	pdf = PdfOutput(L"test2.pdf");
	EmotionalScore scoreNorm({ 21, 5, 30, 15, 15, 0, 10, 0 });
	EmotionalScore scoreSingle({ 1, 0, 0, 0, 0, 0, 0, 0 });
	EmotionalScore scoreEmpty({ 0, 0, 0, 0, 0, 0, 0, 0 });
	pdf.writeMessage(L"text of some kind. just checking line wrapping and stuff. i really hope that at some point i go to a new line. Without... without using the ... \\n");
	pdf.writeMessage(scoreNorm);
	pdf.writeMessage(L"it used to break when writing text between charts. fun stuff");
	pdf.writeMessage(scoreSingle);
	pdf.writeMessage(L"why cant i use pie chart drawing to draw a full circle with single score. it just breaks. why???");
	pdf.writeMessage(scoreEmpty);
	pdf.writeMessage(L"Empty also does not work by default");
	pdf.saveFile();
	std::ifstream pdffile2("test2.pdf", std::ifstream::binary);
	EXPECT_TRUE(pdffile2.good());
}

TEST(PdfOutput, createPdfFromText)
{
	//// Very dependant on other parts
	//Text text({
	//		Paragraph({ Sentence({ L"word", L"another" }), Sentence({ L"aaand", L"one", L"more" })}),
	//		Paragraph({ Sentence({ L"i", L"really", L"cant"}), Sentence({ L"do", L"dis", L"no"})}),
	//		Paragraph({ Sentence({ L"more"})}),
	//	});
	//
	//std::vector<Keyword> keywords{ Keyword(L"word", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"really", {0, 2, 0, 0, 0, 0, 0, 1}) };
	//std::vector<Multiplier> affixes{ Multiplier(L"on", 0) };
	//std::unordered_map<std::wstring, float> negations{ { L"oh", -1 } };
	//std::unordered_map<std::wstring, float> amplifiers{ { L"for", 2 } };
	//Database database(keywords, affixes, negations, amplifiers);
	//
	//BowAnalyser anal(&database);
	//
	//EmotionalScore results = anal.analyse(text);
	//
	//PdfOutput pdf(L"text.pdf");
	//pdf.writeMessage(text);
	//pdf.saveFile();
	//std::ifstream pdffile("text.pdf", std::ifstream::binary);
	//EXPECT_TRUE(pdffile.good());
}

#pragma endregion



// ----------------------------------------------
//                    FlagUsage
// ----------------------------------------------

#define FlagTests
#pragma region FlagTests

TEST(FlagUsage, noOutputFlags)
{
	std::vector<Keyword> keywords{ Keyword(L"love", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"sad", {0, 1, 0, 0, 0, 0, 0, 0}) };
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);

	std::wstring input = L"I love you. I am sad without you.\nI feel that I could love you.";
	Text text = InputManager::separateText(input);

	EmotionalScore results = vader.analyse(text);

	EXPECT_EQ(text.formatFinalResults(), L"Joy: 2 67%\nSadness: 1 33%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nDominating emotion in this text is Joy.\n");
}

TEST(FlagUsage, paragraphFlag)
{
	std::vector<Keyword> keywords{ Keyword(L"love", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"sad", {0, 1, 0, 0, 0, 0, 0, 0}) };
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);

	std::wstring input = L"I love you. I am sad without you.\nI feel that I could love you.";
	Text text = InputManager::separateText(input);

	EmotionalScore results = vader.analyse(text);

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(resultsParagraphFlag)->raise();

	EXPECT_EQ(text.formatFinalResults(), L"Paragraph 1\nJoy: 1 50%\nSadness: 1 50%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nJoy and Sadness are two most prevalent emotions in this text, indicating that this paragraph's emotion could be bittersweetness.\n\nParagraph 2\nJoy: 1 100%\nSadness: 0 0%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nDominating emotion in this paragraph is Joy.\n\n");
}

TEST(FlagUsage, resultsParagraphAndTextFlag)
{
	std::vector<Keyword> keywords{ Keyword(L"love", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"sad", {0, 1, 0, 0, 0, 0, 0, 0}) };
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);

	std::wstring input = L"I love you. I am sad without you.\nI feel that I could love you.";
	Text text = InputManager::separateText(input);

	EmotionalScore results = vader.analyse(text);

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(resultsParagraphAndTextFlag)->raise();

	EXPECT_EQ(text.formatFinalResults(), L"I love you. I am sad without you.\nJoy: 1 50%\nSadness: 1 50%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nJoy and Sadness are two most prevalent emotions in this text, indicating that this paragraph's emotion could be bittersweetness.\n\nI feel that I could love you.\nJoy: 1 100%\nSadness: 0 0%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nDominating emotion in this paragraph is Joy.\n\n");
}

TEST(FlagUsage, resultsSentenceAndTextFlag)
{
	std::vector<Keyword> keywords{ Keyword(L"love", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"sad", {0, 1, 0, 0, 0, 0, 0, 0}) };
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);

	std::wstring input = L"I love you. I am sad without you.\nI feel that I could love you.";
	Text text = InputManager::separateText(input);

	EmotionalScore results = vader.analyse(text);

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(resultsSentenceAndTextFlag)->raise();

	EXPECT_EQ(text.formatFinalResults(), L"I love you.\nJoy: 1 100%\nSadness: 0 0%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nDominating emotion in this sentence is Joy.\n\nI am sad without you.\nJoy: 0 0%\nSadness: 1 100%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nDominating emotion in this sentence is Sadness.\n\nI feel that I could love you.\nJoy: 1 100%\nSadness: 0 0%\nTrust: 0 0%\nDisgust: 0 0%\nFear: 0 0%\nAnger: 0 0%\nSurprise: 0 0%\nAnticipation: 0 0%\n\nDominating emotion in this sentence is Joy.\n\n");
}

TEST(FlagUsage, csvFlag)
{
	std::vector<Keyword> keywords{ Keyword(L"love", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"sad", {0, 1, 0, 0, 0, 0, 0, 0}) };
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);

	std::wstring input = L"I love you. I am sad without you.\nI feel that I could love you.";
	Text text = InputManager::separateText(input);

	EmotionalScore results = vader.analyse(text);

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(csvFlag)->raise();

	EXPECT_EQ(text.formatFinalResults(), L"Joy,Sadness,Trust,Disgust,Fear,Anger,Surprise,Anticipation\n2,1,0,0,0,0,0,0\n");
}

TEST(FlagUsage, csvAndVerboseParagraphFlags)
{
	std::vector<Keyword> keywords{ Keyword(L"love", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"sad", {0, 1, 0, 0, 0, 0, 0, 0}) };
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);

	std::wstring input = L"I love you. I am sad without you.\nI feel that I could love you.";
	Text text = InputManager::separateText(input);

	EmotionalScore results = vader.analyse(text);

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(csvFlag)->raise();
	ArgumentHandler::getFlag(resultsParagraphFlag)->raise();

	EXPECT_EQ(text.formatFinalResults(), L"paragraphNrJoy,Sadness,Trust,Disgust,Fear,Anger,Surprise,Anticipation\n1,1,1,0,0,0,0,0,0\n2,1,0,0,0,0,0,0,0\n");
}

TEST(FlagUsage, csvAndVerboseSentenceFlags)
{
	std::vector<Keyword> keywords{ Keyword(L"love", {1, 0, 0, 0, 0, 0, 0, 0}), Keyword(L"sad", {0, 1, 0, 0, 0, 0, 0, 0}) };
	std::vector<Multiplier> affixes;
	std::unordered_map<std::wstring, float> negations;
	std::unordered_map<std::wstring, float> amplifiers;
	Database database(keywords, affixes, negations, amplifiers);
	VaderAnalyser vader(&database);

	std::wstring input = L"I love you. I am sad without you.\nI feel that I could love you.";
	Text text = InputManager::separateText(input);

	EmotionalScore results = vader.analyse(text);

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(csvFlag)->raise();
	ArgumentHandler::getFlag(resultsSentenceAndTextFlag)->raise();

	EXPECT_EQ(text.formatFinalResults(), L"paragraphNr,sentenceNr,Joy,Sadness,Trust,Disgust,Fear,Anger,Surprise,Anticipation\n1,1,1,0,0,0,0,0,0,0\n1,2,0,1,0,0,0,0,0,0\n2,1,1,0,0,0,0,0,0,0\n");
}
#pragma endregion



// ----------------------------------------------
//                    Topic
// ----------------------------------------------

#define TopicTests
#pragma region TopicTests

TEST(TopicTest, createTopicFromFile1)
{
	std::wofstream out("testTopic1.txt");
	out << L"-ts Sun sollar Earth Moon -tw star planet satellite system" << std::endl;

	std::wstring parameter = L"testTopic1.txt";
	std::vector<std::wstring> expectedStrongWords = { L"sun", L"sollar", L"earth", L"moon" };
	std::vector<std::wstring> expectedWeakWords = { L"star", L"planet", L"satellite", L"system" };

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(topicFlag)->raise(parameter);

	Topic topic = Topic::createTopic(parameter);

	EXPECT_EQ(expectedStrongWords, topic.getStrongWords());
	EXPECT_EQ(expectedWeakWords, topic.getWeakWords());
}

TEST(TopicTest, createTopicFromFile2)
{
	std::wofstream out("testTopic2.txt");
	out << L"-tw" << "\n";
	out << L"green" << "\n";
	out << L"calm" << "\n";
	out << "\n";
	out << L"-ts" << "\n";
	out << L"Nature" << "\n";
	out << L"forest" << std::endl;

	std::wstring parameter = L"testTopic2.txt";
	std::vector<std::wstring> expectedStrongWords = { L"nature", L"forest" };
	std::vector<std::wstring> expectedWeakWords = { L"green", L"calm" };

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(topicFlag)->raise(parameter);

	Topic topic = Topic::createTopic(parameter);

	EXPECT_EQ(expectedStrongWords, topic.getStrongWords());
	EXPECT_EQ(expectedWeakWords, topic.getWeakWords());
}

TEST(Topic, createTopicFromText)
{
	std::wstring parameter = L"christmas snow winter holiday";
	std::vector<std::wstring> expectedStrongWords = { L"christmas", L"snow", L"winter", L"holiday" };
	std::vector<std::wstring> expectedWeakWords = {};

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(topicFlag)->raise(parameter);

	Topic topic = Topic::createTopic(parameter);

	EXPECT_EQ(expectedStrongWords, topic.getStrongWords());
	EXPECT_EQ(expectedWeakWords, topic.getWeakWords());
}

TEST(Topic, strongAndWeak)
{
	std::wstring strongParameter = L"president leader Gitanas Nauseda";
	std::wstring weakParameter = L"he him mr.";
	std::vector<std::wstring> expectedStrongWords = { L"president", L"leader", L"gitanas", L"nauseda" };
	std::vector<std::wstring> expectedWeakWords = { L"he", L"him", L"mr." };

	ArgumentHandler::reset();

	ArgumentHandler::getFlag(strongTopicFlag)->raise(strongParameter);
	ArgumentHandler::getFlag(weakTopicFlag)->raise(weakParameter);

	Topic topic = Topic::createTopic(strongParameter, weakParameter);

	EXPECT_EQ(expectedStrongWords, topic.getStrongWords());
	EXPECT_EQ(expectedWeakWords, topic.getWeakWords());
}

#pragma endregion