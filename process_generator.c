#include "DataStructure.c"

void clearResources(int);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources); // for clearing all resources at the end of simulation
    printf("%s\n", argv[0]);
    // TODO Initialization
    
    // 2. Read the chosen scheduling algorithm and its parameters, 

    // check for aruments
    // arguments between % is optional depending on algorithm

    // argc should >=4 (.out file ,filename.txt , -sch , scheduling algorithm number, % -q, quantum number %)
    if (argc < 4)
    {
        printf("Too few arguments. Exiting!\n");
        exit(1);
    }

    int q; // quantam number

    if ((argc == 6) && (atoi(argv[3]) == 3 || atoi(argv[3]) == 4)) // RR or MLFL
    {
        q = atoi(argv[5]); // set quantum number
    }
    //If algorithm needs a quantum and one wasn't specified.
    else if ((argc < 6) && (atoi(argv[3]) == 3 || atoi(argv[3]) == 4))
    {
        printf("The scheduling algorithm needs a quantum. Exiting!\n"); // don't ask for it
        exit(1);
    }

    // 1. Read the input files.
    // 5. Create a data structure for processes and provide it with its parameters.

    FILE *file = fopen(argv[1], "r"); // file ptr

    // * read and ignore, [^\n] -> any char except newline
    fscanf(file, "%*[^\n]"); // skip first line

    int no_processes = 0;
    int file_num;

    // Getting number of processes.
    while (fscanf(file, "%d", &file_num) != EOF)
    {
        no_processes++;
    }

    // dividing by 4 since I have 4 colns.
    no_processes /= 4;

    // resetting file pointer
    fseek(file, 0, SEEK_SET);
    fscanf(file, "%*[^\n]"); // skip first line

    struct process process_array[no_processes];
    int i = 0;

    // Filling process array with input data
    while (fscanf(file, "%d", &file_num) != EOF)
    {
        process_array[i].id = file_num;
        fscanf(file, "%d", &file_num);
        process_array[i].arrival_time = file_num;
        fscanf(file, "%d", &file_num);
        process_array[i].runtime = file_num;
        fscanf(file, "%d", &file_num);
        process_array[i].priority = file_num;
        i++;
    }

    fclose(file);

    // for (i = 0; i < no_processes; i++)
    // {
    //     printf("id %d arrival %d run %d p %d\n", process_array[i].id, 
    //     process_array[i].arrival_time, process_array[i].runtime, process_array[i].priority);
    // }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    char *sch = argv[3];

    // 3. Initiate and create the scheduler and clock processes.

    // Forking Clock
    pid_t pid;
    pid = fork();

    if (pid == -1)
    {

        // pid == -1 means error occurred
        printf("error in forking of clock occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // The first argument is is the filename of the file that 
        // contains the executable image of the new process.
        // Second argument is array of character pointers
        // First member is filename
        // ,last member must be null 
        // ,The rest is list of arguments available to new process
        char *argv_list[] = {"./clk.out", NULL};

        execv("./clk.out", argv_list);
        exit(0);
    }

    // Forking Scheduler
    pid = fork();
    if (pid == -1)
    {

        // pid == -1 means error occurred
        printf("error in forking of scheduler occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // to send number of processes as an argument to the forked scheduler.
        char temp_nProcess[10];
        sprintf(temp_nProcess, "%d", no_processes);

        // argument list (./scheduler.out , scheduling algo number , number of process , quantam if exist)

        // If RR or MLFL
        if ((argc == 6) && (atoi(argv[3]) == 3 || atoi(argv[3]) == 4))
        {

            char *argv_list[] = {"./scheduler.out", sch, temp_nProcess, argv[5], NULL};
            execv("./scheduler.out", argv_list);
            // printf("arguments sent successfully\n");
        }
        else // If SJF, HPF
        {
            char *argv_list[] = {"./scheduler.out", sch, temp_nProcess, NULL};
            execv("./scheduler.out", argv_list);
            // printf("arguments sent successfully\n");
        }

        exit(0);
    }

    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    // To get time use this function.
    // int x = getClk();
    // printf("Current Time is %d\n", x);

    // TODO Generation Main Loop

    // 6. Send the information to the scheduler at the appropriate time.
    key_t key_id;
    int msgq_id, send_approve;
    struct msgbuff message_buffer;
    message_buffer.mtype = no_processes;

    key_id = ftok("sendProcess", 65);           // create unique key
    msgq_id = msgget(key_id, 0666 | IPC_CREAT); // create message queue and return id

    for (int i = 0; i < no_processes; i++)
    {
        message_buffer.msg_process = process_array[i];

        if (process_array[i].arrival_time > getClk()) // If no process's time has arrived yet
        {
            sleep(process_array[i].arrival_time - getClk());
        }

        // If a process's arrival time has come/ passed, send to scheduler.
        send_approve = msgsnd(msgq_id, &message_buffer, sizeof(message_buffer.msg_process), !IPC_NOWAIT);

        if (send_approve == -1)
        {
            perror("Failed to send. The error is ");
        }
        else
        {
            printf("process %d at time %d sent successfully\n", message_buffer.msg_process.id, message_buffer.msg_process.arrival_time);
        }
    }

    // 7. Clear clock resources
    destroyClk(0);

    // process generator waiting till the exit signal at clock destruction (end of scheduler)
    pause();

    return 0;
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption

    // getting the queue ID
    int keyid = ftok("sendProces", 65);
    int msqid = msgget(keyid, 0666 | IPC_CREAT);

    struct msqid_ds *buf;
    // deleting message queue
    msgctl(msqid, IPC_RMID, buf);

    printf("IPC DELETED\n");

    exit(1);
}
