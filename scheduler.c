#include "DataStructure.c"

// argument list (./scheduler.out , scheduling algo number , number of process , quantam if exist)
//the list given to the scheduler by its parent (the process generator)

int main(int argc, char *argv[])
{
    initClk();

    int sch = atoi(argv[1]);
    int no_processes = atoi(argv[2]);
    

    key_t key_id;
    int msgq_id, send_approve;
    struct msgbuff message_buffer;
    message_buffer.mtype = no_processes;

    key_id = ftok("sendProcess", 65);           // create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); // create message queue and return id
    //the scheduler now has access to the message queue

    if (atoi(argv[1])==2) //the HPF algorithm, PREEMPTIVE highest priority first 
    {
        //signal here that the process terminated
        int number_of_processes=no_processes; //reading number of processes from argument list
        
        

        //initializing a ready queue
        struct Queue* HPF_ready_queue=createQueue();

        struct Node* Running_process=NULL; //the process currently running (initialized to null)


        //running all the processes
        while(number_of_processes>0)
        {
            //receiving the processes in the message buffer sent by the process generator
            struct msgbuff message_rec;
            int rec_check=msgrcv(msgq_id, &message_rec, sizeof(message_rec.msg_process), 0, IPC_NOWAIT); //no wait
            while(rec_check != -1)
            {
                //while there is no error
                //put the message in a node process structure
                //put the node in the ready queue
                //note: the info I have about the process
                //1- priority
                //2- id
                //3- arrival time
                //4- run time
                printf("\n At scheduler: Received process %d at time %d \n", message_rec.msg_process.id, getClk());

                //create a new node
                struct Node* created_node=newNode(message_rec.msg_process.id,message_rec.msg_process.priority);
                

                //putting this node into the queue
                enQueue(HPF_ready_queue,created_node);

                //receive the next process from the message queue
                rec_check=msgrcv(msgq_id, &message_rec, sizeof(message_rec.msg_process), 0, IPC_NOWAIT); //no wait

            }

            bool running=false; //is there a process running rn or not

            //case that there is no process running, take the process in the ready queue and run it
            if(Running_process==NULL && isEmpty(HPF_ready_queue)==false)
            {
                //the running process is the one in front of the queue
                Running_process=popQueue(HPF_ready_queue);
                Running_process->node_process.start_time=getClk();
                running=true;
            }

            //case that the received (arrived) process
            //has higher priority than the one currently running
            //preemption
            if(Running_process!=NULL)
            {
                //priority of the currently running process
                int running_process_priority=Running_process->node_process.priority;
                //priority of the process received (in the ready queue)
                int waiting_process_priority=peekQueue(HPF_ready_queue)->node_process.priority;
                if(running==true && isEmpty(HPF_ready_queue)==false && running_process_priority>waiting_process_priority)
                {
                    //the waiting process has the priority to run
                    //preempt the running process
                    int kill_running_process=kill(Running_process->pID,SIGSTOP);

                    //calculate the remaining time for the killed process
                    Running_process->node_process.remaining_time=Running_process->node_process.runtime-(getClk()-Running_process->node_process.start_time);

                    //store the time this process got stopped at
                    Running_process->node_process.stopped_time=getClk();
                    
                    //put the old process in the ready queue
                    enQueue(HPF_ready_queue,Running_process);

                    //remove the higher priority process from the ready queue to run
                    Running_process=popQueue(HPF_ready_queue);
                    Running_process->node_process.start_time=getClk();
                    running=true;

                }
            }

            if (running==true)
            {
                if( Running_process->pID==-1)//this process doesn't have an id, it has not been forked before
                //a new process that just started running for the first time
                {
                    //the scheduler forks the process
                    int pid=fork();
                    if (pid==0) //the fork is successful
                    {
                        //store the pid of the forked process
                        Running_process->pID=getpid();
                        //saving the remaining time of the running process
                        //to send to the process file

                        char* remaining_time_char=malloc(sizeof(char));
                        sprintf(remaining_time_char,"%d",Running_process->node_process.remaining_time);
                        char * arg[]={remaining_time_char,NULL};

                        //sending the remaining time info to the process file
                        execv("./process.out",arg);
                    }

                    //pause the scheduler to ensure synchronization 
                    raise(SIGSTOP);
                    //calculate the waiting time of the process
                    Running_process->node_process.wait_time=(getClk()-Running_process->node_process.start_time)-(Running_process->node_process.runtime-Running_process->node_process.remaining_time);

                    
                }
                else
                {
                    //the process has an id, it has been forked before
                    //this is not its first time to run
                    //continue the killed process
                    kill(Running_process->pID,SIGCONT);
                    Running_process->node_process.start_time=getClk();
                }
                
            }
            






        }

    }

    destroyClk(true);
}
