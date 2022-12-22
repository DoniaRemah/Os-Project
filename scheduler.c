#include "DataStructure.c"

// argument list (./scheduler.out , scheduling algo number , number of process , quantam if exist)

int no_processes;

int main(int argc, char *argv[])
{

    printf("Schedular Started.\n");
    initClk(); // initizaling clock

    int sch = atoi(argv[1]); // scheduling algorithm number
    no_processes = atoi(argv[2]); // Number of processes to expect

    struct Node* running_process;

    // Creating & initializing Message queue for processes generator communication
    key_t key_id;
    int msgq_id, send_approve;

    key_id = ftok("sendProcess", 65);           // create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); // create message queue and return id

    // Creating ready queue to store arrived processes.
    struct Queue* ready_queue = createQueue();

    /////////////// THINGS TO COMPUTE ////////////////////////
    //CPU Utilization,waiting time,start time, finish time, stop time, TA, WTA
    //////////////////////////////////////////////////////

    // while there is still processes to complete
    // will decrement each time a process finishes.
    while (no_processes > 0)
    {
        // Check if a process has arrived, if not, continue
        struct msgbuff message_buffer;

        //printf("Number of process: %d\n",no_processes);

        int rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),0, !IPC_NOWAIT);
        
        //printf("rec_value: %d \n at time %d",rec_value, getClk());

        if(rec_value != -1)
        {
            printf("process  %d recieved successfully at time %d\n", message_buffer.msg_process.id, getClk());
        }

        while (rec_value != -1)
        {
            // send process to newNode
            struct Node* arrived_process = newNode(message_buffer.msg_process);

            /////////////////////////////////// SJF /////////////////////////////////////
            if(sch == 1)
            {   
                //TODO: Set sorting_priority according to running time
                arrived_process->sorting_priority = arrived_process->node_process.runtime;
                // Adding to queue where sorting occures according to the specified priority
                enQueue(ready_queue,arrived_process);
            }
            /////////////////////////////////// HPF /////////////////////////////////////
            else if(sch == 2)
            {
                //TODO: Set sorting_priority according to Priority
                
                printf("In Algorithm 2\n");
                // arrived_process->sorting_priority = arrived_process->node_process.priority;
                // // Adding to queue where sorting occures according to the specified priority
                // enQueue(ready_queue,arrived_process);

                rec_value=-1;
            }
            /////////////////////////////////// RR /////////////////////////////////////
            else if (sch == 3)
            {
                //TODO: Set sorting_priority according to arrival time
                arrived_process->sorting_priority = arrived_process->node_process.arrival_time;
                // Adding to queue where sorting occures according to the specified priority
                enQueue(ready_queue,arrived_process);
            }
            /////////////////////////////////// MLFL /////////////////////////////////////
            else if (sch == 4)
            {
                
                //TODO: Set sorting_priority according to Priority
                arrived_process->sorting_priority = arrived_process->node_process.priority;
                // Adding to queue where sorting occures according to the specified priority
                enQueue(ready_queue,arrived_process);

                
                // 1. use popqueue to remove the process from readyqueue and get pointer to it.
                // 2. after it finishes its quantum, Check for arrived processes then 
                // update its sorting_priority and re-enqueue it to get its actual place

            }
        }
        



    }
    


    destroyClk(true);
}
