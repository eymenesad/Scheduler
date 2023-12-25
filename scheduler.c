#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure for a process
typedef struct {
    char name[10];
    int priority;
    int arrivalTime;
    char type[10];
    int currentLine;
    char instructions[100][20]; // Assuming a maximum of 100 instructions per process
    int instructionCount; // Number of instructions
} Process;


// Define a structure for an instruction
typedef struct {
    char name[20]; // Name of the instruction
    int duration;  // Duration of the instruction in ms
} Instruction;

// Function prototypes
void parseProcessFile(char* filename, Process* process) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[20];
    process->instructionCount = 0;
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        strcpy(process->instructions[process->instructionCount], line);
        process->instructionCount++;
    }

    fclose(file);
}
void parseInstructionFile(char* filename, Instruction* instructions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[50];
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%s %d", instructions[index].name, &instructions[index].duration);
        index++;
    }

    fclose(file);
}
void parseDefinitionFile(char* filename, Process* processes, int processCount) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[50];
    while (fgets(line, sizeof(line), file)) {
        char processName[10], processType[10];
        int priority, arrivalTime;
        sscanf(line, "%s %d %d %s", processName, &priority, &arrivalTime, processType);

        // Find the matching process and update its attributes
        for (int i = 0; i < processCount; i++) {
            if (strcmp(processes[i].name, processName) == 0) {
                processes[i].priority = priority;
                processes[i].arrivalTime = arrivalTime;
                strcpy(processes[i].type, processType);
                break;
            }
        }
    }

    fclose(file);
}
void scheduler(Process* processes, int processCount, Instruction* instructions, int instructionCount);

int main() {
    // Placeholder for process and instruction arrays
    Process processes[10]; // Assuming a maximum of 10 processes
    Instruction instructions[21]; // Assuming 20 instructions plus 'exit'

    // Parse the process definition file

    
    // Parse each process file
    for (int i = 0; i < 10; i++) {
        char filename[15];
        sprintf(filename, "P%d.txt", i + 1);
        sprintf(processes[i].name, "P%d", i + 1); // Set the process name
        processes[i].priority = -1;  // Default value indicating uninitialized
        processes[i].arrivalTime = -1; // Default value indicating uninitialized
        strcpy(processes[i].type, "UNKNOWN"); // Default value
        parseProcessFile(filename, &processes[i]);
    }

    // Parse the process definition file
    
    
   // For testing: Print the process files to verify parsing
    /*for (int i = 0; i < 10; i++) {
        for (int j = 0; j < processes[i].instructionCount; j++) {
            printf("%s\n", processes[i].instructions[j]);
        }
    }*/
    parseDefinitionFile("definition.txt", processes, 10);
    // For testing: Print the definition.txt to verify parsing
    for (int i = 0; i < 10; i++) {
        printf("%s %d %d %s\n", processes[i].name, processes[i].priority, processes[i].arrivalTime, processes[i].type);
    }

    // Parse the instruction set file
    parseInstructionFile("instructions.txt", instructions);
    // For testing: Print the instructions to verify parsing
    /*for (int i = 0; i < 21; i++) {
        printf("%s %d\n", instructions[i].name, instructions[i].duration);
    }*/
    
    // Call the scheduler function
    //scheduler(processes, 10, instructions, 21);

    // Add code to calculate and print waiting and turnaround times

    return 0;
}

void scheduler(Process* processes, int processCount, Instruction* instructions, int instructionCount) {
    // Implement the scheduling algorithm
}
