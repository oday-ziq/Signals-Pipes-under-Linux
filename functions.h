


#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "header.h"
#include "constants.h"


void readFile(char* filename){
    char line[200];
    char label[50];

    FILE *file;

    file = fopen(filename, "r");

    if (file == NULL){
        perror("The file not exist\n");
        exit(-2);
    }

    char separator[] = "=";

    while(fgets(line, sizeof(line), file) != NULL){

        char *str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);

        if (strcmp(label, "NUMBER_OF_TEAMS") == 0){
            NUMBER_OF_TEAMS = atoi(str);
        } else if (strcmp(label, "NUMBER_OF_PLAYERS_PER_TEAM") == 0){
            NUMBER_OF_PLAYERS_PER_TEAM = atoi(str);
        } else if(strcmp(label, "GAME_LENGTH") == 0){
            GAME_LENGTH = atoi(str);
        }

    }

    fclose(file);
}

// Returns a random sleep dependant to energy_level, the bigger the energy level the less time the ball takes time to arrive to the next player.
int getRandomSleepForEnergyLevel(int energy_level, pid_t process_id){
    srand(process_id);
    return 1 + rand() % (300 / energy_level);
}

int getMax(int a, int b){
    if(a > b){
        return a;
    }
    return b;
}


#endif