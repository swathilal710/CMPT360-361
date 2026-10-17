# Procview:  Process Snapshot Analyzer

# Academic Integrity Statement
I certify that this submission represents entirely my own work.


The program uses two structs:
- 'proc' : (Process Record): Represents a single process. It stores the PID, PPID, CPU_TIME, MEM, STATE, and COMMAND as a pointer to a dynamically allocated string.
- 'pv' : (Process Table): It stores a pointer to the array(entry) of 'proc' records, the count of valid records, and the capacity allocated for the array.


# How Data is Managed
Dynamic memory allocation is used to store the process snapshot without hardcoding the number of process records.

- Table Allocation: Memory is initially allocated for the entries using an initial capacity of 20.

- Capacity Growth: The 'dynamic_mem' function checks whether 'count == capacity'. If the array is full, it uses 'realloc' to double the capacity for the array.

- COMMAND Pointer Storage: To not give the COMMAND string a designated limit to the characters, a pointer to an array of strings is used. 'malloc(strlen(command) + 1)' is used to allocate enough memory for the command and its null terminator before the string is copied from the temporary buffer and to the temporary struct.

- Clean-up: Function 'free_table' is used to call under fuctions that allocates memory (including the main) before program termination. It safely iterates through all the records, frees the COMMAND strings, then the record, then finally the Table. This is done to prevent any memory leaks.


# How to Compile
- From the Makefile provided, the program can be compiled using the instruction: 'make'
This produces an executable program called 'procview'. 
- To remove the executable files, the instruction 'make clean' is used. 

Run: make run
This program supports two modes:

    - Snapshot Summary Mode: ./procview <filename.txt>
    This reads the process records in the file, validates the records, stores it in the Table, calculates the summary statistics, sorts the PID in ascending order, and then finnaly prints the process table. 

    - Process Lookup Mode: ./procview <filename.txt> ##
    This searches for the PID number in table and prints process record, and prints the PIDs of all processes whose PPID is the number passed in the argument.

Valgrind: 
To check the program for memory errors and leaks: make valgrind
Valgrind runs using the filename, so before running it, add the filename beside the instruction: valgrind --leak-check=full ./$(MAIN) <filename.txt>


Known Errors or Limitations:
This program currently runs smoothly with no errors present.



