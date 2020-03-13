#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <random>

#define TOTAL_TEAMS 4
#define TOTAL_ROUNDS 2*(TOTAL_TEAMS - 1)

using namespace std;

vector<vector<int> > schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > initial_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<int> choices(2*TOTAL_TEAMS);
int choices_no = 1;

// Functions to generate initial schedule
vector<vector<int> > randomSchedule();
bool generateRandomSchedule();

// Neighborhood functions
void swapHomes(int home_team, int away_team);
void swapRounds(int week_src, int week_dest);
void swapTeamSchedule(int team_1, int team_2);
void partialSwapRounds(int team, int round_1, int round_2);

// Supporting functions
int elementExists(vector<int> weeks_sched, int element);
void printSchedule();

vector<vector<int> > randomSchedule()
{
    cout<<"Total teams are "<<TOTAL_TEAMS<<" with total rounds "<<TOTAL_ROUNDS<<endl;

    // This is Q from figure 3 which contains all the possible schedules
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

    // Creating choices which is a vector of all the home and away games
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

    // Unique seed for the random number generator
    // srand((unsigned)time(0));
    // Generating the random schedule (Figure 3)
    generateRandomSchedule();

    // Printing the generated schedule
    cout<<"Initial Schedule"<<endl;
    // for(int i=0; i<TOTAL_ROUNDS; i++)
    // {
    //     cout<<"Week "<<i+1<<": ";
    //     for(int j=0; j<TOTAL_TEAMS; j++)
    //     {
    //         cout<<schedule[i][j]<<",";
    //     }
    //     cout<<endl;
    // }

    return schedule;
}

void printSchedule()
{
    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        cout<<"Week "<<i+1<<": ";
        for(int j=0; j<TOTAL_TEAMS; j++)
        {
            cout<<schedule[i][j]<<",";
        }
        cout<<endl;
    }
}

// Returns the index of the element if it exists otherwise -1
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
    srand(time(NULL));
    // static int team_no = 0;
    static int week_no = 0;
    int opponent_index = -1;

    // if(initial_schedule.empty())
    //     return true;
    random_device random_dev;
    mt19937 generate(random_dev());

    while(!initial_schedule.empty())
    {
        // cout<<initial_schedule.size()<<" : "<<initial_schedule[0].size()<<endl;
        int current_team = initial_schedule[0][0];
        int current_opponent = initial_schedule[0][0];
        int random_index;
        
        while(current_opponent == current_team)
        {
            random_index = rand() % choices.size() - 1;

            opponent_index = elementExists(initial_schedule[0], abs(choices[random_index]));
            if(opponent_index != -1)
                current_opponent = abs(choices[random_index]);
            // cout<<"Current team "<<current_team<<" Current Opponent is "<<current_opponent<<endl;
        }
        
        current_opponent = choices[random_index];
        // cout<<"Week no: "<<week_no<<endl;
        // cout<<"Current team "<<current_team<<" Current Opponent is "<<current_opponent<<endl;

        // schedule[week_no][team_no] = current_opponent;
        schedule[week_no][current_team-1] = current_opponent;
        // cout<<"wrote to schedule----------\n";

        if(current_opponent > 0)
        {
            // schedule[week_no][opponent_index] = -1*current_team;
            schedule[week_no][abs(current_opponent)-1] = -1*current_team;
            // cout<<"-wrote to schedule opponent----------\n";
        }
        else
        {
            // schedule[week_no][opponent_index] = current_team;
            schedule[week_no][abs(current_opponent)-1] = current_team;
            // cout<<"wrote to schedule opponent----------\n";
        }
        
        // cout<<"Week no: "<<week_no+1<<endl;
        // cout<<"Current team "<<current_team<<" Current Opponent is "<<current_opponent<<endl;

        if(initial_schedule[0].size() <= 2)
        {
            initial_schedule.erase(initial_schedule.begin()+0);
            week_no++;
            // team_no = 0;
        }
        else
        {
            int curr_team_in_init = elementExists(initial_schedule[0], abs(current_team));
            int opp_team_in_init = elementExists(initial_schedule[0], abs(current_opponent));
            // cout<<initial_schedule[0].size()<<" : ";
            // cout<<"Starting erasing "<<abs(current_opponent)-1<<endl;
            // initial_schedule[0].erase(initial_schedule[0].begin() + abs(current_opponent)-1);
            initial_schedule[0].erase(initial_schedule[0].begin() + opp_team_in_init);
            // initial_schedule[0].erase(initial_schedule[0].begin() + team_no);
            // cout<<"Starting erasing "<<abs(current_team)-1<<endl;
            // initial_schedule[0].erase(initial_schedule[0].begin() + current_team-1);
            initial_schedule[0].erase(initial_schedule[0].begin() + curr_team_in_init);
            // cout<<"completed erasing\n";
            // team_no++;
        }

        // for(int i=0; i<TOTAL_ROUNDS; i++)
        // {
        //     cout<<"Week "<<i+1<<": ";
        //     for(int j=0; j<TOTAL_TEAMS; j++)
        //     {
        //         cout<<schedule[i][j]<<",";
        //     }
        //     cout<<endl<<endl;
        // }
    }


    // if(generateRandomSchedule())
    //     return true;

    return false;
}

