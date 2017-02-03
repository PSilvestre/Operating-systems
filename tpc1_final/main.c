#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define ARGVMAX 50
#define LINESIZE 1024

int runcommand(char* argv[]) {
  int result;
  int pid = fork();
  if(pid == 0){
    char const* f = argv[0];
    result = execvp(f, argv);
  }else{
      waitpid(pid, NULL, 0);
  }
  return result;
}

//This function receives a string with at most LINESIZE chars,
//and replaces the first ocurrence of the '\n' character with
//the string termination character '\0'
void cleanString(char str[]) {
	for(int i = 0; i < LINESIZE; i++)
		if(str[i] == '\n') {
			str[i] = '\0';
			break;
		}
}

int main(int argc, char* argv[]){

    char args[ARGVMAX][LINESIZE]; //the argv to pass
    int numArgs;
    char line[LINESIZE];
    char* av[ARGVMAX];

    fgets(args[0], LINESIZE, stdin);
    cleanString(args[0]);

    while(strlen(args[0]) != 0){ //if only \n is passed, it is removed by cleanString() thus it'll be 0 length
        fgets(line, LINESIZE, stdin);//Get the number of commands
        numArgs = atoi(line);
        av[0] = args[0]; //put the files name as the first argument
        for(int i = 1; i < numArgs+1; i++){//Get the arguments
            fgets(args[i], LINESIZE, stdin);
            cleanString(args[i]);
            av[i] = args[i]; //put the args in the pointer array
        }
        av[numArgs+1] = NULL;// argv has to be terminated by a NULL pointer
        if(runcommand(av) == -1){ //if the exec fails
            printf("Error executing binary: %s\n", av[0]);
            exit(0); //end the fork that was loose.
        }
        fgets(args[0], LINESIZE, stdin); //get next command
        cleanString(args[0]);
    }

    printf("No more commands.\n");
    return 0;
}
