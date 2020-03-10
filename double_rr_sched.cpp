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
vector<int> choices(2*TOTAL_TEAMS);
int choices_no = 1;

vector<vector<int> > randomSchedule();
bool generateRandomSchedule();
int elementExists(vector<int> weeks_sched, int element);

vector<vector<int> > randomSchedule()
{
    cout<<"Total teams are "<<TOTAL_TEAMS<<" with total rounds "<<TOTAL_ROUNDS<<endl;

    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        for(int j=0; j<TOTAL_TEAMS; j++)
        {
            initial_schedule[i][j] = j+1;
        }
    }

    cout<<"Initial Set"<<endl;
    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        cout<<"Week "<<i+1<<": ";
        for(int j=0; j<TOTAL_TEAMS; j++)
        {
            cout<<initial_schedule[i][j]<<",";
        }
        cout<<endl;
    }

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


    generateRandomSchedule();

    cout<<"Final Schedule"<<endl;
    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        cout<<"Week "<<i+1<<": ";
        for(int j=0; j<TOTAL_TEAMS; j++)
        {
            cout<<schedule[i][j]<<",";
        }
        cout<<endl;
    }

    return schedule;
}

int elementExists(vector<int> weeks_sched, int element)
{
    // cout<<"Ele exists:\t";
    for(int i=0; i<weeks_sched.size(); i++)
    {
        // cout<<weeks_sched[i]<<",";
        if(weeks_sched[i] == element)
            return i;
    }
    // cout<<endl;

    return -1;
}

bool generateRandomSchedule()
{
    static int team_no = 0;
    static int week_no = 0;
    int opponent_index = -1;
    srand((unsigned)time(0));

    if(initial_schedule.empty())
        return true;
    
    int current_team = initial_schedule[week_no][team_no];
    int current_opponent = initial_schedule[week_no][team_no];
    int random_index;
    
    while(current_opponent == current_team)
    {
        random_index = rand() % choices.size() - 1;
        opponent_index = elementExists(initial_schedule[week_no], abs(choices[random_index]));
        cout<<"Opponent is "<<opponent_index<<endl;
        if(opponent_index != -1)
            current_opponent = abs(choices[random_index]);
        // cout<<"Current team "<<current_team<<" Current Opponent is "<<current_opponent<<endl;
    }

    current_opponent = choices[random_index];
    // cout<<"Current team "<<current_team<<" Current Opponent is "<<current_opponent<<endl;

    schedule[week_no][current_team-1] = current_opponent;
    if(current_opponent > 0)
        schedule[week_no][opponent_index] = -1*current_team;
    else
        schedule[week_no][opponent_index] = current_team;
    
    if(initial_schedule[week_no].size() <= 2)
    {
        cout<<"Week no: "<<week_no<<endl;
        initial_schedule.erase(initial_schedule.begin()+week_no);
        week_no++;
    }
    else
    {
        initial_schedule[0].erase(initial_schedule[0].begin() + opponent_index);
        initial_schedule[0].erase(initial_schedule[0].begin() + current_team-1);
    }

    if(generateRandomSchedule())
        return true;

    return false;
}

int main()
{
    randomSchedule();

    return 0;
}