#include "header.h"
#include "constants.h"
#include "functions.h"

int main(){
    pid_t main_parent_application_id = getpid();
    printf("Parent ID: %d\n", main_parent_application_id);

    readFile("args.txt");
    pid_t teams[NUMBER_OF_TEAMS];

    for(int i = 0; i < NUMBER_OF_TEAMS; i++){
        teams[i] = fork();
        if(teams[i] == -1){
            perror('Team Creation Failed');
            exit(-1);
        }
        else if(teams[i] == 0){
            char team_number_string[3];
            sprintf(team_number_string, "%d", i + 1);

            char NUMBER_OF_PLAYERS_PER_TEAM_STRING[5];
            sprintf(NUMBER_OF_PLAYERS_PER_TEAM_STRING, "%d", NUMBER_OF_PLAYERS_PER_TEAM);

            char GAME_LENGTH_STRING[5];
            sprintf(GAME_LENGTH_STRING, "%d", GAME_LENGTH);

            execlp("./team", "team", NUMBER_OF_PLAYERS_PER_TEAM_STRING, team_number_string, GAME_LENGTH_STRING, NULL);
            perror("Team Exec Error\n");
            exit(-5);
        }
    }
    return 0;
}