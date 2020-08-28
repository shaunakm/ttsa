#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <random>
#include <math.h>
#include <chrono>
#include <iostream>
#include <fstream>

#define TOTAL_TEAMS 8
#define TOTAL_ROUNDS 2*(TOTAL_TEAMS - 1)

using namespace std;
using namespace std::chrono;

vector<vector<int> > schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > prev_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > initial_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));
vector<vector<int> > distance_matrix(TOTAL_TEAMS, vector<int>(TOTAL_TEAMS, 0));
vector<int> choices(2*TOTAL_TEAMS);
vector<int> rounds(TOTAL_ROUNDS, -1);
vector<int> teams(TOTAL_TEAMS, -1);
vector<int> possible_choices;

int choices_no = 1;

// Parameters
int max_r = 5;
int max_c = 3000;
int max_p = 4000;

float weight = 4000;
float theta = 1.05;
float delta = 1.05;
float const_beta = 0.9999;
float temperature = 400;
float best_temperature = 400;
int best_feasible = INT32_MAX;
int number_of_feasible = INT32_MAX;
int best_infeasible = INT32_MAX;
int number_of_infeasible = INT32_MAX;

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
void validateSchedule();
void initialize();
bool opponenetExistsRound(int op_team, int round);
bool opponenetExistsTeam(int curr_team, int op_team, int round);
bool updatePossibleChoices(int team, int round);

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

    while(!generateRandomSchedule())
    {
        initialize();
    }

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

    while(current_team <= teams.back())
    {
        int tm_index = current_team-1;
        while(current_round <= rounds.back())
        {
            int rnd_index = current_round - 1;
            if(schedule[rnd_index][tm_index] == 0)
            {
                if(!updatePossibleChoices(current_team, current_round))
                {
                    return false;
                }

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

void printSchedule()
{
    ofstream sched_data_file;
    string filename = "T" + to_string(TOTAL_TEAMS) + "_schedule_data.txt";
    sched_data_file.open(filename, ios::app);
    cout<<"T\\R:\n";
    sched_data_file<<"T\\R:\n";
    for(int i=0; i<TOTAL_TEAMS; i++)
    {
        cout<<i+1<<" \t";
        sched_data_file<<i+1<<" \t";
        for(int j=0; j<TOTAL_ROUNDS; j++)
        {
            if(schedule[j][i] > 0)
            {
                cout<<" "<<schedule[j][i]<<"\t";
                sched_data_file<<" "<<schedule[j][i]<<"\t";
            }
            else
            {
                cout<<schedule[j][i]<<"\t";
                sched_data_file<<schedule[j][i]<<"\t";
            }
        }
        cout<<endl;
        sched_data_file<<"\n";
    }

    sched_data_file.close();

    // for(int i=0; i<TOTAL_ROUNDS; i++)
    // {
    //     cout<<"Week "<<i+1<<": ";
    //     for(int j=0; j<TOTAL_TEAMS; j++)
    //     {
    //         cout<<schedule[i][j]<<",";
    //     }
    //     cout<<endl;
    // }
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

void getPaperSchedule()
{
    schedule = {{6, 5, -4, 3, -2, -1},
                {-2, 1, 5, 6, -3, -4},
                {4, -3, 2, -1, 6, -5},
                {3, -6, -1, -5, 4, 2},
                {-5, 4, 6, -2, 1, -6},
                {-4, 3, -2, 1, -6, 5},
                {-3, 6, 1, 5, -4, -2},
                {5, -4, -6, 2, -1, 3},
                {2, -1, -5, -6, 3, 4},
                {-6, -5, 4, -3, 2, 1}
                };
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
    // Distance matrix for 12 teams
    distance_matrix  = {  {0   ,745  ,665  ,929  ,605  ,521  ,370  ,587  ,467  ,670 , 700 ,1210 },
                        {745 ,  0  , 80  ,337  ,1090 , 315 , 567 , 712 , 871 , 741, 1420, 1630},
                        {665 , 80  ,  0  ,380  ,1020 , 257 , 501 , 664 , 808 , 697, 1340, 1570},
                        {929 ,337  ,380  ,  0  ,1380 , 408 , 622 , 646 , 878 , 732, 1520, 1530},
                        {605 ,1090 ,1020 ,1380 ,   0 ,1010 , 957 ,1190 ,1060 ,1270,  966, 1720},
                        {521 , 315 , 257 , 408 , 1010,   0 , 253 , 410 , 557 , 451, 1140, 1320},
                        {370 , 567 , 501 , 622 ,  957,  253,   0 , 250 , 311 , 325,  897, 1090},
                        {587 , 712 , 664 , 646 , 1190,  410,  250,   0 , 260 ,  86,  939,  916},
                        {467 , 871 , 808 , 878 , 1060,  557,  311,  260,   0 , 328,  679,  794},
                        {670 , 741 , 697 , 732 ,1270 , 451 , 325 ,  86 ,328  ,  0 , 1005,  905},
                        {700 ,1420 ,1340 ,1520 , 966 ,1140 , 897 , 939 , 679 ,1005,   0 , 878 },
                        {1210, 1630, 1570, 1530, 1720, 1320, 1090,  916,  794, 905,  878,   0 },
                      };
}

int cost(bool updated_schedule)
{
    int total_cost = 0;
    vector<vector<int> > curr_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));

    // curr_schedule = {{2, -1},
    //                  {-2, 1}};
    if(updated_schedule)
    {
        // curr_schedule = schedule;   // for loop deep copy

        for(int i=0; i<TOTAL_ROUNDS; i++)
            for(int j=0; j<TOTAL_TEAMS; j++)
                curr_schedule[i][j] = schedule[i][j];
    }
    else
    {
        // curr_schedule = prev_schedule;    

        for(int i=0; i<TOTAL_ROUNDS; i++)
            for(int j=0; j<TOTAL_TEAMS; j++)
                curr_schedule[i][j] = prev_schedule[i][j];
    }

    for(int tm_index = 0; tm_index<TOTAL_TEAMS; tm_index++)
    {
        if(curr_schedule[0][tm_index] < 0)
        {
            total_cost += distance_matrix[tm_index][abs(curr_schedule[0][tm_index])-1];
            // total_cost = distance_matrix[0][tm_index];
            // cout<<"team "<<tm_index+1<<" travelling to "<<curr_schedule[0][tm_index]<<" distance is "<<distance_matrix[tm_index][abs(curr_schedule[0][tm_index])-1]<<endl;
        }
        
        for(int rnd_index = 1; rnd_index<TOTAL_ROUNDS; rnd_index++)
        {
            // cout<<"team "<<tm_index+1<<" travelling to "<<curr_schedule[rnd_index][tm_index]<<" distance is "<<distance_matrix[tm_index][abs(curr_schedule[rnd_index][tm_index])-1]<<endl;
            // cout<<"Inside distance for"<<endl;
            if(curr_schedule[rnd_index][tm_index] < 0)
            {
                if(curr_schedule[rnd_index-1][tm_index] < 0)
                {
                    total_cost += distance_matrix[abs(curr_schedule[rnd_index-1][tm_index])-1][abs(curr_schedule[rnd_index][tm_index])-1];
                }
                else
                {
                    total_cost += distance_matrix[tm_index][abs(curr_schedule[rnd_index][tm_index])-1];
                }
            }
            else
            {
                if(curr_schedule[rnd_index-1][tm_index] < 0)
                {
                    total_cost += distance_matrix[tm_index][abs(curr_schedule[rnd_index][tm_index])-1];
                }
            }
        }
    }

    // cout<<"cost is "<<total_cost<<endl;

    return total_cost;
}

int numberOfViolations(bool updated_schedule)
{
    int total_violations = 0;
    int nr_violations = 0;
    int atmost_violations = 0;

    vector<vector<int> > curr_schedule(TOTAL_ROUNDS, vector<int> (TOTAL_TEAMS, 0));

    if(updated_schedule)
    {
        // curr_schedule = schedule;   // for loop deep copy

        for(int i=0; i<TOTAL_ROUNDS; i++)
            for(int j=0; j<TOTAL_TEAMS; j++)
                curr_schedule[i][j] = schedule[i][j];
    }
    else
    {
        // curr_schedule = prev_schedule;    

        for(int i=0; i<TOTAL_ROUNDS; i++)
            for(int j=0; j<TOTAL_TEAMS; j++)
                curr_schedule[i][j] = prev_schedule[i][j];
    }

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

    // cout<<"Atmost violations are "<<atmost_violations<<" Non repeat violations are "<<nr_violations<<endl;
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
    if(numberOfViolations(updated_schedule) == 0)
        return cost(updated_schedule);
    else
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
    // prev_schedule = schedule; // deep copy for loop
    for(int i=0;i<TOTAL_ROUNDS; i++)
        for(int j=0; j<TOTAL_TEAMS; j++)
            prev_schedule[i][j] = schedule[i][j];


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
    int reheat = 0;
    int counter = 0;

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
                    float prob = 1000*exp(-1*diff_in_cost/temperature);
                    cout<<"Difference is "<<diff_in_cost<<" and probability is "<<prob<<" temperature is "<<temperature<<endl;
                    if(prob > 100)
                        accept = true;
                    else
                        accept = false;
                }
                

                if(accept)
                {
                    // prev_schedule = schedule;
                    for(int i=0; i<TOTAL_ROUNDS; i++)
                        for(int j=0; j<TOTAL_TEAMS; j++)
                        prev_schedule[i][j] = schedule[i][j];
                        
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
                    else
                    {
                        counter++;
                    }
                }
            }
            phase++;
            temperature = temperature * const_beta;
        }
        reheat++;
        temperature = 2*best_temperature;
    }
}

