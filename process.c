#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int run_count;
int quantum;
int runTime;
int nowclk;

int main(int agrc, char *argv[])
{
    initClk();
    remainingtime = atoi(argv[0]);
    runTime = remainingtime;

    if (agrc != 3)
    {
        nowclk = getClk();
        while (remainingtime > 0 )
             remainingtime = runTime - (getClk() - nowclk);
    
        // if remaning time =0, send to schedular process is finished.
        kill(getppid(),SIGUSR1); 
    }

    if (agrc==3)
    {
        // to ensure synchronization with scheduler
        kill(getppid(),SIGCONT);
        //there is also the count and quantum
        //for quantum algorithms
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

    
    destroyClk(false);
    return 0;
}
