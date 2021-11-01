/* COP 3502C Assignment 2
This program is written by: Edelis Molina Rios */

/* Circular doubly linked list and queue data structure */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"


#define G 10      // Execution Grounds
#define EMPTY -1

// node struct for soldier. Stores one soldier/node of the linked list
typedef struct soldier 
{
  int seq; // sequence Number
  struct soldier* prev;
  struct soldier* next;
}soldier;

// node struct for a queue
typedef struct queue
{
  soldier* front;
  soldier* back;
  int nodeCount;
  int k, th;
  char queueName[50]; // per instructions, name is <= 50 characters
}queue;

/* Function prototypes for queues */
void init(queue* qPtr);
int enqueue(queue* qPtr, int val);
int dequeue(queue* qPtr);
int isEmpty(queue* qPtr);
int peek(queue* qPtr);

/* Required functions per instructions */
soldier* createSoldier(int sequence); 
void createReverseCircle(queue* q); 
void rearrangeCircle(queue* q);
void display(queue* q, FILE *ofp);

/* Additional functions */
void phase1Execution(queue* q, FILE *ofp);
void phase2Execution(queue* q, FILE *ofp);



/* Master of Ceremonies */
int main(void) {

  atexit(report_mem_leak);

  int gCount, gNumber; 
  char gName[50];
  int n, k, th;     // number of soldiers, killing sequence, threshold (stop the killing process)

  FILE *ifp = fopen("in.txt", "r");
  FILE *ofp = fopen("out.txt", "w");

  if((ifp) != NULL)
  {
    // Allocate space for an array of queue (Execution Grounds) and initialize it.
    queue* queueArr = (queue*) malloc(sizeof(queue) * G);
    for(int i = 0; i < G; i++)
      init(&queueArr[i]);

    // Group of soldiers captured
    fscanf(ifp, "%d", &gCount);

    // Fill in gCount structs queue
    for(int j = 0; j < gCount; j++)
    {
      fscanf(ifp, "%d %s %d %d %d", &gNumber, gName, &n, &k, &th);
      queueArr[gNumber - 1].nodeCount = n;
      queueArr[gNumber - 1].k = k;
      queueArr[gNumber - 1].th = th;
      strcpy(queueArr[gNumber - 1].queueName, gName);

      createReverseCircle(&queueArr[gNumber - 1]);
    }

    // ********************************
    printf("Initial nonempty lists status\n");
    fprintf(ofp,"Initial nonempty lists status\n");
    for(int k = 0; k < G; k++)
    {
      if(!isEmpty(&queueArr[k]))
      {
        // printf("\n");
        // fprintf(ofp, "\n");
        printf("%d %s ", k + 1, queueArr[k].queueName);
        fprintf(ofp,"%d %s ", k + 1, queueArr[k].queueName);
        display(&queueArr[k], ofp);
      } 
    } 

    // ********************************
    printf("\nAfter ordering nonempty lists status\n");
    fprintf(ofp, "\nAfter ordering nonempty lists status\n");
    for(int z = 0; z < G; z++)
    {
      if(!isEmpty(&queueArr[z]))
      {
        // printf("\n");
        // fprintf(ofp, "\n");
        rearrangeCircle(&queueArr[z]);     
        printf("%d %s ", z + 1, queueArr[z].queueName);
        fprintf(ofp, "%d %s ", z + 1, queueArr[z].queueName);
        display(&queueArr[z], ofp);
      } 
    }


    // ********************************
    printf("\nPhase1 execution\n\n");
    fprintf(ofp, "\nPhase1 execution\n\n");
    for(int y = 0; y < G; y++)
    {
      if(!isEmpty(&queueArr[y]))
      {
        printf("Line# %d %s\n", y + 1, queueArr[y].queueName);
        fprintf(ofp, "Line# %d %s\n", y + 1, queueArr[y].queueName);

        phase1Execution(&queueArr[y], ofp); 

        printf("\n");    
        fprintf(ofp, "\n");  
      } 
    }

    printf("Phase2 execution\n");
    fprintf(ofp, "Phase2 execution\n");
    phase2Execution(queueArr, ofp);

    fclose(ifp);
    fclose(ofp);
    free(queueArr);
  }
  else{
    printf("Input file cannot be opened!!!\n");
    exit(1);
  }

  return 0;
}


