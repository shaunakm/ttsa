#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <math.h>
#include <ctime>
#include <algorithm>

#define TOTAL_TEAMS 4
#define TOTAL_ROUNDS 2*(TOTAL_TEAMS - 1)

using namespace std;

vector<vector<int> > schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > initial_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));

int main()
{
    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        for(int j=0; j<TOTAL_TEAMS; j++)
        {
            schedule[i][j] = j+i;
            // schedule[i].erase(schedule[i].begin() + TOTAL_TEAMS - j -1);
            // cout<<schedule[i].size()<<",";
        }
    }

    cout<<schedule.size()<<" : "<<schedule[0].size()<<" Before erase: ";
    for(int i=0; i<schedule[0].size(); i++)
        cout<<schedule[0][i]<<",";

    schedule[0].erase(schedule[0].begin() + 0);
    schedule[0].erase(schedule[0].begin() + 2);
    cout<<"\n"<<schedule.size()<<" : "<<schedule[0].size()<<" : "<<schedule[1].size()<<" After erasing one element: ";
    for(int i=0; i<schedule[0].size(); i++)
        cout<<schedule[0][i]<<",";

    schedule.erase(schedule.begin() + 0);
    cout<<"\n"<<schedule.size()<<" : "<<schedule[0].size()<<" After erasing a row: ";
    for(int i=0; i<schedule[0].size(); i++)
        cout<<schedule[0][i]<<",";
    cout<<endl;

    return 0;
}