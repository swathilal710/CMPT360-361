/* -----------------------------------------------------
 * filename: procview.c
 * name: Swathi Prema Lal
 * Student Id: 3147620
 * assignment: Assignment #1
 * Submission Date: September 19, 2026
 * file purpose: Reads and analyzes process snapshot records.
 * --------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h> //For `size_t`, `malloc`, `calloc`, `free`.
#include <string.h> //For `strcmp`, `strdup`.

#include "pv_impl.h"        //call the header func for structs and function prorotypes
#define INITIAL_SIZE 20     //starting size of the capacity


/*
 * create_tb
 * Purpose: Allocates dynamic memory for a new table and initializes its capacity and entry count.
 * Parameters: None.
 * Returns:
 *      Pointer to the new created table, or NULL if memory allocation fails.
 */

pv *create_tb(void)
{
    pv *Table = malloc(sizeof(pv));

    if (Table == NULL)
    { // check if malloc failed
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    Table->capacity = INITIAL_SIZE; // start of with 20 slots for each entries
    Table->count = 0;

    Table->entries = malloc(Table->capacity * sizeof(proc));

    if (Table->entries == NULL)
    { // check if malloc failed
        printf("Memory Allocation Failed\n");
        free(Table); // free the previously allocated table structure

        return NULL;
    }

    return Table;
}

/*
 * dynamic_mem
 * Purpose: Checks if the table is full and doubles its capacity if necessary.
 * Parameters: Table - pointer to the process table
 * Returns: None
 */
void dynamic_mem(pv *Table)
{
    size_t old_capacity = Table->capacity;
    size_t old_count = Table->count;

    // if the capacity is all used up, double it
    if (old_capacity ==  old_count)
    {
        size_t new_capacity = Table->capacity *= 2;


        //request more ram for entries
        proc *new_entries = realloc(Table->entries, new_capacity * sizeof(proc));

        
        if (Table->entries == NULL) { // check if malloc failed
            printf("Memory Allocation Failed\n");
            free(Table); // free the previously allocated table structure
            exit(0);

    }

    //update Table w/ new pointer and capacity
    Table->capacity = new_capacity;
    Table->entries = new_entries;

    printf("Capacity doubled to %zu\n", Table->capacity);
        
    }

}


/*
 * read_file
 * Purpose: Opens the file, parses each line into a temporary struct, calls validation check fucntion, 
            and stores valid records into the table.
 * Parameters:
 *      filename - pointer to the filename string user enters in the terminal
 *      Table - pointer to the process table
 * Returns: None.
 */
void read_file(char *filename, pv *Table)
{
    FILE *fp;
    proc slot; // temp struct storage for records

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening File\n");
        exit(1); 
    }

    char buf[500];
    int line_count = 1;
    char command[100];  //temp arr to store str from file


    // loop through + read line by line and store each record into a temp struct
    while (fgets(buf, sizeof(buf), fp))
    {
        
        //read formatted contents from a string arr -> sscanf + store it in temp struct
        int found = sscanf(buf, "%d %d %c %d %d %s",
                    &slot.PID, &slot.PPID, &slot.STATE, 
                    &slot.CPU_TIME, &slot.MEM, command);     //Since COMMAND is a pointer to string, it doesn't have any memory allocated to store rec yet

         
        // If the elements/records stored aren't 6 in ttl, show error and close
        if (found != 6)
        {
            fprintf(stderr, "Invalid process record at line %d\n", line_count);
            free_table(Table);
            fclose(fp);
            exit(1);
        }

        //copy command back to sturct
        slot.COMMAND = malloc(strlen(command) + 1);  //allocate memory for string in struct
        strcpy(slot.COMMAND, command);


        //validate records
        int check = validate_record(&slot);
        if (check == 0) {    //if the return val is 0 -> raise error and stop immediately : free Table
            fprintf(stderr, "Invalid process record at line %d\n", line_count);
            free_table(Table);
            fclose(fp);
            exit(1);

        }

        int dupliID = duplicate_pid(&slot, Table);
        if (dupliID == 0) {     //if the return val is 0 -> raise error and stop immediately : free Table
            fprintf(stderr,"Error! Invalid process record at line %d\n", line_count);
            free_table(Table);
            fclose(fp);

            exit(1);
        }

        //ensure records have enough memory
        dynamic_mem(Table);

        //store the records in entries in Table
        Table->entries[Table->count] = slot;
        Table->count += 1;

        line_count++;
        
    }

    fclose(fp);
}


/*
 * validate_record
 * Purpose: Verifies that the new temp struct passes the verification checks
 * Parameters:
 *      slot - pointer to the temporary process record being validated
 * Returns:
 *      1 if the record is valid, 0 if any error in validation.
 */
