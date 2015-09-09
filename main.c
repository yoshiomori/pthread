#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int avail = 0;
clock_t start_time;



void inc(void *arg){
  unsigned long int i = 0;
  time_t start_time_proc, dt;
  int status;
  start_time_proc = clock();
  dt = (time_t)arg;
  
  while(dt > clock() - start_time_proc){
    /* Consumidor: Requisitando uma CPU */
    status = pthread_mutex_lock(&mutex);
    if(status){
      printf("%s/n", strerror(status));
      exit(EXIT_FAILURE);
    }
    while(avail == 0){
      status = pthread_cond_wait(&cond, &mutex);
      if(status){
	printf("%s\n", strerror(status));
	exit(EXIT_FAILURE);
      }
    }
    avail--;
    status = pthread_mutex_unlock(&mutex);
    if(status){
      printf("%s\n", strerror(status));
      exit(EXIT_FAILURE);
    }
    
    /* Executa o processo */
    i++;

    /* Produtor: Liberando a CPU */
    status = pthread_mutex_lock(&mutex);
    if(status){
      printf("%s/n", strerror(status));
      exit(EXIT_FAILURE);
    }
    avail++;
    status = pthread_mutex_unlock(&mutex);
    if(status){
      printf("%s/n", strerror(status));
      exit(EXIT_FAILURE);
    }
    status = pthread_cond_signal(&cond);
    if(status){
      printf("%s/n", strerror(status));
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv){
  pthread_t tid;
  int status;

  /* Configurando tempo de início */
  start_time = clock();

  /* Pegando quantidade de CPU disponível no sistema */
  avail = sysconf(_SC_NPROCESSORS_ONLN);
  
  /* Fazendo outras coisas */
  status = pthread_create(&tid, NULL, (void*)inc, (void*)(2 * CLOCKS_PER_SEC));
  if(status){
    printf("%s\n", strerror(status));
    exit(EXIT_FAILURE);
  }
  status = pthread_join(tid,NULL);
  if(status){
    printf("%s\n", strerror(status));
    exit(EXIT_FAILURE);
  }
  return 0;
}
