#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv, char *envp[]) {
	//Process id
	int cpid;
	//User command, token, and line pointer  
	char *command[20];
	char *token;
	char *lineptr = NULL;
	//Size of memory allocation for cmdLine and lineptr
	size_t i, n;
	//Status of command
	int status;
	
	while(1) {
		printf("$ ");
		//Store user input in line and check the line was read successfully
		if(getline(&lineptr, &n, stdin) < 0) {
			break;
		}
		//Tokenize line
		token = strtok(lineptr, "\t\n\r");
		for (i = 0; i < 20 && token != NULL; i++) {
          command[i] = token;
          token = strtok(NULL, " \t\n\r");
        }
        command[i] = NULL;
		
		int builtInCommand = 0;
		cpid = fork();
		//Fork fails
		if(cpid < 0) {
			fprintf(stderr, "fork failed\n");
        	exit(1);
		}
		//Run child process
		else if(cpid == 0) {
			execvp(command[0], command);
			builtInCommand = builtInCmdHandler(command);
			
			if (execve(command[0], command, envp)) {
	        	if(builtInCommand > 0) {
	        		exit(0);
	        	}
	        	printf("command not found: %s\n", command[0]);	          		 			exit(EXIT_FAILURE);
        	}
        	
		}
		//Run parent process
		else if(cpid > 0) {
			wait(&status);
			//builtInCommand = builtInCmdHandler(command);
			if(strcmp(command[0], "exit") == 0) {
				exit(0);
			}
			
			
		}
		
	}
	
	putchar('\n');
	free(lineptr);
	exit(status);
	
	
	return 0;
}



// Function to execute builtin commands 
int builtInCmdHandler(char** parsed) 
{ 
    int NoOfOwnCmds = 2, i, switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    char* username; 
  
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "cd"; 
     
  
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
            switchOwnArg = i + 1; 
            break; 
        } 
    } 
  
    switch (switchOwnArg) { 
    case 1: 
        printf("Closing shell..\n"); 
        return 1; 
    case 2: 
        chdir(parsed[1]); 
        return 1;  
    default: 
        break; 
    } 
  
    return 0; 
} 


