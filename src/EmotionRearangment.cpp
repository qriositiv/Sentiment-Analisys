#include <fstream>
#include <iostream>

using namespace std;

int emotionRearangment()
{
    ifstream input;
    ofstream output;
    input.open("keywords.txt");
    output.open("keywordsNew.txt");

    string keyword;
    float emotions[6];
    float emotionalWeight;

    while(!input.eof())
    {
        input >> keyword;
        output << keyword << " ";

        for (int i = 0; i < 6; i++)
        {
            input >> emotions[i];
        }

        for (int i = 0; i < 8; i++)
        {
            switch(i)
            {
            case 0: // joy
                output << emotions[0] << " ";
                break;
            case 1: // sadness
                output << emotions[1] << " ";
                break;
            case 2: // trust
                cout << keyword << " Trust is ";
                cin >> emotionalWeight;
                output<< emotionalWeight << " ";
                break;
            case 3: // disgust
                output << emotions[5] << " ";
                break;
            case 4: // fear
                output << emotions[3] << " ";
                break;
            case 5: // anger
                output << emotions[2] << " ";
                break;
            case 6: // surprise
                output << emotions[4] << " ";
                break;
            case 7: // anticipation
                cout << keyword << " Anticipation is ";
                cin >> emotionalWeight;
                output<< emotionalWeight << " ";
                break;
            }
        }

        output << endl;
    }

    input.close();
    output.close();

    return 0;
}

// happiness, sadness, anger, fear, surprise, disgust
// joy, sadness, trust, disgust, fear, anger, surprise, anticipation