/* Initialize a struct queue */
void init(queue* qPtr)
{
  qPtr->front = NULL;
  qPtr->back = NULL;
  qPtr->nodeCount = 0;
  qPtr->k = 0;
  qPtr->th = 0;
}





/* Returns true if the queue pointed to by qPtr is empty>>>>>*/
int isEmpty(queue* qPtr)
{
  return qPtr->front == NULL;
}



/* Creates circular double linked list. Nodes contains sequence numbers 
  in reverse order. Uses createSoldier() and enQueue() functions */
void createReverseCircle(queue* q)
{
  int seq = q->nodeCount;
  while(seq > 0)
  {
    if(enqueue(q, seq)) // enqueue successfully
      seq--;
  }
}


/* <<<<<<<<<<<<<<<<<<<<<< Queue functions >>>>>>>>>>>>>>>>>>>>>>>>>> */

/* Display queue function */
void display(queue* q, FILE *ofp)
{
  soldier *t = q->front;

  if(q->front == NULL)
  {
    printf("List is EMPTY\n");
    return;
  }

  // Now, iterate through each element, printing.
  do{
    // The if-else statement below is a quick fix to perfect match the output
    // file for passing the test cases. Do not fprintf an space after the last
    // node seq 
    if(t->next == q->front){
      printf("%d", t->seq);
      fprintf(ofp, "%d", t->seq);
    }      
    else{
      printf("%d ", t->seq);
      fprintf(ofp, "%d ", t->seq);
    }

    t = t->next;  

  // Here we check if the next element is back to the front, if so, we stop!
  } while(t != q->front);

  // Go to the next line.
  printf("\n");
  fprintf(ofp, "\n");
}


/* Enqueue function */
int enqueue(queue* qPtr, int val)
{
  soldier *temp;

  // Allocate space for a new soldier to add into the queue
  temp = createSoldier(val);

  if(temp != NULL)
  {
    // If the queue is NOT empty, set the old "last" node to point to this newly created node.
    if(qPtr->back != NULL)
    {
      temp->next = qPtr->front;  
      temp->prev = qPtr->back;   
      qPtr->back->next = temp;   
      qPtr->front->prev = temp;
    }

    // Reset the back of the queue to our newly created node.
    qPtr->back = temp;

    // If the queue was previously empty, ALSO set the front of the queue.
    if(qPtr->front == NULL)
    {
      temp->next = temp->prev = temp; // link to yourself
      qPtr->front = temp;
    }

    return 1;
  }
    
  // No change to the queue was made bc we couldn't find space for our new enqueue.
  else
    return 0;
}


/* Peek function (or front function)
  If the queue pointed to by qPtr is non-empty, the value stored at the front 
  of the queue is returned. Otherwise, -1 (EMPTY) is returned */
int peek(queue* qPtr)
{
  if(qPtr->front != NULL)
    return qPtr->front->seq;
  else
    return EMPTY;
}


/* Dequeue function 
  If the queue pointed to by qPtr is non-empty, then the value at the front
  of the queue is deleted from the queue and returned. Otherwise, -1 (EMPTY) is returned
  to signify that the queue was already empty when the dequeue was attempted.*/