int validate_record(proc *slot) {

        //PID must be greater than 0
    
        if (slot->PID <= 0) {
            return 0;
        }

        //PPID must be >= 0
        if (slot->PPID < 0) {
            return 0;
        }

        //STATE has to be either S, Z or R
        if (slot->STATE != 'R' &&
            slot->STATE != 'S' &&
            slot->STATE != 'Z') {
            return 0;
        }

        //CPU_TIME must be >= 0
        if (slot->CPU_TIME < 0) {
            return 0;
        }

        //MEM must be >= 0
        if (slot->MEM < 0) {
            return 0;
        }

        //if COMMAND is missing entirely or contains spaces
        char space = ' ';
        if ((slot->COMMAND) == NULL) {
            return 0;
        }
        if (strchr(slot->COMMAND, space) != NULL) {     //if it returns a pointer to where the space is -> return 0
            return 0;
        }
       
        return 1;
    

}


/*
 * duplicate_pid
 * Purpose: Scans the temp struct to check if a repitition of the PID exists
 * Parameters:
 *      slot - pointer to the temporary process record containing the PID to check
 *      Table - pointer to the process table
 * Returns:
 *      1 if the PID is unique, 0 if a duplicate PID is found.
 */
int duplicate_pid(proc *slot, pv* Table) {
    
    if (Table == NULL) {
        return 0;
    }
   
    int current_PID = slot->PID;
    size_t i;
    for (i = 0; i < Table->count; i++) {
        if (Table->entries[i].PID == current_PID) {  //if the PID at the index in the Table struct == current stored PID -> duplicate
            return 0;
        
        }
        
    }
    
    return 1;
     
    
}



/*
 * print_rec
 * Purpose: sorts the table by PID, and Prints the summary statistics calculation
 * Parameters:
 *      Table - pointer to the process table
 * Returns: None
 * 
 */
void print_rec(pv *Table) {
    //print each line in the Table struct

    calc_ttl(Table);
    //sort the arr in ascending order
    //args: (Array, number of items, size of one item, comparator function)
    qsort(Table->entries, Table->count, sizeof(proc), compPid);

    
    printf("%-5s %-6s %-8s %-10s %-13s %-11s\n",
        "PID", "PPID", "STATE", "CPU(ms)", "MEMORY(KB)", "COMMAND");

    
    
    for (size_t i = 0; i < Table->count; i++) {     //loop through each entry slots
        printf("%-5d %-6d %-8c %-10d %-13d %-11s\n",
            Table->entries[i].PID,
            Table->entries[i].PPID, 
            Table->entries[i].STATE, 
            Table->entries[i].CPU_TIME, 
            Table->entries[i].MEM, 
            Table->entries[i].COMMAND);


    }
}

/*
 * calc_ttl
 * Purpose: Iterates through the process table to calculate and display summary statistics.
 * Parameters:
 *      Table - pointer to the process table
 * Returns: None.
 * 
 */

void calc_ttl(pv *Table) {

    //counter to keep track of the total
    int ttl_R = 0;
    int ttl_S = 0;
    int ttl_Z = 0;
    int ttl_mem = 0;
    int ttl_cpu = 0;

   
    printf("Processes: %ld", Table->count); //print the total count of processes

    //loop through table to check state + update ttl
    for (size_t i = 0; i < Table->count; i++) {
        if ((Table->entries[i].STATE) == 'R') {
            ttl_R++;
        }
        if((Table->entries[i].STATE) == 'S'){
            ttl_S++;
        }
        if((Table->entries[i].STATE) == 'Z'){
            ttl_Z++;
        }

        if (Table->entries[i].MEM){
            ttl_mem += Table->entries[i].MEM;   
        }
        if (Table->entries[i].CPU_TIME){
            ttl_cpu += Table->entries[i].CPU_TIME;   
        }

  
    }

    //tracker counters to find the biggest mem
    int mem_track = 0;
    int mem_pid = 0;
    char *mem_cmd;      //pointer to string of arr

    //tracker counters to find the biggest cpu
    int cpu_track = 0;
    int cpu_pid = 0;
    char *cpu_cmd;      //pointer to string of arr

    for (size_t j = 0; j < Table->count; j++) {
         if ((Table->entries[j].MEM) > mem_track) {     //if the current Mem > mem_track, mem_track becomes the bigger one
            mem_track = Table->entries[j].MEM;
            mem_pid = Table->entries[j].PID;
            mem_cmd = Table->entries[j].COMMAND;
        }

        if ((Table->entries[j].CPU_TIME) > cpu_track) {    
            cpu_track = Table->entries[j].CPU_TIME;
            cpu_pid = Table->entries[j].PID;
            cpu_cmd = Table->entries[j].COMMAND;
        }

        
    }
    printf("\nRunning: %d", ttl_R);
    printf("\nSleeping: %d", ttl_S);
    printf("\nZombie: %d", ttl_Z);
    printf("\nTotal memory: %d KB", ttl_mem);
    printf("\nTotal memory: %d ms", ttl_cpu);
    printf("\nHighest memory process: %d %s %d KB", mem_pid, mem_cmd, mem_track);
    printf("\nHighest CPU process: %d %s %d ms\n", cpu_pid, cpu_cmd, cpu_track);
    

    printf("\n");

}

