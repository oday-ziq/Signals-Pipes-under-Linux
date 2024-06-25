#include "header.h"
#include "constants.h"
#include "functions.h"

int energy_level, player_number, team_number, otherTeamLeaderId;

int balls_count_with_team = 1;

struct message message_to_write, message_to_read;

int shmid_var1, shmid_var2;
int *shared_var1, *shared_var2;

pid_t getOtherTeamLeaderID(int team_number){
    if(team_number == 1){
        return (*shared_var2);
    }
    return (*shared_var1);
}

void write_to_fifo(struct message msg, struct message msg_read){
    int f = open(msg.fifo_name, O_WRONLY);
    if(f == -1){
        perror("Couldn't Open Fifo for Writing");
        exit(-4);
    }
    
    if(write(f, msg.message_content, sizeof(msg.message_content)) == -1){
        perror("Couldn't Write to Fifo");
        exit(-4);
    }    

    energy_level = getMax(energy_level - 5, 5);
    close(f);

    if(player_number == 0){
        read_from_fifo(msg_read, msg);
    }
}

void read_from_fifo(struct message msg, struct message msg_write){
    int f;
    if((f = open(msg.fifo_name, O_RDONLY)) == -1){
        perror("Couldn't Open fifo");
        exit(-4);
    }
    if(read(f, msg.message_content, sizeof(msg.message_content)) == -1){
        perror("Couldn't Read From Fifo");
        exit(-4);
    }
    if(strcmp(msg.message_content, "") != 0){
        int sleep_duration = getRandomSleepForEnergyLevel(energy_level, getpid());
        fflush(stdout);
        sleep(sleep_duration);
        printf("Team %d, Ball Received by player %d, after %d seconds of throwing it %s\n", team_number, player_number, sleep_duration, msg.message_content);
        fflush(stdout);

        close(f);

        if(player_number == 0){
            printf("Hi other team leader id = %d and my id = %d\n", getOtherTeamLeaderID(team_number), getpid());
            balls_count_with_team--;
            kill(getOtherTeamLeaderID(team_number), SIGUSR1);
            if(balls_count_with_team == 0){
                // Drop new ball.
                balls_count_with_team++;
                write_to_fifo(msg_write, msg);
            }
        }
        else{
            write_to_fifo(msg_write, msg);
        }
    }

}

// Receive a new ball.
void signal_handler(int sig){
    printf("Hello there\n");
    balls_count_with_team++;
    printf("Balls with team = %d are currently = %d\n", team_number, balls_count_with_team);
    fflush(stdout);
    signal(SIGUSR1, signal_handler);
    write_to_fifo(message_to_write, message_to_read);
}

void alarm_handler(int signum) {
    printf("Program terminated after %d seconds.\n", GAME_LENGTH);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]){

    if(argc < 6){
        perror("Expected 6 argumernts");
        exit(-1);
    }
    
    // Change seed
    srand(getpid()); 

    // Set energy level of the player to a random number between 80 - 100.
    energy_level = 80 + rand() % 21;

    player_number = atoi(argv[1]);

    team_number = atoi(argv[4]);

    GAME_LENGTH = atoi(argv[5]);

    if(signal(SIGALRM, alarm_handler) == -1){
        perror("Setting up a signal");
        exit(-3);
    };

    alarm(GAME_LENGTH);

    strcpy(message_to_write.fifo_name, argv[2]);
    strcpy(message_to_write.message_content, "Sending ball to next player");
    strcpy(message_to_read.fifo_name, argv[3]);

    if(player_number == 0){
        // Create shared memory segment for variable 1
        shmid_var1 = shmget(TEAM_LEADER1_SHM_KEY, sizeof(int), IPC_CREAT | 0666);
        if (shmid_var1 == -1) {
            perror("shmget for variable 1");
            exit(1);
        }

        // Attach shared memory segment for variable 1
        shared_var1 = (int *)shmat(shmid_var1, NULL, 0);
        if (shared_var1 == (int *)(-1)) {
            perror("shmat for variable 1");
            exit(1);
        }

        // Create shared memory segment for variable 2
        shmid_var2 = shmget(TEAM_LEADER2_SHM_KEY, sizeof(int), IPC_CREAT | 0666);
        if (shmid_var2 == -1) {
            perror("shmget for variable 2");
            exit(1);
        }

        // Attach shared memory segment for variable 2
        shared_var2 = (int *)shmat(shmid_var2, NULL, 0);
        if (shared_var2 == (int *)(-1)) {
            perror("shmat for variable 2");
            exit(1);
        }

        if(team_number == 1){
            (*shared_var1) = getpid();
        }
        else{
            (*shared_var2) = getpid();
        }

        // Set up signal handler for SIGUSR1
        if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
            perror("Error setting up signal handler");
            exit(-3);
        }
        write_to_fifo(message_to_write, message_to_read);
    }
    else{
        while(1){
            read_from_fifo(message_to_read, message_to_write);
        }
    }

    // Detach shared memory segments
    if (shmdt(shared_var1) == -1) {
        perror("shmdt for variable 1");
        exit(1);
    }

    if (shmdt(shared_var2) == -1) {
        perror("shmdt for variable 2");
        exit(1);
    }
    return 0;
}