int main()
{
    ofstream sched_file;
    string filename = "T" + to_string(TOTAL_TEAMS) + "_schedule_data.txt";
    sched_file.open(filename, ios::app);
    cout<<"Initial Schedule: \n";
    sched_file<<"\n----------\nInitial Schedule: \n";
    sched_file.close();
    auto start_time = high_resolution_clock::now();
    randomSchedule();
    auto initial_time = high_resolution_clock::now();
    printSchedule();

    // Total Violations
    sched_file.open(filename, ios::app);
    int violations = numberOfViolations(true);
    cout<<"Total violations of initial schedule are "<<violations<<endl;
    sched_file<<"Total violations of initial schedule are "<<violations<<endl;
    // // Cost function
    generateDistanceMatrix();
    int distance = cost(true);
    cout<<"Cost of the initial schedule is "<<distance<<endl;
    sched_file<<"Cost of the initial schedule is "<<distance<<endl;
    sched_file.close();
    // TTSA
    sched_file.open(filename, ios::app);
    cout<<"\nOptimized schedule "<<endl;
    sched_file<<"\nParameters are maxR: "<<max_r<<" maxC: "<<max_c<<" maxP: "<<max_p<<" T0: "<<temperature<<endl;
    sched_file<<"Optimized Schedule: \n";
    sched_file.close();
    ttsa();
    auto stop_time = high_resolution_clock::now();
    printSchedule();
    // Total Violations
    violations = numberOfViolations(true);
    sched_file.open(filename, ios::app);
    cout<<"Total violations of optimized schedule are "<<violations<<endl;
    sched_file<<"Total violations of optimized schedule are "<<violations<<endl;

    distance = cost(true);
    cout<<"Cost of the optimized schedule is "<<distance<<endl;
    sched_file<<"Cost of the initial schedule is "<<distance<<endl;

    auto initial_duration = duration_cast<milliseconds>(initial_time - start_time);
    auto ttsa_duration = duration_cast<milliseconds>(stop_time - initial_time);

    cout<<"Time to generate initial schedule "<<initial_duration.count()<<" and optimized schedule "<<ttsa_duration.count()<<endl;
    sched_file<<"Time to generate initial schedule "<<initial_duration.count()<<" ms"<<" and optimized schedule "<<ttsa_duration.count()<<" ms"<<endl;
    sched_file<<"----------\n";

    sched_file.close();

    return 0;
}