/*
 * compPid
 * Purpose: Helper function used by qsort to compare and sort two PIDs at a time in ascending order
 * Parameters:
 *      a - void pointer to the first process record
 *      b - void pointer to the second process record
 * Returns:
 *      A negative integer if first PID is smaller, a positive integer if first PID is larger, or 0 if they are equal.
 */
int compPid(const void* a, const void* b) {
    proc* A =  (proc*) a;   //pointers to the proc struct
    proc* B = (proc*) b;

    
    //If A is bigger, A moves down
    //If B is bigger, B moves down
    return (A->PID - B->PID);
}


/*
 * lookup
 * Purpose: Searches through the Table for the specific PID, followed by looking for the children of that PID
 * Parameters:
 *      Table - pointer to the process table
 *      pid - the target PID
 * Returns: None.
 * 
 */
void lookup(pv *Table, int pid) {
    //first find where the PPID is the PID + print the children 
    //print the og PID + PPID, the children of the new PID

    //check for validity
    if (pid <= 0) {
        fprintf(stderr, "Invalid PID\n");
    }

    int pidFound = 0;   //counter for existing PID
    for (size_t i = 0; i < Table->count; i++) {
        
        //if the pid typed == PID in the table, then print
        if ((Table->entries[i].PID) == pid) {   
            printf("PID: %d\nParent PID: %d\nState: %c\nCPU time: %d\nMemory: %d\nCommand: %s\n",
                Table->entries[i].PID, Table->entries[i].PPID, Table->entries[i].STATE,
                Table->entries[i].CPU_TIME, Table->entries[i].MEM, Table->entries[i].COMMAND);
            
            pidFound++;     //increments the counter if the PID is found

        }
    }

    if (pidFound == 0) {        //if PID not found in the table(counter stays 0), print error
        fprintf(stderr, "Process 999 not found\n");
        return;
    }

    int child = 0;      //keep track of the # of children
    printf("Children: ");

    //loop through to find children of the pid and print
    for (size_t j = 0; j < Table->count; j++) {
        if (Table->entries[j].PPID == pid) {
            printf("%d ", Table->entries[j].PID);   //print the children of the PID
            child++;

        }
            
    }
    

    if (child == 0){    //if after the loop, no children are found
        printf("none\n");
        return;
    }  
    
    printf("\n");
    
    
}
    
  
/*
 * free_table
 * Purpose: Iterates through the table to safely deallocate the dynamically allocated memory for
            command strings before freeing the entries array and the table itself.
 * Parameters:
 *      Table - pointer to the process table
 * Returns: None.
 * 
 */
void free_table(pv* Table) {
    
    for (size_t i= 0; i < Table->count; i++) {
        free(Table->entries[i].COMMAND);    //free the dynamically allocated strings
    
    }

    free(Table->entries);      //free the entries in the mem arr

    free(Table);        //finnaly free the whole Table
}


/*
 * main
 * Purpose: The main program which validates command-line arguments, initializes the process table, 
            calls the read_file, Pid lookup, and snapshot printing functions.
 * Parameters:
 *      argc - total num of args(counter) passed in the program
 *      argv - an array of string pointers passed when running the program
 * Returns:
 *      0 if program executes successfully, or 1 if error occurs
 */
int main(int argc, char *argv[])
{
    //check if filename provided
    if (argc < 2)
    {
        fprintf(stderr, "Error: cannot open file!\n");
    
        //Terminate immediately with a nonzero exit status
        exit(1);
    }

    pv *table = create_tb();  //create a table

    if (table == NULL)
    {
        return 1;
    }

    //reading the file
    read_file(argv[1], table);
    if (argc == 2) {        //if there are 2 arguments passed in total: ./procview ./<filename.txt>
        //print the records
        print_rec(table);

    }
    if (argc == 3) {    //if htere are 3 args in ttl: ./procview ./<filename.txt> <PID number>
        //Convert argv[2](PID number) from a string to an integer: in the terminal, the number is read as a string
        int target_pid = atoi(argv[2]);     
        
        lookup(table, target_pid);  //pass in the pid as the int pid 
    }
   

    free_table(table);    //make sure main freed all the heaps so no leaks

    return 0;
}
