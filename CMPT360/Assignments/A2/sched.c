#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sch_impl.h"

#define INITIAL_SIZE 20

process_t *create_tb(void) {
    process_t *Table = malloc(sizeof(process_t));

    if (Table == NULL)
    { // check if malloc failed
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    Table->capacity = INITIAL_SIZE; //start of with 20 slots for each entries
    Table->count = 0;

    Table->entries = malloc(Table->capacity * sizeof(sched));

    if (Table->entries == NULL)
    { // check if malloc failed
        printf("Memory Allocation Failed\n");
        free(Table); //free the previously allocated table structure

        return NULL;
    }

    return Table;
}


void grow_table(process_t *Table)
{
    size_t old_capacity = Table->capacity;
    size_t old_count = Table->count;

    //If the table is full, double its capacity
    if (old_capacity <= old_count)
    {
        size_t new_capacity = old_capacity * 2;

        //Request more memory for the entries
        sched *new_entries = realloc(Table->entries, new_capacity * sizeof(sched));

        if (new_entries == NULL)
        {
            fprintf(stderr, "Memory allocation failed.\n");
            free(Table->entries);
            free(Table);
            exit(1);
        }
        
    //Update the table with the new pointer and capacity
    Table->entries = new_entries;
    Table->capacity = new_capacity;
    }
}

void free_table(process_t *table) {

    if (table != NULL) {
        free(table->entries);
        free(table);
    }
}



//reads the workload file, validates every process, and stores valid processes in the table.
void read_file(const char *filename, process_t *table)
{
    FILE *fp = fopen(filename, "r");

    if (fp == NULL){
        fprintf(stderr, "Error opening File\n");
        free_table(table);
        exit(1);
    }

    char buf[500];  //temp arr to hold 1 line

    //Read each line from the file
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        char first_char;
        sched slot;     //temp struct to store the chars

        //Skip blank lines and comments
        //if the sscanf was able to successfully ignore the space and store it in first_char
        if (sscanf(buf, " %c", &first_char) != 1 || first_char == '#')
        {
            continue;      //skip char
        }

        //Read exactly three integers
        int valid_char = sscanf(buf, " %d %d %d",
            &slot.pid,
            &slot.arrival,
            &slot.cpu_time
        );
           
        

        //Validate the input
        if (valid_char != 3 ||
            slot.pid < 0 ||
            slot.arrival < 0 ||
            slot.cpu_time <= 0)
        {
            fprintf(stderr, "Error: Invalid workload.\n");
            fclose(fp);
            free_table(table);
            exit(1);
        }

        //Check for duplicate PIDs
        for (size_t i = 0; i < table->count; i++)
        {
            if (table->entries[i].pid == slot.pid) {
                fprintf(stderr, "Error: Invalid workload.\n");

                fclose(fp);
                free_table(table);
                exit(1);
            }
        }

        //Initialize sched fields
        slot.remaining_time = slot.cpu_time; 
        slot.first_run = -1;      //init to indicate it hasnt run, if 0 = run
        slot.completion = 0;
        slot.remaining_quantum = 0;

        //Expand memory if the table is full
        grow_table(table);

        //store process in the table from the slot
        table->entries[table->count] = slot;
        table->count++;
    }

    
    fclose(fp);

}

//sort by Arrival Time 
int compArrival(const void* a, const void* b) {
    sched* A = (sched*) a;
    sched* B = (sched*) b;

    //If arrival times are different, sort by arrival time
    if (A->arrival != B->arrival) {
        return (A->arrival - B->arrival);
    }
    
    //If they arrived at the exact same time, smaller PID goes first
    return (A->pid - B->pid);
}

//sort  by PID
int compPid(const void* a, const void* b) {
    sched* A = (sched*) a;
    sched* B = (sched*) b;
    
    return (A->pid - B->pid);
}

//FIFO
void simulate_fcfs(process_t *table)
{
    int current_time = 0;

    //sort processes by arrival time, then PID
    qsort(table->entries, table->count, sizeof(table->entries[0]), compArrival);

    for (size_t i = 0; i < table->count; i++)
    {
        sched *p = &table->entries[i];      //pointer to the first process in the enrty

        // Wait if the process has not arrived yet
        if (current_time < p->arrival)
        {
            current_time = p->arrival;
        }

        //Record when the process first starts running
        p->first_run = current_time;

        //Run the process until its CPU burst is complete
        current_time += p->cpu_time;

        //Record the completion time
        p->completion = current_time;

        //The process stopped executing
        p->remaining_time = 0;
    }
}




