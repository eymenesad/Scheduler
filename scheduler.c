#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure for an instruction
typedef struct {
    char name[20]; // Name of the instruction
    int duration;  // Duration of the instruction in ms
} Instruction;

// Define a structure for a process
typedef struct {
    char name[10];
    int priority;
    int arrivalTime;
    char type[10];
    int currentLine;
    Instruction instructions[21]; // Array of instructions
    int instructionCount; // Number of instructions
} Process;


// Function prototypes
void parseProcessFile(char* filename, Process* process) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[21];
    process->instructionCount = 0;
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
         // Copy the instruction name to the next available slot in the array
        strcpy(process->instructions[process->instructionCount].name, line);
        
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
        int lineCount = 0;
        sscanf(line, "%s %d %d %s", processName, &priority, &arrivalTime, processType);
        lineCount +=1;

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
Process* getNextProcess(Process* processes, int processCount, int currentTime) {
    Process* nextProcess = NULL;

    // Find the highest priority Platinum process that has arrived
    for (int i = 0; i < processCount; i++) {
        if (strcmp(processes[i].type, "PLATINUM") == 0 && processes[i].arrivalTime <= currentTime) {
            if (nextProcess == NULL ||
                (processes[i].priority < nextProcess->priority) ||
                (processes[i].priority == nextProcess->priority && strcmp(processes[i].name, nextProcess->name) < 0)) {
                nextProcess = &processes[i];
            }
        }
    }

    if (nextProcess != NULL) {
        return nextProcess;
    }

    // Find the highest priority Gold or Silver process that has arrived
    for (int i = 0; i < processCount; i++) {
        if ((strcmp(processes[i].type, "GOLD") == 0 || strcmp(processes[i].type, "SILVER") == 0) &&
            processes[i].arrivalTime <= currentTime) {
            if (nextProcess == NULL ||
                (processes[i].priority < nextProcess->priority) ||
                (processes[i].priority == nextProcess->priority && strcmp(processes[i].name, nextProcess->name) < 0)) {
                nextProcess = &processes[i];
            }
        }
    }

    return nextProcess;
}

int shouldPreempt(Process* currentProcess, Process* processes, int processCount, int currentTime) {
    // Preemption is allowed only for silver processes after their time quantum is reached
    if (strcmp(currentProcess->type, "SILVER") == 0 && currentTime - currentProcess->arrivalTime >= 3 * 80) {
        return 1; // Preempt the silver process
    }

    // Check if a gold process has reached its time quantum and should be upgraded to platinum
    if (strcmp(currentProcess->type, "GOLD") == 0 && currentTime - currentProcess->arrivalTime >= 5 * 120) {
        strcpy(currentProcess->type, "PLATINUM");
        return 0; // Continue executing the upgraded platinum process
    }

    // Check if a platinum process has completed its execution
    if (strcmp(currentProcess->type, "PLATINUM") == 0 && currentProcess->currentLine >= currentProcess->instructionCount) {
        return 0; // Process has completed, no need to preempt
    }
    // Check if there is a higher-priority process that has arrived
    for (int i = 0; i < processCount; i++) {
        if (currentProcess != &processes[i] && processes[i].arrivalTime <= currentTime) {
            if (processes[i].priority < currentProcess->priority ||
                (processes[i].priority == currentProcess->priority && strcmp(processes[i].name, currentProcess->name) < 0)) {
                return 1; // Preempt the current process
            }
        }
    }
    // No preemption for other cases
    return 0;
}

// Comparator function for sorting processes by arrival time
int compareArrivalTimeAndPriority(const void* a, const void* b) {
    const Process* processA = (const Process*)a;
    const Process* processB = (const Process*)b;

    // First, compare by arrival time
    if (processA->arrivalTime < processB->arrivalTime) {
        return -1;
    } else if (processA->arrivalTime > processB->arrivalTime) {
        return 1;
    } else {
        // If arrival times are equal, compare by priority
        if (processA->priority < processB->priority) {
            return -1;
        } else if (processA->priority > processB->priority) {
            return 1;
        } else {
            // If both arrival times and priorities are equal, compare by name
            return strcmp(processA->name, processB->name);
        }
    }
}