void swapHomes(int home_team, int away_team)
{
    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        if(abs(schedule[i][home_team-1]) == away_team)
        {
            schedule[i][home_team-1] *= -1;
            schedule[i][away_team-1] *= -1;
        }
    }
}

void swapRounds(int src_week, int dest_week)
{
    if((src_week > TOTAL_ROUNDS) || (dest_week > TOTAL_ROUNDS))
        exit(0);
    else
        schedule[src_week-1].swap(schedule[dest_week-1]);
}

void swapTeamSchedule(int team_1, int team_2)
{
    int team_1_index = team_1 - 1;
    int team_2_index = team_2 - 1;
    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        if(abs(schedule[i][team_1_index]) != team_2)
        {
            int team_1_opponent = schedule[i][team_1_index];
            int team_2_opponent = schedule[i][team_2_index];

            schedule[i][team_1_index] = team_2_opponent;
            if(team_2_opponent > 0)
                schedule[i][abs(team_2_opponent)-1] = (-1)* team_1;
            else
                schedule[i][abs(team_2_opponent)-1] = team_1;
            
            schedule[i][team_2_index] = team_1_opponent;
            if(team_1_opponent > 0)
                schedule[i][abs(team_1_opponent)-1] = (-1)* team_2;
            else
                schedule[i][abs(team_1_opponent)-1] = team_2;
        }
    }
}

void partialSwapRounds(int team, int round_1, int round_2)
{
    int r1_index = round_1 - 1;
    int r2_index = round_2 - 1;
    for(int i=0; i<TOTAL_TEAMS; i++)
    {
        if(abs(schedule[r1_index][i]) != abs(schedule[r2_index][i]))
        {
            int temp_team = schedule[r1_index][i];
            schedule[r1_index][i] = schedule[r2_index][i];
            schedule[r2_index][i] = temp_team;
        }
    }
}

int main()
{
    // Generate the initial schedule
    randomSchedule();
    printSchedule();

    // // Swap Homes
    // swapHomes(1, 3);
    // cout<<"\nAfter Home swapping:\n";
    // printSchedule();

    // // Swap Rounds
    // swapRounds(2, 3);
    // cout<<"\nAfter Round swapping:\n";
    // printSchedule();

    // Swap Team Schedule
    // swapTeamSchedule(1, 4);
    // cout<<"\nAfter Team Schedule Swapping:\n";
    // printSchedule();

    // Partial Swap
    partialSwapRounds(2, 1, 4);
    cout<<"\nAfter Partial Swap Rounds:\n";
    printSchedule();

    return 0;
}