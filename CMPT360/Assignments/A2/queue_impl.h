#ifndef _QUE_IMPL_H_
#define _que_IMPL_H_

#include <stddef.h> //For `size_t`

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        int *elements;
	
}Queue;

Queue * createQueue(int maxElements);
void Enqueue(Queue *Q,int element);
int Dequeue(Queue *Q);
int front(Queue *Q);
int display(Queue *Q);
int search(Queue *Q,int element);



#endif