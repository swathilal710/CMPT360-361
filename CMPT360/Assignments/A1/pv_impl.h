
#ifndef _PV_IMPL_H_
#define _PV_IMPL_H_

#include <stdlib.h>
#include <stddef.h> //For `size_t`

typedef struct proc
{
    int PID;
    int PPID;
    char STATE;
    int CPU_TIME;
    int MEM;
    char *COMMAND; // Pointer to string to dynamic allocate space for it

} proc;

typedef struct pv
{

    proc *entries;   // pointer to the proc struct
    size_t capacity; // how many slots exist
    size_t count;    // counter to track the memory used

} pv;

//Function Prototypes
pv *create_tb(void);

void dynamic_mem(pv *Table);

void read_file(char *filename, pv *Table);

int validate_record(proc*);

int duplicate_pid(proc *slot, pv* Table);

void print_rec(pv *Table);

int compPid(const void* a, const void* b);

void calc_ttl(pv *Table);

void snapshot(pv *Table);

void lookup(pv *Table, int pid);

void free_table(pv *Table);

#endif
