#include "header.h"
#include "constants.h"
#include "functions.h"

int current_balls_count = 1, team_number;

void create_fifo(char fifo_name[]){
    char *fifo = fifo_name;
    remove(fifo_name);
    if(mknod(fifo_name, S_IFIFO | 0666, 0) == -1){
        perror("FAILED To create a FIFO");
        exit(-4);
    }
}

char* generate_write_fifo_pipe_name(pid_t team_id, int player_number){
    char tmp_fifo[100] = "/tmp/PRIVATE_";
    char team_id_string[20];
    sprintf(team_id_string, "%d", team_id);


    char write_from[3];
    sprintf(write_from, "%d", player_number);

    char write_to[3];
    sprintf(write_to, "%d", (player_number + 1)%NUMBER_OF_PLAYERS_PER_TEAM);

    strcat(tmp_fifo, team_id_string);
    strcat(tmp_fifo, "_");
    strcat(tmp_fifo, write_from);
    strcat(tmp_fifo, "_");
    strcat(tmp_fifo, write_to);

    char* fifo_name = tmp_fifo;
    return fifo_name;
}

int main(int argc, char* argv[]){

    if(argc < 4){
        perror("Expected 4 argumernts");
        exit(-1);
    }

    int parent_team_id = getpid();

    NUMBER_OF_PLAYERS_PER_TEAM = atoi(argv[1]);
    pid_t players[NUMBER_OF_PLAYERS_PER_TEAM];

    team_number = atoi(argv[2]);

    GAME_LENGTH = atoi(argv[3]);

    for(int i = 0; i < NUMBER_OF_PLAYERS_PER_TEAM; i++){
        players[i] = fork();
        if(players[i] == -1){
            perror("Player Creation Failed");
            exit(-1);
        }
        else if(players[i] == 0){
            char player_number_string[3];
            sprintf(player_number_string, "%d", i);

            char write_to_fifo[100];
            strcpy(write_to_fifo, generate_write_fifo_pipe_name(parent_team_id, i));
            create_fifo(write_to_fifo);
            char read_from_fifo[100];
            strcpy(read_from_fifo, generate_write_fifo_pipe_name(parent_team_id, (i - 1 + NUMBER_OF_PLAYERS_PER_TEAM) % NUMBER_OF_PLAYERS_PER_TEAM));
            
            char team_number_string[3];
            sprintf(team_number_string, "%d", team_number);

            char GAME_LENGTH_STRING[6];
            sprintf(GAME_LENGTH_STRING, "%d", GAME_LENGTH);

            sleep(1);
            execlp("./player", "player", player_number_string, write_to_fifo, read_from_fifo, team_number_string, GAME_LENGTH_STRING, NULL);
            perror("Player Exec Error");
            exit(-5);
        }
    }
    return 0;
}