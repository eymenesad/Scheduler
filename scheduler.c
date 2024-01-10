#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
    int arrivalTimePrimal; // Arrival time unchanged
    char type[10]; 
    int currentLine; 
    Instruction instructions[21]; // Array of instructions
    int instructionCount; // Number of instructions
    int totalExecTime; // Total execution time so far
    int quantum; // Time quantum for the process
    int quantumCount; // Number of time quanta used so far
    int timeSlice; // Time slice used so far
} Process;

// Parse a process file and store the instructions in the process
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
// Parse the process definition file and update the process attributes
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
                processes[i].arrivalTimePrimal = arrivalTime;
                strcpy(processes[i].type, processType);
                if (strcmp(processes[i].type, "SILVER") == 0) {
                    processes[i].quantum = 80; // 80ms is the time quantum for silver processes
                } else if(strcmp(processes[i].type, "GOLD") == 0) { 
                    processes[i].quantum = 120; // 120ms is the time quantum for gold processes
                }else if(strcmp(processes[i].type, "PLATINUM") == 0) { 
                    processes[i].quantum = INT_MAX; // INT_MAX is used to represent infinity
                }
                break;
            }
        }
    }

    fclose(file);
}

void scheduler(Process* processes, int processCount) {
    int currentTime = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    Process* lastProcess = NULL; // Pointer to the last process that was executed

    // Execute processes in a loop until all processes are completed
    while (1) {
        Process* currentProcess = NULL;
        int highestPriority = 0;
        
        
        
        // Find the highest priority process that has arrived
        for (int i = 0; i < processCount; i++) {
            Process* possible = &processes[i];
            if (possible->arrivalTime <= currentTime &&
                possible->currentLine < possible->instructionCount) {// Check if this possible has arrived
                if(strcmp(possible->type, "PLATINUM") == 0){
                    if(currentProcess != NULL && strcmp(currentProcess->type , "PLATINUM")==0 ){// Check if this possible has higher priority to be scheduled
                        // Check if this possible has higher priority to be scheduled
                        if(currentProcess->currentLine==0){
                            if (possible->priority > highestPriority){// Check if this possible has higher priority to be scheduled
                                currentProcess = possible;
                                highestPriority = possible->priority;
                            }else if(possible->priority == highestPriority ){// Check if this possible has same priority and arrived earlier
                                if(possible->arrivalTime < currentProcess->arrivalTime){
                                    currentProcess = possible;
                                    highestPriority = possible->priority;
                                }else if(possible->arrivalTime == currentProcess->arrivalTime){
                                    if(strcmp(possible->name, currentProcess->name) < 0){
                                        currentProcess = possible;
                                        highestPriority = possible->priority;
                                    }
                                }
                            }
                        }
                    }else{// Check if this possible has higher priority to be scheduled
                        currentProcess = possible;
                        highestPriority = possible->priority;
                        break;
                        
                    }
                }else {
                    
                    // Check if this possible has higher priority to be scheduled
                        if (possible->priority > highestPriority){
                            currentProcess = possible;
                            highestPriority = possible->priority;
                        }
                        // Check if this possible has same priority and arrived earlier
                        else if(possible->priority == highestPriority ){
                            if(possible->arrivalTime < currentProcess->arrivalTime){// Check if this possible has same priority and arrived earlier
                                currentProcess = possible;
                                highestPriority = possible->priority;
                            }else if(possible->arrivalTime == currentProcess->arrivalTime){ // Check if this possible has same priority and arrived at the same time
                                if(strcmp(possible->name, currentProcess->name) < 0){
                                    currentProcess = possible;
                                    highestPriority = possible->priority;
                                }
                            }
                        }
                    
                }
            }  
        }
        
        // If no process is available to execute, break the loop
        if (currentProcess == NULL) {
           int zz=0;
           int aa = INT_MAX;
           // check if there is any process that has not arrived yet
            for (int i = 0; i < processCount; i++) {
                Process* possible = &processes[i];
                
                if (possible->currentLine == 0) {
                    aa = possible->arrivalTime;
                    zz=1;

                }

            }
            currentTime = aa;
            if(zz==1){
                continue;
            }else{
                break;
            }
            
            
        }
        // Handle context switch
        if (currentProcess != lastProcess) {
            currentTime += 10; // Context switch time
            // Check if the last process has used up its time quantum
            if(lastProcess!=NULL && lastProcess->timeSlice > 0){
                lastProcess->arrivalTime = currentTime;
                lastProcess->quantumCount +=1;
                lastProcess->timeSlice=0;
                // Check for preemption (Silver process) or upgrades (Gold to Platinum)
                if (strcmp(lastProcess->type, "SILVER") == 0 && lastProcess->quantumCount == 3 ) {
                    strcpy(lastProcess->type, "GOLD");
                    lastProcess->quantumCount = 0;
                    lastProcess->quantum=120;
            
                }
                if (strcmp(lastProcess->type, "GOLD") == 0 && lastProcess->quantumCount == 5 ) {
                    strcpy(lastProcess->type, "PLATINUM");
                    lastProcess->quantumCount = 0;
                    lastProcess->quantum=INT_MAX;
            
                }
            }
            
        }
        
        int duration = currentProcess->instructions[currentProcess->currentLine].duration;
        // Execute the instruction
        currentTime += duration;
        currentProcess->totalExecTime += duration;
        
        
        currentProcess->timeSlice += duration;
        currentProcess->currentLine++;
        // Check if the process has used up its time quantum
        if(currentProcess->timeSlice >= currentProcess->quantum){
            currentProcess->arrivalTime = currentTime;
            currentProcess->quantumCount +=1;
            currentProcess->timeSlice = 0;
        }
        // Check for preemption (Silver process) or upgrades (Gold to Platinum)
        if (strcmp(currentProcess->type, "SILVER") == 0 && currentProcess->quantumCount == 3 ) {
            strcpy(currentProcess->type, "GOLD");
            currentProcess->quantumCount = 0;
            currentProcess->quantum=120;
        }
        if (strcmp(currentProcess->type, "GOLD") == 0 && currentProcess->quantumCount == 5 ) {
            strcpy(currentProcess->type, "PLATINUM");
            currentProcess->quantumCount = 0;
            currentProcess->quantum=INT_MAX;
            
        }

            
            
            
        
        // Check if the process has completed
        if(strcmp(currentProcess->instructions[currentProcess->currentLine-1].name, "exit") == 0){
            // Calculate turnaround and waiting times
            int turnaroundTime = currentTime - currentProcess->arrivalTimePrimal;
            int waitingTime = turnaroundTime - (currentProcess->totalExecTime);
            totalWaitingTime += waitingTime;
            totalTurnaroundTime += turnaroundTime;
            
            
        }
        
       

        
        // Update the last process
        lastProcess = currentProcess;
        
}

    // Calculate and print average waiting and turnaround times
    double avgWaitingTime = (double)totalWaitingTime / (processCount);
    double avgTurnaroundTime = (double)totalTurnaroundTime / (processCount);
    printf("%.1lf\n", avgWaitingTime);
    printf("%.1lf\n", avgTurnaroundTime);
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
        processes[i].currentLine = 0; // Initialize the current line to 0
        processes[i].priority = -1;  // Default value indicating uninitialized
        processes[i].arrivalTime = -1; // Default value indicating uninitialized
        strcpy(processes[i].type, "UNKNOWN"); // Default value
        processes[i].totalExecTime = 0; // Initialize the total execution time to 0
        processes[i].quantumCount = 0; // Initialize the quantum to 0
        processes[i].timeSlice=0;// Initialize the time slice to 0
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
    
    // Call the scheduler function with only the defined processes
    scheduler(definedProcesses, definedProcessCount);
    return 0;
}