void scheduler(Process* processes, int processCount) {
    int currentTime = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    
    // Create an array of pointers to processes
    Process* processPointers[processCount];
    for (int i = 0; i < processCount; i++) {
        processPointers[i] = &processes[i];
    }
    
    // Sort the array of process pointers based on arrival time
    qsort(processPointers, processCount, sizeof(Process*), compareArrivalTimeAndPriority);
    
    // function to get the next process
    Process* currentProcess = getNextProcess(processes, processCount, currentTime);

    while (currentProcess != NULL) {
        // Handle context switch
        currentTime += 10; // Context switch time
        int processBurstTime = 0; // Total burst time for the current process
        /// Execute the process
        for (int i = currentProcess->currentLine; i < currentProcess->instructionCount; i++) {
            // Find the duration of the current instruction
            int duration = currentProcess->instructions[i].duration;
            // Check for preemption or quantum expiration
            if (shouldPreempt(currentProcess, processes, processCount, currentTime)) {
                currentProcess->currentLine = i ;
                break;
            }

            // Check if a silver process should be upgraded to gold
            if (strcmp(currentProcess->type, "SILVER") == 0 && (currentTime - currentProcess->arrivalTime) % (3 * 80) == 0) {
                strcpy(currentProcess->type, "GOLD");
                break;
            }

            // Check if a gold process should be upgraded to platinum
            if (strcmp(currentProcess->type, "GOLD") == 0 && (currentTime - currentProcess->arrivalTime) % (5 * 120) == 0) {
                strcpy(currentProcess->type, "PLATINUM");
                break;
            }

            // Execute the instruction
            currentTime += duration;
            currentProcess->currentLine++;
            // Update process burst time
            processBurstTime += duration;
        }
        // Calculate turnaround time for the current process
        int turnaroundTime = currentTime - currentProcess->arrivalTime;

        // Calculate waiting time for the current process
        int waitingTime = turnaroundTime - processBurstTime ;

        // Add the waiting time and turnaround time to the total
        totalWaitingTime += waitingTime;
        totalTurnaroundTime += turnaroundTime;
        // Select the next process to run
        Process* nextProcess = getNextProcess(processes, processCount, currentTime);
        if (nextProcess != currentProcess) {
            currentTime += 10; // Context switch time for switching processes
        }
        currentProcess = nextProcess;
        
    }

    // Calculate and print average waiting and turnaround times
    double avgWaitingTime = (double)totalWaitingTime / processCount;
    double avgTurnaroundTime = (double)totalTurnaroundTime / processCount;
    printf("Average Waiting Time: %.2lf\n", avgWaitingTime);
    printf("Average Turnaround Time: %.2lf\n", avgTurnaroundTime);
}


int main() {
    // Placeholder for process and instruction arrays
    Process processes[10]; // Assuming a maximum of 10 processes
    Instruction instru_ctions[21]; // Assuming a maximum of 21 instructions
    

    int processCount = 0;
    // Parse each process file
    for (int i = 0; i < 10; i++) {
        char filename[15];
        sprintf(filename, "P%d.txt", i + 1);
        sprintf(processes[i].name, "P%d", i + 1); // Set the process name
        processes[i].priority = -1;  // Default value indicating uninitialized
        processes[i].arrivalTime = -1; // Default value indicating uninitialized
        strcpy(processes[i].type, "UNKNOWN"); // Default value
        parseProcessFile(filename, &processes[i]);
        if (processes[i].instructionCount > 0) {
            processCount++; // Increment processCount for successfully parsed process
        }
    }
    // Parse the process definition file
    parseDefinitionFile("definition.txt", processes, 10);
    // Create an array to store processes from the definition file
    Process definedProcesses[10]; // Assuming a maximum of 10 processes in the definition file
    int definedProcessCount = 0; // Initialize the count of defined processes

    // Filter out the processes defined in the definition file
    for (int i = 0; i < 10; i++) {
        if (processes[i].arrivalTime >= 0) { // Check if process is defined
            definedProcesses[definedProcessCount] = processes[i];
            definedProcessCount++;
        }
    }
    

    // Parse the instruction set file
    parseInstructionFile("instructions.txt", instru_ctions);
    // Update the duration of each instruction in the defined processes
    for (int i = 0; i < definedProcessCount; i++) {
        for (int j = 0; j < definedProcesses[i].instructionCount; j++) {
            for (int k = 0; k < 21; k++) {
                if (strcmp(definedProcesses[i].instructions[j].name, instru_ctions[k].name) == 0) {
                    definedProcesses[i].instructions[j].duration = instru_ctions[k].duration;
                    break;
                }
            }
        }
    }
    
    
   // For testing: Print the process files to verify parsing
    /*for (int i = 0; i < 10; i++) {
        for (int j = 0; j < processes[i].instructionCount; j++) {
            printf("%s\n", processes[i].instructions[j]);
        }
    }*/
    
    // For testing: Print the instructions to verify parsing
    /*for (int i = 0; i < 21; i++) {
        printf("%s %d\n", instructions[i].name, instructions[i].duration);
    }*/
    
    // For testing: Print the definition.txt to verify parsing
    /*for (int i = 0; i < 10; i++) {
        printf("%s %d %d %d %s\n", processes[i].name, processes[i].priority, processes[i].arrivalTime,processes[i].instructionCount ,processes[i].type);
    }*/
    

    // Call the scheduler function with only the defined processes
    scheduler(definedProcesses, definedProcessCount);
    

    // Add code to calculate and print waiting and turnaround times

    return 0;
}


