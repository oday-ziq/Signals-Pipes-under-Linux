
#ifndef LIBRARIES
#define LIBRARIES

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <pthread.h>
#include<sys/shm.h>
#include <errno.h>

#define B_SIZ (PIPE_BUF / 2)

struct message {
  char    fifo_name[B_SIZ];
  char    message_content[B_SIZ];
};


#endif