#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>

#define _GNU_SOURCE
#define NP 26 //number of products 
#define MAX_CLIENTS 200 //maximum number of clients allowed in the store
#define MAX_EMPLOYEES 20 //maximum number of employees
#define RAT_MAX 10 /*o número de vezes que o número de clientes pode exeder o 
		     número de empregados*/
#define MAX_DESKS 20 //número maximo de empregados no balcão
#define MAX_SHELVES 20 //número maximo de prateleiras

typedef struct { //mantém a informação sobre os balcões de atendimento
  
  int n; //número de clientes criados
  
  sem_t store_sem;
  int desks[MAX_DESKS];
  int shelves[MAX_SHELVES];
  sem_t busy_desk;
  sem_t free_desk;
  sem_t busy_shelv;
  sem_t free_shelv;
  sem_t advance_to_desk_p;
  sem_t advance_to_desk_g;
  int nextEmp;
  int nextClient;
  sem_t pmut;
  sem_t cmut;
  sem_t updt_waiting_p;
  sem_t updt_waiting_g;
  int cli_gen_atte;
  int cli_pre_atte;
  int cli_gen_wait;
  int cli_pre_wait;
  int opened_desks;
  int is_open;
  sem_t sem_esp;
}Store;

Store my_store;
sem_t sem_id;
sem_t sem_store;
int n;
pthread_cond_t cont = PTHREAD_COND_INITIALIZER;

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
void* client_gen(void* store)
{
  Store* ms = (Store*) store;

  int sem_val;
  int i;
  sem_wait(&sem_store);
  sem_wait(&sem_id);
  n++;
  int id = n;
  printf("Cliente geral %d entrou na loja\n", id);
  sem_post(&sem_id);
  
  sem_wait(&ms->updt_waiting_g);
  ++ms->cli_gen_wait;
  printf("Número de clientes gerais à espera: %d\n", ms->cli_gen_wait);
  sem_post(&ms->updt_waiting_g);
    
  //espera se tem clientes preferencias à espera
  sem_wait(&ms->advance_to_desk_g);
  printf("Cliente geral %d passa ao balcão\n", id);
  
  sem_wait(&ms->updt_waiting_g);
  --ms->cli_gen_wait;
  if((ms->cli_gen_atte > 2 && ms->cli_pre_wait > 0) || ms->cli_gen_wait == 0) {
    ms->cli_gen_atte = 0;
    printf("Passa da fila geral para a fila com prioridade\n");
    sem_post(&ms->advance_to_desk_p);
  }
  else {
    ++ms->cli_gen_atte;
    sem_post(&ms->advance_to_desk_g);
  }
  sem_post(&ms->updt_waiting_g);
  sem_post(&sem_store);

  printf("Cliente geral %d saiu da loja\n", id);
  return NULL;
}

void* client_pre(void* store)
{
  Store *ms = (Store*)store;
  int sem_val;
  int i;
  
  sem_wait(&sem_store);
  
  sem_wait(&sem_id);//trinco_fecha
  n++;
  int id = n;
  printf("Cliente preferencial %d entrou na loja\n", id);
  sem_post(&sem_id);//trinco_abre
  
  sem_wait(&ms->updt_waiting_p);
  ++ms->cli_pre_wait;
  printf("Número de clientes preferencias à espera: %d\n", ms->cli_pre_wait);
  sem_post(&ms->updt_waiting_p);

  sem_wait(&ms->advance_to_desk_p);
  printf("Cliente preferecial %d passou ao balcão\n", id);
  sleep(2);
  sem_wait(&ms->updt_waiting_p);
  --ms->cli_pre_wait;
  if((ms->cli_pre_atte > 5 && ms->cli_gen_wait > 0) || ms->cli_pre_wait == 0) {
    ms->cli_gen_atte = 0;
    printf("Passa da fila com prioridade para a fila geral\n");
    sem_post(&ms->advance_to_desk_g);
  }
  else {
    ++ms->cli_gen_atte;
    sem_post(&ms->advance_to_desk_p);
  }
  sem_post(&ms->updt_waiting_p);
  
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
  pthread_t thread_id[200];
  
  sem_init(&sem_id, 0, 1);
  sem_init(&sem_store, 0, NP);
  sem_init(&my_store.sem_esp, 0, 1);
  sem_init(&my_store.updt_waiting_p, 0, 1);
  sem_init(&my_store.updt_waiting_g, 0, 1);
  sem_init(&my_store.advance_to_desk_g, 0, 1);
  sem_init(&my_store.advance_to_desk_g, 0, 1);
  for(i = 0; i < 200; i++) {
    j = rand() % 10;
    if(j >= 0 && j < 5) {
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
  for(i = 0; i < 200; i++)
    pthread_join(thread_id[i], NULL);

  return 0;
}
