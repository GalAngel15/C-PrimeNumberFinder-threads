#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "lock.c"

void parseargs(char *argv[], int argc, int *lval, int *uval, int *n, int *t);
int isprime(int n);
void *mythread(void *arg);
pthread_mutex_t lock= PTHREAD_MUTEX_INITIALIZER;
rwlock_t myLock;
int count=0;
int num=0;
char *flagarr = NULL;

typedef struct {
	int lval; 
	int uval;
} myarg_t;

int main (int argc, char **argv)
{
	int lval = 2;
	int uval = 100;
	int nval=10;
	int t=4;

	// Parse arguments
	parseargs(argv, argc, &lval, &uval, &nval, &t);
	if (uval < lval)
	{
		fprintf(stderr, "Upper bound should not be smaller then lower bound\n");
		exit(1);
	}    
	if (lval < 2)
	{
		lval = 2;
		uval = (uval > 1) ? uval : 1;
	}
	if (nval < 0)
	{
		nval = 0;
	}
	if (t < 1)
	{
		t = 1;
	}

	// Allocate flags
	flagarr= (char *)malloc(sizeof(char) * (uval-lval+1));
	if (flagarr == NULL)
		exit(1);
	myarg_t args;
	args.lval=lval;
	args.uval=uval;
	num=lval;
	rwlock_init(&myLock);
	pthread_t *allThread = malloc(sizeof(pthread_t)*t);
	for (int i=0; i<t; i++)
	{
		pthread_create(&allThread[i],NULL,mythread,&args);
	}
	for (int i=0; i<t; i++)
	{
		pthread_join(allThread[i],NULL);
	}
	// Print results
	printf("\nFound %d primes%c\n", count, count ? ':' : '.');
	for (num = lval; num <= uval && nval>0; num++)
		if (flagarr[num - lval])
		{
			nval--;
			count--;
			printf("%d%c", num, count&&nval ? ',' : '\n');  
		}
	free(allThread);
	free(flagarr);
	return 0;
}

void *mythread(void *arg)
{
	// Set flagarr
	myarg_t *args=(myarg_t*)arg;
	while(num <= args->uval)
	{
		rwlock_acquire_writelock(&myLock);
		int temp = num;
		num++;
		rwlock_release_writelock(&myLock);
		if(temp>args->uval)
			return NULL;
		printf("%d ,",temp);
		if (isprime(temp))
		{
			flagarr[temp - args->lval] = 1; 
			pthread_mutex_lock(&lock);
			count++;
			pthread_mutex_unlock(&lock);
		} else {
			flagarr[temp - args->lval] = 0; 
		}
	}
	return NULL;
}

// NOTE : use 'man 3 getopt' to learn about getopt(), opterr, optarg and optopt 
void parseargs(char *argv[], int argc, int *lval, int *uval, int *n, int *t)
{
  int ch;

  opterr = 0;
  while ((ch = getopt (argc, argv, "l:u:n:t:")) != -1)
    switch (ch)
    {
      case 'l':  // Lower bound flag
        *lval = atoi(optarg);
        break;
      case 'u':  // Upper bound flag 
        *uval = atoi(optarg);
        break;
      case 'n':  // n flag 
        *n = atoi(optarg);
        break;
      case 't':  // t flag 
        *t = atoi(optarg);
        break;
      case '?':
        if ((optopt == 'l') || (optopt == 'u') || (optopt == 'n') || (optopt == 't'))
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        exit(1);
      default:
        exit(1);
    }    
}
int isprime(int n){
	static int *primes = NULL; 	// NOTE: static !
	static int size = 0;		// NOTE: static !
	static int maxprime;		// NOTE: static !
	int root;
	int i;

	// Init primes array (executed on first call)
	 if (primes == NULL) {
		rwlock_acquire_writelock(&myLock); // Acquire write lock for initialization
		if (primes == NULL) { // Double check after acquiring lock
		    primes = (int *)malloc(2 * sizeof(int));
		    if (primes == NULL) {
		        rwlock_release_writelock(&myLock); // Release the lock on failure
		        exit(1);
		    }
		    size = 2;
		    primes[0] = 2;
		    primes[1] = 3;
		    maxprime = 3;
		}
		rwlock_release_writelock(&myLock); // Release write lock after initialization
	    }
	root = (int)(sqrt(n));

	// Update primes array, if needed
	if(root > maxprime)
	{
		while (root > maxprime)
		{
			for (i = maxprime + 2 ;  ; i+=2)
				if (isprime(i))
				{
					rwlock_acquire_writelock(&myLock);
					size++;
					primes = (int *)realloc(primes, size * sizeof(int));
					if (primes == NULL){
						rwlock_release_writelock(&myLock);
						exit(1);
					}
					primes[size-1] = i;
					maxprime = i;
					rwlock_release_writelock(&myLock);
					break;
				}
		}
	}	
	// Check 'special' cases
	if (n <= 0)
		return -1;
	if (n == 1)
		return 0;

	// Check prime
	rwlock_acquire_readlock(&myLock); // Acquire read lock for accessing primes array
    	for (i = 0; i < size && root >= primes[i]; i++)
        	if (n % primes[i] == 0) {
			rwlock_release_readlock(&myLock); // Release read lock before returning
            		return 0;
       		}
    	rwlock_release_readlock(&myLock);
	return 1;
}

