#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>
#include <math.h>
#include "queue.h"

#define _GNU_SOURCE
#define NP 26 //number of products 
#define MAX_CLIENTS 200 //maximum number of clients allowed in the store
#define MAX_EMPLOYEES 20 //maximum number of employees
#define RAT_MAX 10 /*o número de vezes que o número de clientes pode exeder o 
		     número de empregados*/
#define MAX_DESKS 20 //número maximo de empregados no balcão
#define MAX_SHELVES 20 //número maximo de prateleiras

typedef struct { //mantém a informação sobre os balcões de atendimento
  int desks[MAX_DESKS];
  int shelves[MAX_SHELVES];
  sem_t busy_desk;
  sem_t free_desk;
  sem_t busy_shelv;
  sem_t free_shelv;
  int nextEmp;
  int nextClient;
  int cli_gen_atte;
  int cli_pre_atte;
  int cli_gen_wait;
  int cli_pre_wait;
  int opened_desks;
  int is_open;
  Queue* withdraw;
}Store;

Store my_store;   //estrutura que contem variáveis de estado da loja
pthread_mutex_t mut_id = PTHREAD_MUTEX_INITIALIZER;     //trinco que controla a atualização do id
pthread_mutex_t mut_waitpc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_waitgc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_advancepc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_advancegc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t join = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_store; //semaforo que controla o acesso à loja
int n;           //número de clientes criados
pthread_cond_t cont = PTHREAD_COND_INITIALIZER;
int nwd = 1; //numero medio de desistencias por minuto
int att_gmax = 2; //número máximo de clientes gerais atendidos antes de passar à outra classe
int att_pmax = 5; //número máximo de clientes prioritarios atendidos antes de passar à outra clase
//recebe a posição do array e retorna o nome do produto


int trans_prod_name(int pos)
{
  return pos + 65;
}
/*
void* employee(void* store)
{
  int i, l, p, o;
  //  store* ms = (store*) store;
  while(&ms->is_open){
    sem_wait(&ms->free_desk);
    sem_wait(&ms->p_mut);    
  }


  return NULL; 
}
*/
 //isto supõe que as desistências seguem uma distribuição de poisson
int withdrawal(int id, int time, pthread_mutex_t* mut1, int* num_wait)
{
  int sval;
  int ret;
  printf("time = %d\n", time);
  double l = nwd*(time);
  int prob = (int)((l/exp(l))*100);
  int random = (rand()%100) + 1;
  //  printf("Probabilidade = %d, random = %d\n", prob, random);
  if(random <= prob){
    pthread_mutex_lock(mut1);
    printf("O cliente %d desistiu durante à espera\n", id);
    --(*num_wait);
    pthread_mutex_unlock(mut1);
    enqueue(my_store.withdraw, pthread_self());
    ret = 1;
  }
  else {
    ret = 0;
  }
  return ret;
}

void* client_gen(void* store)
{
  Store* ms = (Store*) store;
  time_t beg, end;
  time(&beg);
  int sem_val;
  int i;
  sem_wait(&sem_store);
  pthread_mutex_lock(&mut_id);
  n++;
  int id = n;
  printf("Cliente geral %d entrou na loja\n", id);
  pthread_mutex_unlock(&mut_id);
  
  pthread_mutex_lock(&mut_waitgc);
  ++ms->cli_gen_wait;
  printf("Número de clientes gerais à espera: %d\n", ms->cli_gen_wait);
  pthread_mutex_unlock(&mut_waitgc);

  time(&end);

  if(!withdrawal(id, end-beg, &mut_waitgc, &ms->cli_gen_wait))
    {
  pthread_mutex_lock(&mut_advancegc);

  printf("Cliente geral %d passa ao balcão\n", id);
  //  sleep(2);
  pthread_mutex_lock(&mut_waitgc);

  if(ms->cli_gen_wait > 0)
    --ms->cli_gen_wait;
  
  if((ms->cli_gen_atte > att_gmax  && ms->cli_pre_wait > 0) || ms->cli_gen_wait == 0) {
    ms->cli_gen_atte = 0;
    printf("Passa da fila geral para a fila com prioridade\n");
    pthread_mutex_unlock(&mut_advancepc);
  }
  else {
    ++ms->cli_gen_atte;
    pthread_mutex_unlock(&mut_advancegc);
  }
  pthread_mutex_unlock(&mut_waitgc);
    }
  else if(ms->cli_gen_wait == 0)
    {
      pthread_mutex_unlock(&mut_advancepc);
    }

  sem_post(&sem_store);

  printf("Cliente geral %d saiu da loja\n", id);
  return NULL;
}

void* client_pre(void* store)
{
  Store *ms = (Store*)store;
  int sem_val;
  int i;
  time_t beg, end;
  time(&beg);
  sem_wait(&sem_store);
  
  pthread_mutex_lock(&mut_id); //fecha trinco
  n++;
  int id = n;
  printf("Cliente preferencial %d entrou na loja\n", id);
  pthread_mutex_unlock(&mut_id);//trinco_abre
  
  pthread_mutex_lock(&mut_waitpc);
  ++ms->cli_pre_wait;
  printf("Número de clientes preferencias à espera: %d\n", ms->cli_pre_wait);
  pthread_mutex_unlock(&mut_waitpc);
  time(&end);

  if(!withdrawal(id, end-beg +1, &mut_waitpc, &ms->cli_pre_wait))
    {
      pthread_mutex_lock(&mut_advancepc);
      printf("Cliente preferecial %d passou ao balcão\n", id);
      //  sleep(2);

      pthread_mutex_lock(&mut_waitpc);
      if(ms->cli_pre_wait > 0)
	--ms->cli_pre_wait;
  
      if((ms->cli_pre_atte > att_pmax && ms->cli_gen_wait > 0) || ms->cli_pre_wait == 0) {
	ms->cli_pre_atte = 0;
	printf("Passa da fila com prioridade para a fila geral\n");
	pthread_mutex_unlock(&mut_advancegc);
      }
      else {
	++ms->cli_pre_atte;
	pthread_mutex_unlock(&mut_advancepc);
      }
      pthread_mutex_unlock(&mut_waitpc);
    }
  else if( ms->cli_pre_wait == 0)
    {
      pthread_mutex_unlock(&mut_advancegc);
    }
  printf("Cliente preferencial %d saiu da loja\n", id);
  sem_post(&sem_store);
  
  return NULL;
}

int main()
{
  sched_setscheduler(getpid(), SCHED_FIFO, NULL);
  time_t t;
  srand((unsigned)time(&t));
  
  int i,j;
  pthread_t thread_id[MAX_CLIENTS];
  my_store.withdraw = create_queue(MAX_CLIENTS);
  sem_init(&sem_store, 0, NP);
  
  for(i = 0; i < 200; i++) {
    j = rand() % 10;
    if(j >= 0 && j < 2) {
      if(pthread_create(&thread_id[i], NULL, client_pre, (void*)&my_store)
	 != 0)
	{
	  printf("erro na criação da tarefa\n");
	  exit(1);
	}
    }
    else {
      if(pthread_create(&thread_id[i], NULL, client_gen, (void*)&my_store)
	 != 0)
	{
	  printf("erro na criação da tarefa\n");
	  exit(1);
	}
    }
  }

  for(i = 0; i < 200; i++) {
    if(!isin(my_store.withdraw, thread_id[i])){
      pthread_join(thread_id[i], NULL);
    }
  }
  sem_destroy(&sem_store);
  return 0;
}
