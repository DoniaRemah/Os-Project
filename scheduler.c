#include "DataStructure.c"

// argument list (./scheduler.out , scheduling algo number , number of process , quantam if exist)

int main(int argc, char *argv[])
{
    printf("Schedular Started.\n");
    initClk(); // initizaling clock

    int sch = atoi(argv[1]); // scheduling algorithm number
    int no_processes = atoi(argv[2]); // Number of processes to expect

    // Creating & initializing Message queue for processes generator communication
    key_t key_id;
    int msgq_id, send_approve;
    struct msgbuff message_buffer;
    message_buffer.mtype = no_processes;

    key_id = ftok("sendProcess", 65);           // create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); // create message queue and return id

    // Creating ready queue to store arrived processes.
    struct Queue* ready_queue = createQueue();

    // while there is still processes to complete
    // will decrement each time a process finishes.
    while (no_processes > 0)
    {
        // Check if a process has arrived, if not, continue
        int rec_value = msgrcv(msgq_id, &message_buffer, sizeof(message_buffer.msg_process),
         0, IPC_NOWAIT);

        while (rec_value != -1)
        {
            printf("process %d recieved successfully at time %d\n", message_buffer.msg_process.id, getClk());

            struct Node* arrived_process = newNode(message_buffer.msg_process.id,
            message_buffer.msg_process.priority);
            /////////////////////////////////// SJF /////////////////////////////////////
            if(atoi(argv[1]) == 1)
            {   
                //TODO: Set sorting_priority according to running time
                arrived_process->sorting_priority = arrived_process->node_process.runtime;
                // Adding to queue where sorting occures according to the specified priority
                enQueue(ready_queue,arrived_process);
            }
            /////////////////////////////////// HPF /////////////////////////////////////
            else if(atoi(argv[1]) == 2)
            {
                //TODO: Set sorting_priority according to Priority
                arrived_process->sorting_priority = arrived_process->pID;
                // Adding to queue where sorting occures according to the specified priority
                enQueue(ready_queue,arrived_process);
            }
            /////////////////////////////////// RR /////////////////////////////////////
            else if (atoi(argv[1]) == 3)
            {
                //TODO: Set sorting_priority according to arrival time
                arrived_process->sorting_priority = arrived_process->node_process.arrival_time;
                // Adding to queue where sorting occures according to the specified priority
                enQueue(ready_queue,arrived_process);
            }
            /////////////////////////////////// MLFL /////////////////////////////////////
            else if (atoi(argv[1]) == 4)
            {
                //TODO: Set sorting_priority according to Priority
                arrived_process->sorting_priority = arrived_process->pID;
                // Adding to queue where sorting occures according to the specified priority
                enQueue(ready_queue,arrived_process);

            }
        }
        



    }
    


    destroyClk(true);
}
