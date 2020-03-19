#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <random>
#include <math.h>

#define TOTAL_TEAMS 4
#define TOTAL_ROUNDS 2*(TOTAL_TEAMS - 1)

using namespace std;


vector<vector<int> > schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > prev_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > initial_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > distance_matrix(TOTAL_TEAMS, vector<int>(TOTAL_TEAMS, 0));
vector<int> choices(2*TOTAL_TEAMS);
int choices_no = 1;

// Parameters
float weight = 4000;
float theta = 1.04;
float delta = 1.04;
float beta = 0.99;
float temperature = 400;
float best_temperature = 400;

// Functions to generate initial schedule
vector<vector<int> > randomSchedule();
bool generateRandomSchedule();

// Optimization function
void getOptimizedSchedule();

// Cost functions
int cost(bool updated_schedule);

// Objective function
float objectiveFunction();

// TTSA Algorithm
void ttsa();

// Neighborhood functions
void swapHomes(int home_team, int away_team);
void swapRounds(int week_src, int week_dest);
void swapTeamSchedule(int team_1, int team_2);
void partialSwapRounds(int team, int round_1, int round_2);
void partialSwapTeams(int team_1, int team_2, int round);

// Supporting functions
int elementExists(vector<int> weeks_sched, int element);
int opponenetExists(int current_team, int opponent);
void printSchedule();
int duplicateRowIndex(int current_team, int opponent_team, int current_round);
int numberOfViolations(bool updated_schedule);
void generateDistanceMatrix();

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

int opponenetExists(int current_team, int opponent)
{
    for(int i=0; i<TOTAL_ROUNDS;i++)
    {
        if(schedule[i][current_team-1] == current_team)
        {
            return -1;
        }
    }

    return 1;
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


int duplicateRowIndex(int current_team, int opponent_team, int current_round)
{
    int duplicate_index = 0;

    for(int i=0; i<TOTAL_ROUNDS; i++)
    {
        if(i != current_round)
        {
            if(schedule[i][abs(current_team)-1] == opponent_team)
            {
                cout<<"Inside the if: "<<i<<endl;
                return i;
            }    
        }
    }

    return -1;
}

void partialSwapTeams(int team_1, int team_2, int round)
{
    int t1_index = team_1 - 1;
    int t2_index = team_2 - 1;
    int round_index = round - 1;
    vector<int> swapped_rounds;
    swapped_rounds.push_back(round);

    // Must also check whether team_1 and team_2 are playing against each other on that round

    while(round_index >= 0)
    {
            int team_1_opponent = schedule[round_index][t1_index];
            int team_2_opponent = schedule[round_index][t2_index];

            schedule[round_index][t1_index] = team_2_opponent;
            if(team_2_opponent > 0)
                schedule[round_index][abs(team_2_opponent)-1] = (-1)* team_1;
            else
                schedule[round_index][abs(team_2_opponent)-1] = team_1;
            
            schedule[round_index][t2_index] = team_1_opponent;
            if(team_1_opponent > 0)
                schedule[round_index][abs(team_1_opponent)-1] = (-1)* team_2;
            else
                schedule[round_index][abs(team_1_opponent)-1] = team_2;

            // round_index = duplicateRowIndex(swapped_rounds, team_2_opponent); // Now it becomes opponent of team 1
            round_index = duplicateRowIndex(team_1, team_2_opponent, round_index);
            cout<<team_1<<" : "<<team_2_opponent<< " : "<<round_index<<endl;
    }
}

void generateDistanceMatrix()
{
    // for 4x4
    distance_matrix = { {0,745,665,929},
                        {745,0,80,337},
                        {665,80,0,380},
                        {929,337,380,0}
                      };
}

int cost(bool updated_schedule)
{
    int total_cost = 0;
    vector<vector<int> > curr_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));

    if(updated_schedule)
        curr_schedule = schedule;   // for loop deep copy
    else
        curr_schedule = prev_schedule;    

    for(int tm_index = 0; tm_index<TOTAL_TEAMS; tm_index++)
    {
        if(curr_schedule[0][tm_index] < 0)
            total_cost += distance_matrix[tm_index][abs(curr_schedule[0][tm_index])];
        
        for(int rnd_index = 1; rnd_index<TOTAL_ROUNDS; rnd_index++)
        {
            if(curr_schedule[rnd_index][tm_index] < 0)
            {
                if(curr_schedule[rnd_index-1][tm_index] < 0)
                {
                    total_cost += distance_matrix[abs(curr_schedule[rnd_index-1][tm_index])][abs(curr_schedule[rnd_index][tm_index])];
                }
                else
                {
                    total_cost += distance_matrix[tm_index][abs(curr_schedule[rnd_index][tm_index])];
                }
                // Add a case where prev is away and current home
            }
        }
    }

    return total_cost;
}

