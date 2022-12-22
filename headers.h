#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
// #include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "math.h"
#include <signal.h>
#include <string.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

// needed structs

enum process_status
{
    RUNNING, // running for the first time
    WAITING, // process hasn't been run before (just arrived)
    STOPPED, // Process is paused
    CONTINUE, // process is running after being stopped.
    FINSIHED // process is terminated 
};

struct process
{
    int id;
    int priority;
    int arrival_time;
    int runtime;
    int remaining_time;
    int start_time;
    int finish_time;
    int wait_time;
    int stopped_time;
    int TA;
    int WTA;
};

struct msgbuff
{
    int mtype;
    struct process msg_process;
};

struct pcb
{
    struct process PCBprocess;
    int remTime;
    int pid;
};