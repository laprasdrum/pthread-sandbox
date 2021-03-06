#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

void *do_one_thing(void *);
void *do_another_thing(void *);
void do_wrap_up(int, int);

int r1 = 0, r2 = 0, r3 = 0;
pthread_mutex_t r3_mutex=PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
  pthread_t thread1, thread2;
  
  if (argc > 1)
    r3 = atoi(argv[1]);
  else
    r3 = 3;

  if (pthread_create(&thread1, NULL, do_one_thing, (void *) &r1) != 0)
    perror("pthread_create"), exit(1);
  
  if (pthread_create(&thread2, NULL, do_another_thing, (void *) &r2) != 0)
    perror("pthread_create"), exit(1);
  
  if (pthread_join(thread1, NULL) != 0)
    perror("pthread_join"), exit(1);
  
  if (pthread_join(thread2, NULL) != 0)
    perror("pthread_join"), exit(1);
  
  do_wrap_up(r1, r2);
  
  return 0;
}

void *do_one_thing(void *pnum_times)
{
  int i, j, x;
  
  pthread_mutex_lock(&r3_mutex);
  if (r3 > 0) {
    x = r3;
    r3--;
  } else {
    x = 1;
  }
  printf("r3: %d\n", r3);
  pthread_mutex_unlock(&r3_mutex);
  
  for (i = 0;  i < 4; i++) {
    printf("doing one thing\n"); 
    for (j = 0; j < 10000; j++) x = x + i;
    (*(int *)pnum_times)++;
  }
  
  return(NULL);
}

void *do_another_thing(void *pnum_times)
{
  int i, j, x;
  
  pthread_mutex_lock(&r3_mutex);
  if (r3 > 0) {
    x = r3;
    r3--;
  } else {
    x = 1;
  }
  printf("r3: %d\n", r3);
  pthread_mutex_unlock(&r3_mutex);
  
  for (i = 0;  i < 4; i++) {
    printf("doing another \n"); 
    for (j = 0; j < 10000; j++) x = x + i;
    (*(int *)pnum_times)++;
  }
  
  return(NULL);
}

void do_wrap_up(int one_times, int another_times)
{
  int total;
  
  total = one_times + another_times;
  printf("All done, one thing %d, another %d for a total of %d\n",
    one_times, another_times, total);
}
