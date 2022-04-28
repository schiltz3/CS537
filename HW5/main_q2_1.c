// Source 1: https://www.geeksforgeeks.org/multithreading-c-2/
// Source 2: man pthread_create
// Source 3: man pthread_join
// Source 4: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
// Source 5: man aoi

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

/*-Macros----------------------------------------------------*/
#define RED "\x1B[0;31m"
#define NC "\x1B[0m"

/*-Struct Defenitions----------------------------------------*/

/*-Global Variables------------------------------------------*/
// Create a global counter
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t *cond = NULL;
volatile int count = 0;
int thread_count = 0;

/*-Function Prototypes---------------------------------------*/
void *increment_counter(void *vargp);

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf(RED "Missing required argument\n" NC);
    printf(RED "main [number of threads]\n" NC);
    exit(EXIT_FAILURE);
  }
  if (argc > 2)
  {
    printf(RED "Too many arguments\n" NC);
    printf(RED "main [number of threads]\n" NC);
    exit(EXIT_FAILURE);
  }

  thread_count = (int)atoi(argv[1]);
  printf("Number of threads: %d\n", thread_count);

  int *thread_args = malloc(sizeof(int) * thread_count);
  cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t) * thread_count);
  pthread_t *thread_id = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);

  for (int thread_index = 0; thread_index < thread_count; thread_index++)
  {
    if (pthread_cond_init(&cond[thread_index], NULL) != 0)
    {
      printf(RED "Pthread cond int error" NC);
    }
  }

  for (int thread_index = 0; thread_index < thread_count; thread_index++)
  {
    printf("Made thread: %d\n", thread_index);
    thread_args[thread_index] = thread_index;
    printf("Arg:%d\n", thread_args[thread_index]);
    pthread_create(&thread_id[thread_index], NULL, increment_counter, (void *)&thread_args[thread_index]);
  }

  printf("Waiting on threads\n");
  for (int thread_index = 0; thread_index < thread_count; thread_index++)
  {
    pthread_join(thread_id[thread_index], NULL);
  }

  return (0);
}

void *increment_counter(void *vargp)
{
  int thread_turn = *(int *)vargp;
  while (true)
  {
    pthread_mutex_lock(&count_lock);

    if (thread_turn != count)
    {
      // turn of synch, accidentally wrote the correct code first
      // pthread_cond_wait(&cond[thread_turn], &count_lock);
    }

    printf("%d, ", thread_turn + 1);

    if (count < thread_count - 1)
    {
      ++count;
    }
    else
    {
      count = 0;
    }

    pthread_cond_signal(&cond[count]);
    pthread_mutex_unlock(&count_lock);
  }
  return NULL;
}