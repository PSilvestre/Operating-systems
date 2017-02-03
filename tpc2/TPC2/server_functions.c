#include "server_functions.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

char* getTime(char* timeT) {
    struct tm* tm_info;

	time_t timer;
    time(&timer);
    tm_info = localtime(&timer);

    strftime(timeT, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    return timeT;
}

void functionA(int a, char* returnValue) {
    char timeT[26]; 
    printf("Function A started: %s\n", getTime(timeT));
    
    sleep(a);

    printf("Function A terminated: %s\n", getTime(timeT));
    sprintf(returnValue, "Function A executed with Sucess for %d seconds.",a);
}

void functionB(int a, int b, char* returnValue) {
    char timeT[26]; 
    printf("Function B started: %s\n", getTime(timeT));

	sleep(10);
    int result = a * b;

    printf("Function B terminated: %s\n", getTime(timeT));
    sprintf(returnValue, "Function B executed with Sucess. Result: %d.",result);
}

void functionC(char* a, char* returnValue) {
    char timeT[26];
    printf("Function C started: %s\n", getTime(timeT));

    for(int i = 0; i < strlen(a); i++)
	if(a[i] >= 'A' && a[i] <= 'Z')
	   a[i] = a[i] - ('A' - 'a'); 
    
    printf("Function C terminated: %s\n", getTime(timeT));
    sprintf(returnValue, "Function C executed with Sucess. Result: %s.",a);
}
