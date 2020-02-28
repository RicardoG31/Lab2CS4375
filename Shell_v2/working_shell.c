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

// Help command builtin 
void openHelp() 
{ 
    puts("\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling"
        "\n>improper space handling"); 
  
    return; 
} 

// Function where the piped system commands is executed 
void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
    // 0 is read end, 1 is write end 
    int pipefd[2];  
    pid_t p1, p2; 
  
    if (pipe(pipefd) < 0) { 
        printf("\nPipe could not be initialized"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        printf("\nCould not fork"); 
        return; 
    } 
  
    if (p1 == 0) { 
        // Child 1 executing.. 
        // It only needs to write at the write end 
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute command 1.."); 
            exit(0); 
        } 
    } else { 
        // Parent executing 
        p2 = fork(); 
  
        if (p2 < 0) { 
            printf("\nCould not fork"); 
            return; 
        } 
  
        // Child 2 executing.. 
        // It only needs to read at the read end 
        if (p2 == 0) { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\nCould not execute command 2.."); 
                exit(0); 
            } 
        } else { 
            // parent executing, waiting for two children 
            wait(NULL); 
            wait(NULL); 
        } 
    } 
} 


// Function to execute builtin commands 
int ownCmdHandler(char** parsed) 
{ 
    int NoOfOwnCmds = 4, i, switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    char* username; 
  
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "cd"; 
    ListOfOwnCmds[2] = "help"; 
    ListOfOwnCmds[3] = "hello"; 
  
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
            switchOwnArg = i + 1; 
            break; 
        } 
    } 
  
    switch (switchOwnArg) { 
    case 1: 
        printf("\nGoodbye\n"); 
        exit(0); 
    case 2: 
        chdir(parsed[1]); 
        return 1; 
    case 3: 
        openHelp(); 
        return 1; 
    case 4: 
        username = getenv("USER"); 
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n", 
            username); 
        return 1; 
    default: 
        break; 
    } 
  
    return 0; 
} 

// function for finding pipe 
int parsePipe(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
} 



