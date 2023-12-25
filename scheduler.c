#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure for a process
typedef struct {
    char name[10]; // Name of the process, e.g., "P1"
    int priority;  // Priority of the process
    int arrivalTime; // Arrival time of the process
    char type[10]; // Type of the process (GOLD, SILVER, PLATINUM)
    int currentLine; // Line number of the last executed instruction
    // Add other necessary fields
} Process;

// Define a structure for an instruction
typedef struct {
    char name[20]; // Name of the instruction
    int duration;  // Duration of the instruction in ms
} Instruction;

// Function prototypes
void parseProcessFile(char* filename, Process* process);

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
void scheduler(Process* processes, int processCount, Instruction* instructions, int instructionCount);

int main() {
    // Placeholder for process and instruction arrays
    Process processes[10]; // Assuming a maximum of 10 processes
    Instruction instructions[21]; // Assuming 20 instructions plus 'exit'

    // Parse the process definition file
    //parseProcessFile("definition.txt", processes);

    // Parse the instruction set file
    parseInstructionFile("instructions.txt", instructions);
    // For testing: Print the instructions to verify parsing
    for (int i = 0; i < 21; i++) {
        printf("%s %d\n", instructions[i].name, instructions[i].duration);
    }

    // Call the scheduler function
    //scheduler(processes, 10, instructions, 21);

    // Add code to calculate and print waiting and turnaround times

    return 0;
}

void parseProcessFile(char* filename, Process* process) {
    // Implement file parsing logic
}

void scheduler(Process* processes, int processCount, Instruction* instructions, int instructionCount) {
    // Implement the scheduling algorithm
}