//Print the CPU timeline
void print_timeline(process_t *table)
{
    //find the ttl execution time
    int total_time = 0;

    //Find when the final process completes
    for (size_t i = 0; i < table->count; i++)
    {
        //checks if the current process's completion is > largest completion
        if (table->entries[i].completion > total_time)
        {
            //updates ttl time to be the largest one
            total_time = table->entries[i].completion;
        }
    }

    //Print the time row
    printf("time:");

    for (int t = 0; t < total_time; t++)
    {
        printf(" %d", t);
    }

    printf("\n");

    //Print the CPU run 
    printf("run :");

    for (int t = 0; t < total_time; t++)
    {
        int pid = -1;

        //Find which process runs at time t
        for (size_t i = 0; i < table->count; i++)
        {
            sched *p = &table->entries[i];  //pointer points to the address of the process

            //if the process started at the iteration of t and t is less than the total time
            //first_run <= current_time < completion time
            if (p->first_run <= t && t < p->completion)
            {
                pid = p->pid;   //stores the process's pid 
            }
        }

        //i fno process was found -> cpu is idle
        if (pid == -1)
        {
            printf(" -");
        }
        else
        {
            printf(" %d", pid);
        }
    }

    printf("\n");
}



//Print process metrics and system metrics
void print_metrics(process_t *table)
{
    long int ttl_tat = 0;   
    long int ttl_resp = 0;
    int context_switches = 0;
    int prev_pid = -1;
    int total_time = 0;

    //Sort processes by PID: ascending order 
    qsort(table->entries, table->count, sizeof(table->entries[0]), compPid);

    //Find the final completion time
    for (size_t i = 0; i < table->count; i++)
    {
        if (table->entries[i].completion > total_time)
        {
            total_time = table->entries[i].completion;
        }
    }

    // Calculate and print each process's metrics
    for (size_t i = 0; i < table->count; i++)
    {
        sched *p = &table->entries[i];

        int tat = p->completion - p->arrival;

        int response = p->first_run - p->arrival;

        printf("P%d: first run=%d completion=%d TAT=%d RESP=%d\n",
            p->pid,
            p->first_run,
            p->completion,
            tat,
            response
        );

        ttl_tat += tat;
        ttl_resp += response;
    }

    //Count context switches using the CPU timeline
    for (int t = 0; t < total_time; t++)
    {
        int current_pid = -1;   //- b/c pids could also be 0

        for (size_t i = 0; i < table->count; i++)
        {
            sched *p = &table->entries[i];

            if (p->first_run <= t && t < p->completion)
            {
                current_pid = p->pid;
                
            }
        }

        //Count a switch: if process is running and one has already run and they are not != -> count swithc
        if (current_pid >= 0 && prev_pid >= 0 && current_pid != prev_pid)
        {
            context_switches++;
        }

        //Idle time does not reset the previous PID
        if (current_pid >= 0)
        {
            prev_pid = current_pid;
        }
    }

    //calc metrics
    double avgTat = (double) ttl_tat / table->count;
    double avgResp = (double) ttl_resp / table->count;

    //Print the system metrics
    printf("System: ctx_switches=%d, avgTAT=%.3f, avgRESP=%.3f\n",
        context_switches, avgTat, avgResp);
    
}


int main(int argc, char *argv[])
{
    const char *filename = NULL;   //pointer to the text file string
    //initialize counters 
    int policy_found = 0;
    int filename_found = 0;

    //parse command-line arguments
    for (int i = 1; i < argc; i++)
    {
        //cheecks if the ttl chars is == 9
        if (strncmp(argv[i], "--policy=", 9) == 0)  //returns 0 if found
        {
            //checks if the strings affte the 9 chars are  is FCFS
            //moves the pointer to 9 chars inclduing 0
            char *new_str = &argv[i][9];
            int result = strcmp(new_str, "FCFS");

            if (result != 0 ) {
                fprintf(stderr, "Usage: ./sched --policy=FCFS --in=FILE\n");
                return 1;
            }

            policy_found = 1;   
        }
        else if (strncmp(argv[i], "--in=", 5) == 0)
        {
            if (argv[i][5] == '\0')
            {
                fprintf(stderr, "Usage: ./sched --policy=FCFS --in=FILE\n");
                return 1;
            }

            filename = &argv[i][5];
            filename_found = 1;
        }
        else
        {
            fprintf(stderr, "Usage: ./sched --policy=FCFS --in=FILE\n");
            return 1;
        }
    }

    //Check that both required arguments were provided
    if (policy_found == 0 || filename_found == 0)
    {
        fprintf(stderr, "Usage: ./sched --policy=FCFS --in=FILE\n");
        return 1;
    }

    //Create the process table
    process_t *table = create_tb();

    if (table == NULL)
    {
        return 1;
    }

    //Read the workload file
    read_file(filename, table);

    //Simulate FCFS
    simulate_fcfs(table);

    //Print timeline and metrics
    print_timeline(table);
    print_metrics(table);

    //Free allocated memory
    free_table(table);

    return 0;
}