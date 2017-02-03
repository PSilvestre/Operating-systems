#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

#include "mythreads.h"

#define SIZE 384*1024*1024

int *array;
int length, count= 0;

pthread_t *ids;
pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;

int length_per_thread;


void * count3p(void *myI){
	int i, myStart, myEnd, myIdx= (int)myI;

	myStart= length_per_thread * myIdx; myEnd= myStart + length_per_thread;
	printf("Thread idx %d, Starting at %d Ending at %d\n", myIdx, myStart, myEnd-1);

  pthread_mutex_lock(&mtx);
	for(i=myStart; i < myEnd; i++){
		if(array[i] == 3){
			count++;
		}
	}
  pthread_mutex_unlock(&mtx);
}


void count3Launcher(int n) {
	int i;

	for (i= 0; i < n; i++) {
		Pthread_create(&ids[i], NULL, count3p, (void *)i);
	}
}


void count3Joiner(int n) {
	int i;

	for (i= 0; i < n; i++) {
		Pthread_join(ids[i], NULL);
	}
}


int main( int argc, char *argv[]){
	int i;
	struct timeval s,t;

	if ( argc != 2 ) {
		printf("Usage: %s <number of threads>\n", argv[0]); return 1;
	}
	int n = atoi(argv[1]);
	ids = (pthread_t *)malloc(n*sizeof(pthread_t));

	array= (int *)malloc(SIZE*sizeof(int));
	length = SIZE;
	srand(0);
	for(i=0; i < length; i++){
		array[i] = rand() % 4;
	}

	if ( (SIZE % n) != 0 ) {
		printf("Work unevenly distributed among threads is not supported by this implementation!!!\n");
		return 2;
	}

	length_per_thread = SIZE / n;
	printf("Using %d threads; length_per_thread =  %d\n", n, length_per_thread);

	gettimeofday(&s, NULL);
	count3Launcher(n);
	count3Joiner(n);
	gettimeofday(&t, NULL);

	printf("Count of 3s = %d\n", count);
	printf("Elapsed time (us) = %ld\n", 
		(t.tv_sec - s.tv_sec)*1000000 + (t.tv_usec - s.tv_usec));
	return 0;
}

