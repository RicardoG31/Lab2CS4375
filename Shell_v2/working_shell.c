#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

//Handle built in commands
int builtInCmdHandler(char** cmd) {
	int NoOfOwnCmds = 1, i, switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
  
    ListOfOwnCmds[0] = "exit"; 
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(cmd[0], ListOfOwnCmds[i]) == 0) { 
            switchOwnArg = i + 1; 
            break; 
        } 
    } 
    
    switch (switchOwnArg) { 
		case 1: 
		    printf("Closing Shell...\n"); 
		    exit(0); 
		default: 
        	break; 
    } 
  
    return 0; 
        
    
}


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
		//FIXME
		//Check if command is a built-in command
		if(execvp(command[0], command) < 0) {
			//execute built in commands (use a method)
			printf("If you used ls, you should not be here\n");
			builtInCmdHandler(command);
		}
		//FIXME
		else {
			cpid = fork();
			//Fork fails
			if(cpid < 0) {
				fprintf(stderr, "fork failed\n");
        		exit(1);
			}
			//Run child process
			else if(cpid == 0) {
				if (execve(command[0], command, envp)) {
		          printf("command not found\n");
		          exit(EXIT_FAILURE);
          		}
			}
			//Run parent process
			else if(cpid > 0) {
				wait(&status);
			}
			
		}
		
	}
	
	putchar('\n');
	free(lineptr);
	exit(status);
	
	
	return 0;
}

