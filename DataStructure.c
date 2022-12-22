// Linked list implementation of a Queue
// definition of all the needed structures
#include "headers.h"

// Linked list node
struct Node
{
    struct Node *next;
    struct process node_process;
    // priority for HPF & MLFL, Running time for SJF, Arrival time for RR
    int sorting_priority;
    int pID; // the id of the process after forking, to be able to communicate with the process
};

// function to create a new linked list node
struct Node *newNode(int p_id, int p_priority,int sort_p)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->next = NULL;
    temp->pID = p_id;
    temp->node_process.priority = p_priority;
    temp->sorting_priority = sort_p;
    return temp;
}

// Defining a Queue (front stores the front node of LL and rear stores the
//  last node of LL)
struct Queue
{
    struct Node *Head;
};
// A utility function to create an empty queue
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    q->Head = NULL;
    return q;
}

// function to add a key k to queue q
void enQueue(struct Queue *q, struct Node *newNode)
{
    // Create a new LL node
    struct Node *temp = newNode;

    // If queue is empty, then new node is front and rear both
    if (q->Head == NULL)
    {
        q->Head = temp;
        return;
    }
    struct Node *p = q->Head;
    if (p->sorting_priority > temp->sorting_priority)
    {
        // Insert New Node before head
        temp->next = q->Head;
        q->Head = temp;
    }
    else
    {
        // Traverse the list and find a position to insert new node
        while (p->next != NULL && p->sorting_priority <= temp->sorting_priority)
            p = p->next;
        // Either at the ends of the list
        // or at required position
        temp->next = p->next;
        p->next = temp;
    }
}

// function to remove a key from queue q
void deQueue(struct Queue *q)
{
    // If queue is empty, return NULL.
    if (q->Head == NULL)
        return;

    // Store previous front and move front one node ahead
    struct Node *temp = q->Head;

    q->Head = q->Head->next;

    // If front becomes NULL, then change rear also as NULL

    free(temp);
}

// function to remove an element from the front of the queue q
struct Node *popQueue(struct Queue *q)
{
    if (q->Head == NULL)
        return NULL;
    struct Node *temp = q->Head;
    q->Head = q->Head->next;
    return temp;
}

// function to print process id in queue q
void printQueue(struct Queue *q)
{
    struct Node *p = q->Head;
    while (p != NULL)
    {
        printf("process ID:%d ->", p->node_process.id);
        p = p->next;
    }
    printf("NULL\n");
}

// function to check if queue is empty
bool isEmpty(struct Queue *q)
{
    return (q->Head == NULL) ? true : false;
}

// function to returns the element at the front the queue without doing anything
struct Node *peekQueue(struct Queue *q)
{
    if (q->Head == NULL)
        return NULL;
    struct Node *temp = q->Head;
    return temp;
}
