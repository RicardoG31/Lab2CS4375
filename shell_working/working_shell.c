#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[])
{
    pid_t child;
    // Not going to use commands with more than 15 words; extra room for NULL 
    // term
    char *command[16], *tok, *lineptr = NULL; 
    // Number of bytes read into lineptr (mallocated internally by getline)
    size_t i, n; 
    int status;
  
    while (1)
    {
      printf("$ ");
      if (getline(&lineptr, &n, stdin) == -1)
          break;
      // First call to strtok stores the line in a static variable; second arg
      // is delimiters
      tok = strtok(lineptr, " \t\n\r"); 
      // strtok returns NULL once there are no more tokens
      for (i = 0; i < 16 && tok != NULL; i++) 
      {
          command[i] = tok;
          tok = strtok(NULL, " \t\n\r");
      }
      // `execve' expects a NULL terminator from both the command and the 
      // environment; this assumes i < 15...
      command[i] = NULL; 
      child = fork();
      // No error checking; too close to being not a "snippet"
      if (child == 0) 
      {
          if (execve(command[0], command, envp))
          {
              perror("execve");
              exit(EXIT_FAILURE);
          }
      }
      // Successful forks return positive process id's the parent
      if (child > 0) 
         wait(&status);
    }
    putchar('\n');  
    free(lineptr);
    exit(status);
}
