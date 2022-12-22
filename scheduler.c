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

    destroyClk(true);
}
