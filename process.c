#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int run_count;
int quantum;

int main(int agrc, char *argv[])
{

    // printf("Before initialize clock in process. Time is %d \n",getClk());
    initClk();

    // to ensure synchronization with scheduler
    kill(getppid(),SIGCONT);

    remainingtime = atoi(argv[0]);
    printf("Process with id: %d is started at time %d. Remaining Time is: %d \n",getpid(),getClk(),remainingtime);
    if (agrc==3)
    //there is also the count and quantum
    //for quantum algorithms
    {
        run_count=atoi(argv[1]);
        quantum=atoi(argv[2]);
        int nowclk;

        while (remainingtime > 0 )
        {
            // Ex: getclk = 1, nowclk = 1
            nowclk = getClk(); // getting time at begining of each loop

            // pausing till clock moves 1 second.
            while (nowclk == getClk()){} 

            remainingtime--;
            run_count++;
            if(run_count==quantum)
            //send a signal to the process to stop
            {
                printf("\n Sending signal to the scheduler, process with id %d finished its quantum.",getpid());
                kill(getppid(),SIGUSR2); 

            }



        }


    }
    
    else
    {
        int nowclk;

    //TODO The process needs to get the remaining time from schedular from args.
        while (remainingtime > 0 )
        {
            // Ex: getclk = 1, nowclk = 1
            nowclk = getClk(); // getting time at begining of each loop

            // pausing till clock moves 1 second.
            while (nowclk == getClk()){} 

            remainingtime--;


        }
        printf("Process with id: %d is terminating at time %d. Remaning Time is: %d \n",getpid(),getClk(),remainingtime);
        // if remaning time =0, send to schedular process is finished.
        kill(getppid(),SIGUSR1); 
        destroyClk(false);

    }

    
    




    return 0;
}
