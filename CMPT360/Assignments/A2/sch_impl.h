#ifndef _SCH_IMPL_H_
#define _SCH_IMPL_H_

#include <stddef.h> //For `size_t`

typedef struct sched
{
    int pid;
    int arrival;
    int cpu_time;
    int remaining_time;
    int first_run;
    int completion;
    int remaining_quantum;

} sched;

//table for storing the valid processes
typedef struct process_t{
    sched *entries;     //pointer to actual arr containing the processes
    size_t capacity;    //max # of entries arr can hold
    size_t count;       //# of items currently in the arr stored
} process_t;


//Function prototypes
process_t *create_table(void);
void grow_table(process_t *table);
void read_file(const char *filename, process_t *table);
void free_table(process_t *table);
int compare_arrival(const void *a, const void *b);
int compare_pid(const void *a, const void *b);
void print_timeline(process_t *table);
void print_metrics(process_t *table);

// typedef struct queue
// {

//     sched **entries;   //pointer entries that point to the sched
//     size_t capacity; //max # of entries
//     size_t count;    //# of items currently in the queue
//     int head;       //item at the front of the queue about to dequeue
//     int rear;       //item at the back that got enqueued

// } queue;

// //Function Prototypes
// queue *create_arr(int n);


#endif