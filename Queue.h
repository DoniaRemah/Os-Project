// Linked list implementation of a Queue  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef short bool;
#define true 1
#define false 0

//Linked list node 
struct Node
{
    struct Node *next;
    int priority;
    int process_id;

    //Variables for the output file
    int finish_time;
    int waiting_time;
    int start_time;
    int stopped_time;
    int running_time;
    int remaining_time;
};

//function to create a new linked list node
struct Node *newNode(int p_id, int p_priority)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->next = NULL;
    temp->process_id = p_id;
    temp->priority = p_priority;
    return temp;
}

//Defining a Queue (front stores the front node of LL and rear stores the
// last node of LL)
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
void enQueue(struct Queue *q,  struct Node* newNode)
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
    if (p->priority > temp->priority)
    {
        // Insert New Node before head
        temp->next = q->Head;
        q->Head = temp;
    }
    else
    {
        // Traverse the list and find a position to insert new node
        while (p->next != NULL && p->next->priority <= temp->priority)
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
struct Node* popQueue(struct Queue* q)
{
    if (q->Head ==NULL)
        return NULL;
    struct Node *temp = q->Head;
    q->Head =q->Head->next;
    return temp;
}

// function to print process id in queue q 
void printQueue(struct Queue *q)
{
    struct Node *p = q->Head;
    while (p != NULL)
    {
        printf("process ID:%d ->", p->process_id);
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
struct Node* peekQueue(struct Queue* q)
{
    if (q->Head ==NULL)
        return NULL;
    struct Node *temp = q->Head;
    return temp;
}


