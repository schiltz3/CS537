#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

/*-Macros----------------------------------------------------*/
#define thread_count 10

/*-Struct Defenitions----------------------------------------*/
struct thread_arg_s
{
  int *counter;
  pthread_mutex_t *count_lock;
  int thread_limit;
  int thread_num;
};

/*-Global Variables------------------------------------------*/
// Create a global counter
int counter = 0;
pthread_mutex_t count_lock;

/*-Function Prototypes---------------------------------------*/
void *increment_counter(void *vargp);

int main()
{
  struct thread_arg_s thread_args[thread_count];

  // Source 1: https://www.geeksforgeeks.org/multithreading-c-2/
  // Source 2: man pthread_create
  // Source 3: man pthread_join
  // Source 4: https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/

  pthread_t thread_id[thread_count];
  for (int thread_index = 0; thread_index < thread_count; thread_index++)
  {
    thread_args[thread_index].counter = &counter;
    thread_args[thread_index].count_lock = &count_lock;
    thread_args[thread_index].thread_limit = 10000 / thread_count;
    thread_args[thread_index].thread_num = thread_index;
  }

  for (int thread_index = 0; thread_index < thread_count; thread_index++)
  {
    pthread_create(&thread_id[thread_index], NULL, increment_counter, (void *)&thread_args[thread_index]);
  }

  for (int thread_index = 0; thread_index < thread_count; thread_index++)
  {
    pthread_join(thread_id[thread_index], NULL);
  }

  printf("Counter final value: %d\n", counter);

  return (0);
}

void *increment_counter(void *vargp)
{
  struct thread_arg_s args = *((struct thread_arg_s *)vargp);
  pthread_mutex_lock(args.count_lock);

  // printf("Thread ID: %d\tCurrent Count: %d\n", args.thread_num, *args.counter);

  for (int i = 0; i < args.thread_limit; i++)
  {
    ++*args.counter;
  }

  pthread_mutex_unlock(args.count_lock);
  return NULL;
}