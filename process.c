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
    {
        //there is also the count and quantum
        //for quantum algorithms
        run_count=atoi(argv[1]);
        quantum=atoi(argv[2]);
        int nowclk;
        // printf("\nQuantum in process %d is %d\n", getpid(), quantum); 
        
        while (remainingtime > 0 )
        {
            // Ex: getclk = 1, nowclk = 1
            nowclk = getClk(); // getting time at begining of each loop

            // pausing till clock moves 1 second.
            while (nowclk == getClk()){} 

            remainingtime--;
            run_count++;
            printf("\nFor process %d -> Run count is:%d, Remaining time is: %d. \n",getpid(),run_count,remainingtime);
            if(remainingtime==0)
            {
                printf("\nProcess with id: %d is terminating at time %d. Remaning Time is: %d \n",getpid(),getClk(),remainingtime);
                kill(getppid(),SIGUSR1); 
                destroyClk(false);
                return 0;
            }
            if(run_count==quantum)
            //send a signal to the process to stop
            {
                printf("\nProcess with id %d finished its quantum -> Sending signal to the scheduler\n",getpid());
                run_count=0;
                if(remainingtime!=0)
                {
                    kill(getppid(),SIGUSR2); 
                }
                else
                {
                    kill(getppid(),SIGUSR1); 
                }
                
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
        printf("\nProcess with id: %d is terminating at time %d. Remaning Time is: %d \n",getpid(),getClk(),remainingtime);
        // if remaning time =0, send to schedular process is finished.
        kill(getppid(),SIGUSR1); 
    }

    
    destroyClk(false);



    return 0;
}