int numberOfViolations(bool updated_schedule)
{
    int total_violations = 0;
    int nr_violations = 0;
    int atmost_violations = 0;

    vector<vector<int> > curr_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));

    if(updated_schedule)
        curr_schedule = schedule;
    else
        curr_schedule = prev_schedule;

    for(int tm_index=0; tm_index<TOTAL_TEAMS; tm_index++)
    {
        int home_games = 0;
        int away_games = 0;
        for(int rnd_index=0; rnd_index<TOTAL_ROUNDS-3; rnd_index++)
        {
            if((curr_schedule[rnd_index][tm_index] > 0) && (curr_schedule[rnd_index+1][tm_index] > 0) && (curr_schedule[rnd_index+2][tm_index] > 0) && (curr_schedule[rnd_index+3][tm_index] > 0))
            {
                home_games += 4;
                away_games = 0;
            }

            if((curr_schedule[rnd_index][tm_index] < 0) && (curr_schedule[rnd_index+1][tm_index] < 0) && (curr_schedule[rnd_index+2][tm_index] < 0) && (curr_schedule[rnd_index+3][tm_index] < 0))
            {
                away_games += 4;
                home_games = 0;
            }

            if((home_games > 3) || (away_games > 3))
            {
                // cout<<"--inside atmost----\n";
                atmost_violations++;
                home_games = 0;
                away_games = 0;
            }
        }
    }

    for(int tm_index=0; tm_index<TOTAL_TEAMS; tm_index++)
    {
        for(int rnd_index=0; rnd_index<TOTAL_ROUNDS-1; rnd_index++)
        {
            if(abs(curr_schedule[rnd_index][tm_index]) == abs(curr_schedule[rnd_index+1][tm_index]))
            {
                nr_violations++;
            }
        }
    }

    cout<<"Atmost violations are "<<atmost_violations<<" Non repeat violations are "<<nr_violations<<endl;
    total_violations = nr_violations + atmost_violations;

    return total_violations;
}

// Sublinear function 
float f(int total_violations)
{
    return 1 + sqrt(total_violations)*log(total_violations/2);
}

float objectiveFunction(bool updated_schedule)
{
    return sqrt(pow(cost(updated_schedule), 2) + pow((weight*f(numberOfViolations(updated_schedule))), 2));
}

void selectRandomMove()
{
    int move = -1;
    int team_1 = -1;
    int team_2 = -1;
    int round_1 = -1;
    int round_2 = -1;

    srand(time(NULL));
    prev_schedule = schedule; // deep copy for loop

    team_1 = (rand() % TOTAL_TEAMS) + 1;

    while(true)
    {
        team_2 = (rand() % TOTAL_TEAMS) + 1;
        if(team_1 != team_2)
            break;
    }

    round_1 = (rand() % TOTAL_ROUNDS) + 1;

    while(true)
    {
        round_2 = (rand() % TOTAL_ROUNDS) + 1;
        if(round_1 != round_2)
            break;
    }

    move = (rand() % 4) + 1;

    switch (move)
    {
    case 1:
        // Swap Homes
        swapHomes(team_1, team_2);
        break;
    
    case 2:
        // Swap rounds
        swapRounds(round_1, round_2);
        break;
    case 3:
        // Swap Team Schedule
        swapTeamSchedule(team_1, team_2);
        break;
    case 4:
        partialSwapRounds(team_1, round_1, round_2);
        break;
    case 5:
        partialSwapTeams(team_1, team_2, round_1);
        break;
    default:
        break;
    }
}

void ttsa()
{
    int best_feasible = INT32_MAX;
    int number_of_feasible = INT32_MAX;
    int best_infeasible = INT32_MAX;
    int number_of_infeasible = INT32_MAX;
    int reheat = 0;
    int counter = 0;
    int max_r = 2;
    int max_c = 50;
    int max_p = 70;

    while(reheat <= max_r)
    {
        int phase = 0;
        while(phase <= max_p)
        {
            counter = 0;
            while(counter <= max_c)
            {
                bool updated_schedule = true;
                bool accept = false;
                selectRandomMove();

                if(objectiveFunction(updated_schedule) < objectiveFunction(!(updated_schedule)) || 
                    ((numberOfViolations(updated_schedule) == 0) && (objectiveFunction(updated_schedule) < best_feasible)) ||
                     ((numberOfViolations(updated_schedule) > 0) && (objectiveFunction(updated_schedule) < best_infeasible)))
                {
                    accept = true;
                }
                else
                {
                    float diff_in_cost = objectiveFunction(updated_schedule) - objectiveFunction(!(updated_schedule));
                    float prob = exp(-1*diff_in_cost/temperature);
                    if(prob > 0.5)
                        accept = true;
                    else
                        accept = false;
                }
                

                if(accept)
                {
                    prev_schedule = schedule;
                    if(numberOfViolations(updated_schedule) == 0)
                    {
                        number_of_feasible = min((int)objectiveFunction(updated_schedule), best_feasible);
                    }
                    else
                    {
                        number_of_infeasible = min((int)objectiveFunction(updated_schedule), best_feasible);
                    }

                    if((number_of_feasible < best_feasible) || (number_of_infeasible < best_infeasible))
                    {
                        reheat = 0;
                        counter = 0;
                        phase = 0;
                        best_temperature = temperature;
                        best_feasible = number_of_feasible;
                        best_infeasible = number_of_infeasible;
                        if(numberOfViolations(updated_schedule) == 0)
                        {
                            weight /= theta;
                        }
                        else
                        {
                            weight /= delta;
                        }
                    }
                }
                else
                {
                    counter++;
                }
            }
            phase++;
            temperature *= beta;
        }
        reheat++;
        temperature *= 2;
    }
}

int main()
{
    // Generate the initial schedule
    int t1, t2, week;

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
    // partialSwapRounds(2, 1, 4);
    // cout<<"\nAfter Partial Swap Rounds:\n";
    // printSchedule();

    // Partial Swap Teams 
    // cout<<"\nEnter team 1, team 2 and round: ";
    // cin>>t1>>t2>>week;
    // cout<<"Partial Swap Team Schedule:\n";
    // partialSwapTeams(t1, t2, week);
    // printSchedule();

    // Total Violations
    // int violations = numberOfViolations(true);
    // cout<<"Total violations are: "<<violations<<endl;

    // // Cost function
    // generateDistanceMatrix();
    // int distance = cost(true);
    // cout<<"Cost of the schedule is "<<distance;

    // ttsa();

    return 0;
}