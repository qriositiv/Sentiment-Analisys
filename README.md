# Sentiment analysis of web information
An application that aims to analyse texts using adapted Lexicon-based sentiment analysis models ([VADER](https://ojs.aaai.org/index.php/ICWSM/article/view/14550/14399), [BoW](https://en.wikipedia.org/wiki/Bag-of-words_model) ...) and classify texts emotions into eight main emotions and their combinations using [Plutchik's wheel of emotions](https://en.wikipedia.org/wiki/Emotion_classification#/media/File:Plutchik_Dyads.svg). In addition the application can scrape text from websites

# The Team - eMotionLess
- Anupras
- Arsenij
- Domas

# Simple overview
## Using the app
The application can be run with arguments. Arguments include specifying source and its type with *-t/-f/u- [source]* flags. Output is directed to terminal by default, but *-o [path]* can be used to specify output file instead. To specify database file *-d [path]* flag can be used. For more flags use *-h* flag

If no arguments were specified the application will ask for input once it is run and try to detect its type on its own. If default database is not detected, it will also ask for the path to it.

## Basic implementation
For handling inputs a virtual `InputManager` class which can detect the type of input and assign appropriate derived classes. 
For scraping websites derived `WebInput` class uses [**cURL**](https://curl.se/) (**libcurl** in library form) to download websites HTML which is later scraped by searching for elements like \<p> and \<h1>.

For analysis the text is separated into paragraphs -> sentences -> words. Then `Analyser` object goes through every word and searches for it in the database and if found, its emotional score is added to total (BoW model). For every analysis model a different class derived from `Analyser` will be used.

The database currently is a .txt file that can be easily modified, containing a word and 6 numbers representing weight for each emotion.

# Setting up IDE
We use [Visual Studio 2022](https://visualstudio.microsoft.com/vs/). The edition (Community/Enterprise/Professional) does not matter.

To setup Visual Studio for work with this project you need to:
- Install Visual Studio 2022.
- If Visual Studio 2022 is already installed, open Visual Studio Installer and choose "Modify".
- When installing or modifying choose "Desktop development with C++" workload
- In Individual components segment select:
- - "C++ MFC for latest v143 build tools (x84 & x64)"
- - "C++ CMake tools for Windows"
- - "Test Adapter for Google Test"

To clone the repository in VS:
- Run Visual Studio 2022.
- Choose **Clone a repository**.
- Make sure destination folder is named **"SentimentAnalysis"** (due to problems with *.gitignore*).

For more information see [wiki](https://git.mif.vu.lt/emotionless/sentiment-analysis/-/wikis/Using-Visual-Studio-and-MSBuild).


# Building the project
## Windows
In Windows project can be build through either the IDE or command line using MSBuild or CMake.

### IDE
To build with MSBuild - if solution is not open select the `SentimentAnalysis.sln` in *folder view* or *Solution explorer*. In the toolbar (at the top) select **build > build solution** or run the project with green arrows.

To build with CMake - in *Solution explorer* select *Switch between solution view and ...* and then **CMake Targets View**. Then choose `CMakeLists.txt` and at the top select **-> Current Document(CMakeLists.txt)**.

For more information see [wiki](https://git.mif.vu.lt/emotionless/sentiment-analysis/-/wikis/Using-Visual-Studio-and-MSBuild).

### Command line
- MSBuild: run `msbuild pathto/src/SentimentAnalysis.sln /p:OutputPath=c:\buildDestination`
- CMake: run `cmake -S pathtoCMakeLists.txt -B buildDestination` then run `cmake --build buildDestination`

*Note: Cmake will automatically use MSVC for compiling. See [wiki](https://git.mif.vu.lt/emotionless/sentiment-analysis/-/wikis/Building-and-Testing-with-CMake) for more information.

## Linux
Linux does not support MSBuild.

To build the project you must install these dependencies:
- make (on some platforms cmake is enough)
- cmake
- g++ or other compiler
- libssl-dev
- libcurl4-openssl-dev
- ca-certificates if openssl is not installed

Then build the project with
- `cmake -S pathtoCMakeLists.txt -B buildDestination`
- `cmake --build buildDestination`

If build fails due to compiler missing, run the first build command with `-DCMAKE_CXX_COMPILER=/usr/bin/g++` flag

For more information see [wiki](https://git.mif.vu.lt/emotionless/sentiment-analysis/-/wikis/Building-and-Testing-with-CMake).

# Running Tests
## IDE
Open *Test explorer* and select *Run all tests in view*.
For more information see [wiki](https://git.mif.vu.lt/emotionless/sentiment-analysis/-/wikis/Using-Visual-Studio-and-MSBuild).

## Command line
Build the project with either CMake or MSBuild. Run SaTests executable.
For more information see [wiki](https://git.mif.vu.lt/emotionless/sentiment-analysis/-/wikis/Building-and-Testing-with-CMake).