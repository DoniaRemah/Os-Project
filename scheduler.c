#include "DataStructure.c"




// argument list (./scheduler.out , scheduling algo number , number of process , quantam if exist)

//Forward declaration of signal handler
void ProcessTerminated(int signum);
void Process_finished_quantum(int signum);


int no_processes;
struct Node* Running_process;
bool check_running=false; //is there a process running rn or not
struct Queue* ready_queue;
int quantum;
int run_count;


int main(int argc, char *argv[])
{

    printf("Schedular Started.\n");
    initClk(); // initizaling clock

    int sch = atoi(argv[1]); // scheduling algorithm number
    no_processes = atoi(argv[2]); // Number of processes to expect
    if(argc==4)
    { 
        quantum=atoi(argv[3]);

    }

    // Creating & initializing Message queue for processes generator communication
    key_t key_id;
    int msgq_id, send_approve;

    key_id = ftok("sendProcess", 65);           // create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); // create message queue and return id

    // Creating ready queue to store arrived processes.
    ready_queue = createQueue();

    // indicating start time of a process each time it runs.
    // for pre-emptive algorithms, to calculate remaining time.
    int round_start_time =0; 

    /////////////// THINGS TO COMPUTE ////////////////////////
    //CPU Utilization,waiting time,start time, finish time, stop time, TA, WTA
    //////////////////////////////////////////////////////
    struct msgbuff message_buffer;
    int rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, !IPC_NOWAIT);
    // while there is still processes to complete
    // will decrement each time a process finishes.
    while (no_processes > 0)
    {
        signal(SIGUSR1, ProcessTerminated);
        signal(SIGUSR2,Process_finished_quantum);

        // Check if a process has arrived, if not, continue
        // struct msgbuff message_buffer;

        //printf("Number of process: %d\n",no_processes);
        
        //printf("rec_value: %d \n at time %d",rec_value, getClk());

            /////////////////////////////////// SJF /////////////////////////////////////
            if(sch == 1)
            {   
                while (rec_value != -1)
                {
                    // send process to newNode
                    struct Node* arrived_process = newNode(message_buffer.msg_process);
                    //TODO: Set sorting_priority according to running time
                    arrived_process->sorting_priority = arrived_process->node_process.runtime;
                    arrived_process->node_process.remaining_time = arrived_process->node_process.runtime;
                    // Adding to queue where sorting occures according to the specified priority
                    enQueue(ready_queue,arrived_process);
                    arrived_process->status=WAITING; 
                    rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, IPC_NOWAIT);
                }
            }
            /////////////////////////////////// HPF /////////////////////////////////////
            else if(sch == 2)
            {

                
                //TODO: Set sorting_priority according to Priority
                
                // printf("Start of Algorithm 2\n");
                while (rec_value != -1)
                {
                    printf("process  %d recieved successfully at time %d\n", message_buffer.msg_process.id, getClk());
                    // send process to newNode
                    struct Node* arrived_process = newNode(message_buffer.msg_process);
                    
                    //TODO: Set sorting_priority according to the process priority
                    arrived_process->sorting_priority = arrived_process->node_process.priority;
                    arrived_process->node_process.remaining_time = arrived_process->node_process.runtime;
                    // Adding to queue where sorting occures according to the specified priority
                    enQueue(ready_queue,arrived_process);
                    
                    arrived_process->status = WAITING;
                    printf("Checking if another process has arrived.\n");
                    rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, IPC_NOWAIT);
                }
                        

                //case that there is no process running, 
                //take the process in the ready queue and run it
                if(Running_process==NULL && isEmpty(ready_queue)==false)
                {
                    printf("Case1: No process is running. \n");
                    //the running process is the one in front of the queue
                    Running_process=popQueue(ready_queue);
                    

                    // If this is the first run for this process, set start time.
                    if(Running_process->status == WAITING)
                    {
                        printf("setting status to running of process %d \n",Running_process->node_process.id);
                        Running_process->status = RUNNING;
                        Running_process->node_process.start_time=getClk();
                    }else // if this is a previously stopped process
                    {
                        printf("Recontinuing.\n");
                        Running_process->status = CONTINUE;
                    }
                    
                    round_start_time = getClk();
                    check_running=true;
                }

                // I have a running process and recieved A new process
                //case that the received (arrived) process
                //has higher priority than the one currently running
                //preemption
                if(Running_process!=NULL && isEmpty(ready_queue)==false)
                {
                    
                    //priority of the currently running process
                    int running_process_priority=Running_process->node_process.priority;
                    //priority of the process received (in the ready queue)

                    int waiting_process_priority=peekQueue(ready_queue)->node_process.priority;

                    if(running_process_priority > waiting_process_priority)
                    {
                        printf("Case2: There is a  process running & Recieved a new one. \n");
                        
                        //the waiting process has the priority to run
                        //preempt the running process
                        printf("Process to be stopped %d\n",Running_process->pID);
                        int kill_running_process=kill(Running_process->pID,SIGSTOP);

                        printf("After stopping the running process. \n");
                        Running_process->status = STOPPED;
                        //calculate the remaining time for the killed process
                        // Remaning time = Remaining time - (clk - round_start_time)
                        Running_process->node_process.remaining_time=Running_process->node_process.remaining_time-(getClk()-round_start_time);

                        //store the time this process got stopped at
                        Running_process->node_process.stopped_time=getClk();
                        
                        //put the old process in the ready queue
                        enQueue(ready_queue,Running_process);

                        //remove the higher priority process from the ready queue to run
                        Running_process=popQueue(ready_queue);
                        Running_process->status = RUNNING;
                        Running_process->node_process.start_time=getClk();
                        round_start_time= getClk();
                        check_running=true;

                    }
                }

                // FORKING 
                if (check_running==true)
                {                
                    //this process doesn't have an id, it has not been forked before
                    if(Running_process->pID==-1) 
                    //a new process that just started running for the first time
                    {
                        printf("Starting forking process of process %d for the first time. \n",Running_process->node_process.id);
                        //the scheduler forks the process
                        int pid=fork();
                        if (pid==0) //the fork is successful (I am the child)
                        {
                            //store the pid of the forked process
                            printf("Pid of Processs %d is %d\n", Running_process->node_process.id,getpid());
                            
                            //saving the remaining time of the running process
                            //to send to the process file

                            char* remaining_time_char=malloc(sizeof(char));
                            sprintf(remaining_time_char,"%d",Running_process->node_process.remaining_time);
                            char * arg[]={remaining_time_char,NULL};
                            //printf("The clock before starting is: %d\n",getClk());
                            //sending the remaining time info to the process file
                            execv("./process.out",arg);
                        }else if ( pid != -1)
                        {   
                            raise(SIGSTOP);
                            Running_process->pID=pid;                            
                        }
                                                                            
                        //pause the scheduler to ensure synchronization                         
                        //calculate the waiting time of the process
                        //Running_process->node_process.wait_time=(getClk()-Running_process->node_process.start_time)-(Running_process->node_process.runtime-Running_process->node_process.remaining_time);                        
                    }
                    else
                    {
                        //the process has an id, it has been forked before
                        //this is not its first time to run
                        //continue the killed process
                        kill(Running_process->pID,SIGCONT);
                        // Running_process->status = CONTINUE;
                        round_start_time=getClk();
                    }
                    
                    // Check for recieved processes.
                    //printf("Checking for recieved messages after forking.\n");
                }
            
            rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, IPC_NOWAIT);

            }
            /////////////////////////////////// RR /////////////////////////////////////
            else if (sch == 3)
            {
                
                
                // printf("\nRound Robin algorithm started!");
                // printf("\n---------------------------------");
                while (rec_value != -1)
                {
                    
                    //create a new node with the process received
                    struct Node* arrived_process = newNode(message_buffer.msg_process);
                    printf("\nAt scheduler: RECEIVED process %d, at time %d", arrived_process->node_process.id,getClk());
                    //TODO: Set sorting_priority according to arrival time
                    arrived_process->sorting_priority = arrived_process->node_process.arrival_time;
                    arrived_process->node_process.remaining_time = arrived_process->node_process.runtime;
                    // Adding to queue where sorting occures according to the specified priority
                    //the processes are arranged in the ready queue sorted ascendingly according to their arrival time
                    enQueue_at_back(ready_queue,arrived_process);
                    printf("\n//////////////////////////////////////////////\n");
                    printQueue(ready_queue);
                    printf("//////////////////////////////////////////////\n");
                    arrived_process->status = WAITING;  //process is waiting to run for the first time
                    rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, IPC_NOWAIT);
                }

                //Round Robin logic


                //case that there is no process running
                //and the ready queue is not empty
                //take the process in front of the queue and run it
                if(Running_process==NULL && isEmpty(ready_queue)==false)
                {
                    printf("\nCase 1: No process is currently running" );
                    //take the process in front of the queue
                    Running_process=popQueue(ready_queue);
                    run_count=0;

                    //if the remaining time of the process
                    //or its running time is less than the quantum

                    if (Running_process->node_process.remaining_time<quantum)
                    {
                        quantum=Running_process->node_process.remaining_time;
                    }
                    
                    //1- this is the first time this process has run
                    //set its start time
                    if (Running_process->status==WAITING)
                    {
                        printf("\nRunning process %d for the first time.",Running_process->node_process.id);
                        //set the status to running
                        Running_process->status=RUNNING;
                        Running_process->node_process.start_time=getClk();
                    }
                    else 
                    //this process has run before
                    //recontinuing the process
                    {
                        printf("\nprocess %d has run before, RECONTINUING.",Running_process->node_process.id);
                        //set the status to continue
                        Running_process->status = CONTINUE;
                        

                    }
                    //calculating the round start time
                    round_start_time=getClk();
                    //set running to true
                    check_running = true;
                    //restarting the run count
                    run_count=0;

                    //forking
                
                    //case that this process has not been forked before
                    //this process is running for the first time
                    if (Running_process->pID==-1)
                    {
                        printf("\nFORKING of process %d",Running_process->node_process.id);
                        int pid=fork();
                        if(pid==0)
                        //forking is successful
                        //I am the child
                        {
                            //send info to process file
                            //send:
                            //remaining time (new)
                            //the run count
                            //the quantum
                            char *runtime_char = malloc(sizeof(char));
                            char* count_char = malloc(sizeof(char));
                            char* quantum_char = malloc(sizeof(char));
                            sprintf(runtime_char, "%d", Running_process->node_process.remaining_time);
                            sprintf(count_char, "%d", run_count);
                            sprintf(count_char, "%d", quantum);
                            char *arg[] = {runtime_char, count_char,quantum_char,NULL};
                            
                            //sending info to the process file
                            execv("./process.out",arg);
                        }
                        else if (pid!=-1)
                        //this is the parent  
                        //the scheduler
                        //the pid of the process is sent to the parent
                        {
                            //pause the scheduler to ensure synchronization
                            raise(SIGSTOP);
                            Running_process->pID=pid;
                        }
                        


                    }
                    else
                    {
                        //the process has been forked before
                        //it is recontinued
                        kill(Running_process->pID,SIGCONT);
                        //update the round start time
                        round_start_time=getClk();
                        run_count=0;
                        //status
                        Running_process->status=CONTINUE;
                        

                    }


                    
                    if(isEmpty(ready_queue)==false)
                    {


                        //the process stopped at time:
                        Running_process->node_process.stopped_time=getClk();

                        //enqueue the process back in the ready queue
                        enQueue_at_back(ready_queue,Running_process);

                        //get another process from the queue to run
                        Running_process=popQueue(ready_queue);
                        Running_process->status=RUNNING;
                        Running_process->node_process.start_time=getClk();
                        round_start_time=getClk();
                        check_running=true;

                    }
                    else
                    //the running process is the only one in the queue
                    // give it a quantum and run it again

                    {
                        run_count=0;
                        quantum=atoi(argv[3]);
                        if (Running_process->node_process.remaining_time<quantum)
                        {
                            quantum=Running_process->node_process.remaining_time;

                        }


                    }
                    



                
                }



                //the very last thing, I check for receiving again
                rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, IPC_NOWAIT);
            }
            /////////////////////////////////// MLFL /////////////////////////////////////
            else if (sch == 4)
            {
                while (rec_value != -1)
                {
                    // send process to newNode
                    struct Node* arrived_process = newNode(message_buffer.msg_process);
                    //TODO: Set sorting_priority according to running time
                    arrived_process->sorting_priority = arrived_process->node_process.priority;
                    arrived_process->node_process.remaining_time = arrived_process->node_process.runtime;
                    // Adding to queue where sorting occures according to the specified priority
                    enQueue(ready_queue,arrived_process);
                    arrived_process->status=WAITING; 
                    rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, IPC_NOWAIT);
                }

                // 1. use popqueue to remove the process from readyqueue and get pointer to it.
                // 2. after it finishes its quantum, Check for arrived processes then 
                // update its sorting_priority and re-enqueue it to get its actual place

            }
        
        



    }
    


    destroyClk(true);
}

void ProcessTerminated(int signum)
{   
    printf("In handler of termination \n");
    free(Running_process);
    Running_process = NULL;
    no_processes--;
    check_running = false;
    printf("////////////////////////////////////\n");
    printQueue(ready_queue);
    printf("////////////////////////////////////\n");
    printf("leaving handler of termination \n");
}

void Process_finished_quantum(int signum)
{
    printf("In the handler of the finished quantum!");
    //stop the running process
    printf("\nProcess %d finished its quantum, its remaining time is %d",Running_process->node_process.id,Running_process->node_process.remaining_time);
    kill(Running_process->pID,SIGSTOP);
    Running_process->status=STOPPED;
    Running_process->node_process.remaining_time-=quantum;

    
}