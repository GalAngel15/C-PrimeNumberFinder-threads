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

void parseargs(char *argv[], int argc, int *lval, int *uval, int *n, int *t);
int isprime(int n);
void *mythread(void *arg);
pthread_mutex_t lock= PTHREAD_MUTEX_INITIALIZER;
int count=0;
int num=0;
char *flagarr = NULL;

typedef struct {
	int lval; 
	int uval;
} myarg_t;

int main (int argc, char **argv)
{
	int lval = 1;
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
	printf("Found %d primes%c\n", count, count ? ':' : '.');
	for (num = lval; num <= uval && nval>0; num++)
		if (flagarr[num - lval])
		{
			nval--;
			count--;
			printf("%d%c", num, count&&nval ? ',' : '\n');  
		}
	return 0;
}

void *mythread(void *arg)
{
	// Set flagarr
	myarg_t *args=(myarg_t*)arg;
	while(num<= args->uval)
	{
		pthread_mutex_lock(&lock);
		num++;
		pthread_mutex_unlock(&lock);
		if(num<=args->uval)
		{
			if (isprime(num))
			{
				flagarr[num - args->lval] = 1; 
				count ++;
			} else {
				flagarr[num - args->lval] = 0; 
			}
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
  pthread_mutex_lock(&lock);
  if (primes == NULL)
  {
    primes = (int *)malloc(2*sizeof(int));
    if (primes == NULL)
      exit(1);
    size = 2;
    primes[0] = 2;
    primes[1] = 3;
    maxprime = 3;
  }
  pthread_mutex_unlock(&lock);
  root = (int)(sqrt(n));

  // Update primes array, if needed
  while (root > maxprime)
    for (i = maxprime + 2 ;  ; i+=2)
      if (isprime(i))
      {
        pthread_mutex_lock(&lock);
        size++;
        primes = (int *)realloc(primes, size * sizeof(int));
        pthread_mutex_unlock(&lock);
        if (primes == NULL)
          exit(1);
        primes[size-1] = i;
        maxprime = i;
        break;
      }

  // Check 'special' cases
  if (n <= 0)
    return -1;
  if (n == 1)
    return 0;

  // Check prime
  for (i = 0 ; ((i < size) && (root >= primes[i])) ; i++)
    if ((n % primes[i]) == 0)
      return 0;
  return 1;
  }

