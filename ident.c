#include <stdio.h>
#include <pthread.h>

pthread_t io_thread;

void * io_routine(void *notused)
{
  pthread_t thread;
  thread = pthread_self();
//  if (io_thread == thread)  // struct 比較なのでこの手法ではなく pthread_equal 使うほうがよい
  if (pthread_equal(io_thread, thread))
    printf("hey it is me!\n");
  else
    printf("thats\' not me!\n");
  
  return NULL;
}

int main(int argc, char *argv[]) {
  extern pthread_t io_thread;
  pthread_create(&io_thread, NULL, io_routine, NULL);
  pthread_join(io_thread, NULL);
}