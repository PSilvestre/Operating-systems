#include <stdlib.h>
#include <string.h> 
#include <stdio.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>


#define ARGVMAX 100
#define LINESIZE 1024
#define STRINGSIZE 500

int makeargv(char *s, char* argv[ARGVMAX]) {
  // pre: argv is
  // out: argv[] points to all words in the string s (*s is modified!)
  // return: number of words pointed to by the elements in argv (or -1 in case of error) 

  int ntokens;
	
  if ( s==NULL || argv==NULL || ARGVMAX==0) 
    return -1;

  ntokens = 0;
  argv[ntokens]=strtok(s, " \t\n");
  while ( (argv[ntokens]!= NULL) && (ntokens<ARGVMAX) ) {
    ntokens++;
    argv[ntokens]=strtok(NULL, " \t\n"); 
  }
  argv[ntokens] = NULL; // it must terminate with NULL
  return ntokens; 
}


void runcommand(char* argv[]) {
  pid_t rc = getpid();
  pid_t pid = fork();
  if(pid == 0){
    char const* f = argv[0];
    execvp(f, argv);
  }
  else{
    waitpid(pid, NULL, 0);
  }
  return;
}

void init(char* currentDir, int maxCurrentDir,  char* currentUser, int maxCurrentUser, char* host, int maxHost){
  
  getcwd(currentDir, maxCurrentDir);
  getlogin_r(currentUser,maxCurrentUser);
  gethostname(host, maxHost);
  
}


int main() {
  char line[LINESIZE];
  char* av[ARGVMAX];
  char cwd[STRINGSIZE];
  char cu[STRINGSIZE];
  char host[STRINGSIZE];
  
  init(cwd, STRINGSIZE, cu, STRINGSIZE, host, STRINGSIZE);

  system("clear");
  printf("[%s@%s]-%s:", cu, host, cwd); fflush(stdout); //writes the prompt on the standard output while ( fgets( line, LINESIZE, stdin ) != NULL ) {

  while ( fgets( line, LINESIZE, stdin ) != NULL ) {
    if ( makeargv( line, av) > 0 ) 
      runcommand( av );
    printf("\n[%s@%s]-%s:", cu, host, cwd); fflush(stdout);
  }

  return 0;
}