int dequeue(queue* qPtr)
{
  soldier* tmp;
  int retval;

  // Check empty status. In fact, there is no need for this as the caller function
  // checks if the queue is empty
  if(qPtr->front == NULL)
    return EMPTY;
    
  // Temp pointer to free the memory for this soldier node
  tmp = qPtr->front;
  
  // Queue contains one node, delete it and make front and back pointers NULL
  if(qPtr->front->next == qPtr->front) 
  {
    // Store front value to return
    retval = qPtr->front->seq;

    // Make queue front and back pointers NULL
    qPtr->front = NULL;
    qPtr->back = NULL;

    free(tmp);
    return retval;
  }
  else  // The queue contains more than one node
  {
    // Store front value to return
    retval = qPtr->front->seq;

    // Make front as next of front, and update links
    qPtr->front = qPtr->front->next;
    qPtr->front->prev = qPtr->back;
    qPtr->back->next = qPtr->front;

    free(tmp);
    return retval;
  }

}


/* <<<<<<<<<<<<<<<<< Required functions per instructions >>>>>>>>>>>>>>>>>> */

/* Creates a soldier node. DMA a soldier struct & return soldier node */
soldier* createSoldier(int sequence)
{
  soldier* soldierNode = (soldier*) malloc(sizeof(soldier));
  soldierNode->seq = sequence;
  soldierNode->next = NULL;
  soldierNode->prev = NULL;  

  return soldierNode;
}


/* Rearrange Circular Doubly Linked list */
void rearrangeCircle(queue* q)
{
  soldier* main_list = q->front->next;
  soldier* rev_list = q->front;
  rev_list->next = q->back; 

  while(main_list != q->front)
  {
    soldier* temp = main_list;
    main_list = main_list->next;

    temp->next = rev_list;
    rev_list->prev = temp;
    rev_list = temp;
  }

  rev_list->prev = q->back;

  soldier* tmp = q->front;
  q->front = q->back;
  q->back = tmp;
}


/* Phase1 Execution Process */

/* This function deletes one soldier node in k - 1 position starting from head */
void phase1Execution(queue *q, FILE *ofp)
{
  soldier* t = q->front;
  soldier* del;  
  
  while(q->nodeCount > q->th)
  {
    int ite = 1;

    // Traverser to the k-1 node (stop one node before the kth node) to delete the kth node
    while(ite ++ < (q->k - 1))
      t = t->next;

    // code reaches here one node before the kth node. Delete next node
    del = t->next;
    printf("Soldier# %d executed\n", del->seq);
    fprintf(ofp, "Soldier# %d executed\n", del->seq);

    // if node to be deleted is pointed to by front, adjust front position
    if(del == q->front)
      q->front = t->next->next;
    
    // if node to be deleted is pointed to by back, adjust back position
    if(del == q->back)
      q->back = t;

    t->next = t->next->next;
    t->next->prev = t;
    free(del);

    t = t->next;
    q->nodeCount--;
  } 
}



/* Phase2 Execution Process */
void phase2Execution(queue *q, FILE *ofp)
{
  int count = 0;
  int peekTest;

  // Retrieves # of soldiers left after Phase1 Execution process
  for(int i = 0; i < G; i++)
  {
    if(!isEmpty(&q[i]))
    count += q[i].nodeCount;
  }

  // Loop "count" times and dequeue node with highest seq across all the fronts
  for(int j = 0; j < count; j++)
  {
    int front_max = -1, max_idx = -1;

    // Loop "G" times (all the fronts) to find node with highest seq
    for(int i = 0; i < G; i++)
    {
      if(!isEmpty(&q[i]))
      {
        peekTest = peek(&q[i]);

        if(peekTest > front_max)
        {
          front_max = peekTest;
          max_idx = i;
        }
      }

    }

    // dequeue j times node with the highest seq across all the fronts
    int exe = dequeue(&q[max_idx]);

    if(j < count - 1 )
    {
      printf("Executed Soldier %d from line %d\n", exe, max_idx + 1);
      fprintf(ofp, "Executed Soldier %d from line %d\n", exe, max_idx + 1);
    }
    else // Last node soldier will survive. Print survive statement after dequeue last soldier 
    {
      printf("\nSoldier %d from line %d will survive", exe, max_idx + 1);
      fprintf(ofp, "\nSoldier %d from line %d will survive", exe, max_idx + 1);
    } 

  } // For loop "count" times

} // Function Phase2 Execution Process 
