#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <random>
#include <math.h>

#define TOTAL_TEAMS 16
#define TOTAL_ROUNDS 2*(TOTAL_TEAMS - 1)

using namespace std;

vector<vector<int> > schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > prev_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > initial_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > distance_matrix(TOTAL_TEAMS, vector<int>(TOTAL_TEAMS, 0));
int choices_no = 1;

vector<int> rounds(TOTAL_ROUNDS, -1);
vector<int> teams(TOTAL_TEAMS, -1);
vector<int> choices(2*TOTAL_TEAMS, -1);
vector<int> possible_choices;

// Functions to generate initial schedule
vector<vector<int> > randomSchedule();
bool generateRandomSchedule();

// Supporting functions
int elementExists(vector<int> weeks_sched, int element);
int opponenetExists(int current_team, int opponent);
void printSchedule();
int duplicateRowIndex(int current_team, int opponent_team, int current_round);
int numberOfViolations(bool updated_schedule);
void generateDistanceMatrix();

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

void validateSchedule()
{
    for(int i=0; i<TOTAL_TEAMS; i++)
    {
        int init_sum = 0;
        for(int j=0; j<TOTAL_ROUNDS; j++)
        {
            init_sum += schedule[j][i];
        }

        cout<<"Team "<<i+1<<" sum is "<<init_sum<<endl;
    }
}

void initialize()
{
    // cout<<"Initializing__________________________\n";
    // Initialize rounds
    for(int i=0; i< rounds.size(); i++)
        rounds[i] = i+1;
    
    // Initialize teams
    for(int i=0; i<teams.size(); i++)
        teams[i] = i+1;
    
    // Initialize choices
    int current_team = 1;
    for(int i=0; i<choices.size(); i++)
    {
        choices[i] = current_team;
        choices[++i] = -current_team;
        current_team++;
    }
    
    for(int i=0; i<TOTAL_ROUNDS; i++)
        for(int j=0; j<TOTAL_TEAMS; j++)
            schedule[i][j] = 0;
    
    if(possible_choices.size() > 0)
        possible_choices.erase(possible_choices.begin(), possible_choices.end());
}

vector<vector<int> > randomSchedule()
{

    initialize();
    // Printing the initial set
    cout<<"Weeks set: ";
    for(int i=0; i< rounds.size(); i++)
        cout<<rounds[i]<<",";
    cout<<endl;
    
    cout<<"Teams set: ";
    for(int i=0; i<teams.size(); i++)
        cout<<teams[i]<<",";
    cout<<endl;
    
    cout<<"Choices set: ";
    for(int i=0; i< choices.size(); i++)
        cout<<choices[i]<<",";
    cout<<endl;

    cout<<"Initial Set: \n";
    printSchedule();

    // cout<<"Last element is "<<rounds.back();

    while(!generateRandomSchedule())
    {
        initialize();
    }

    cout<<"Initial Schedule: \n";
    printSchedule();

    validateSchedule();

    return schedule;
    
}

bool opponenetExistsRound(int op_team, int round)
{
    int rnd_index = round - 1;

    for(int i=0; i<TOTAL_TEAMS; i++)
    {
        if(abs(schedule[rnd_index][i]) == op_team)
            return true;
    }
    return false;
}

bool opponenetExistsTeam(int curr_team, int op_team, int round)
{
    int curr_tm_index = curr_team - 1;
    int op_tm_index = abs(op_team) - 1;
    int rnd_index = round - 1;

    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        if(schedule[i][curr_tm_index] == op_team)
            return true;
    }

    return false;
}

bool updatePossibleChoices(int team, int round)
{
    // cout<<"Updating the possible choices for "<<team<<" : "<<round<<"\n";
    random_device random_dev;
    mt19937 generate(random_dev());

    shuffle(choices.begin(), choices.end(), generate);

    for(int i=0; i<choices.size(); i++)
    {
        // Check if opponent exist and team exists
        if((abs(choices[i]) != team)
            && (!opponenetExistsRound(abs(choices[i]), round))
            && (!opponenetExistsTeam(team, choices[i], round)))
        {
            possible_choices.push_back(choices[i]);
        }
    }

    if(possible_choices.size() > 0)
        return true;

    return false;
}

bool generateRandomSchedule()
{
    srand(time(NULL));
    int current_round = rounds[0];
    int current_team = teams[0];
    // vector<int> possible_choices;
    
    // cout<<"Inside generateRandomSchedule()\n";
    while(current_team <= teams.back())
    {
        // cout<<current_round<<" : "<<current_team<<endl;
        // cout<<"Inside teams loop\n";
        int tm_index = current_team-1;
        while(current_round <= rounds.back())
        {
            // cout<<"Inside rounds loop\n";
            int rnd_index = current_round - 1;
            if(schedule[rnd_index][tm_index] == 0)
            {
                // printSchedule();
                if(!updatePossibleChoices(current_team, current_round))
                {
                    return false;
                }
                
                // cout<<"choices for week "<<current_round<<" "<<" and team "<<current_team<<" : ";
                // for(int i=0; i<possible_choices.size(); i++)
                //     cout<<possible_choices[i]<<",";
                // cout<<endl;

                int random_index = rand() % possible_choices.size();

                int current_opponent = possible_choices[random_index];
                int op_index = abs(current_opponent)-1;

                schedule[rnd_index][tm_index] = current_opponent;

                if(current_opponent > 0)
                {
                    schedule[rnd_index][op_index] = -1*current_team;
                }
                else
                {
                    schedule[rnd_index][op_index] = current_team;
                }
            }

            if(possible_choices.size() > 0)
                possible_choices.erase(possible_choices.begin(), possible_choices.end());

            current_round++;
        }
        current_round = rounds[0];
        current_team++;
    }

    return true;
}

int main()
{

    // Generating initial set    
    randomSchedule();
    // getPaperSchedule();
    // printSchedule();

    return 0;
}