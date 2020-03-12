#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <random>

using namespace std;

int main()
{
    int TOTAL_TEAMS = 4;
    srand((unsigned)time(0));
    vector<int> choices(2*TOTAL_TEAMS);
    int choices_no = 1;

    random_device random_dev;
    mt19937 generate(random_dev());

    for(int i=0; i<(2*TOTAL_TEAMS); i++)
    {
        choices[i] = choices_no;
        choices[++i] = -choices_no;
        choices_no++;
    }

    cout<<"Choices"<<endl;
    for(int i=0; i<(2*TOTAL_TEAMS); i++)
    {
        cout<<choices[i]<<",";
    }
    cout<<endl;

    // for(int i=0; i<TOTAL_TEAMS; i++)
    // {
    //     int random_index = rand() % choices.size();
    //     cout<<"Random number is :"<<choices[random_index]<<endl;
    // }

    for(int i=0; i<5; i++)
    {
        shuffle(choices.begin(), choices.end(), generate);
        cout<<"Choices"<<endl;
        // for(int i=0; i<(2*TOTAL_TEAMS); i++)
        for(int i=0; i<choices.size(); i++)
        {
            cout<<choices[i]<<",";
        }
        cout<<endl;
    }
}