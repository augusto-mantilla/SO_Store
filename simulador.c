#include <syscall.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sched.h>
#include <sys/types.h>
#include <math.h>
#include "queue.h"
#include "file.h"
#include "unix.h"
/* Servidor do tipo socket stream.
   Recebe linhas do cliente e reenvia-as para o cliente */
#define _GNU_SOURCE

#define NP 26 //number of products 
#define MAX_CLIENTS 20000 //maximum number of clients allowed in the store
#define MAX_EMPLOYEES 20 //maximum number of employees
#define RAT_MAX 10 /*o numero de vezes que o numero de clientes pode exeder o 
		     numero de empregados*/
#define PARAMETERS 4 //numero de parametros que aparecem no ficheiro de configuracao
#define MAX_DESKS 20 //numero maximo de empregados no balcao
#define MAX_SHELVES 20 //numero maximo de prateleiras
int product[NP]; /*letters from A to Z, the position is the type of product and 
		  each position contains the number of products from that type*/

typedef struct { //mantem a informacao sobre os balcoes de atendimento
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

Store my_store;   //estrutura que contem variaveis de estado da loja
pthread_mutex_t mut_id = PTHREAD_MUTEX_INITIALIZER;     //trinco que controla a atualização do id
pthread_mutex_t mut_waitpc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_waitgc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_advancepc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_advancegc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t join = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_store; //semaforo que controla o acesso a loja
int n;           //numero de clientes criados
pthread_cond_t cont = PTHREAD_COND_INITIALIZER;
int nwd = 1; //numero medio de desistencias por minuto
int att_gmax = 2; //numero maximo de clientes gerais atendidos antes de passar a outra classe
int att_pmax = 5; //numero maximo de clientes prioritarios atendidos antes de passar a outra clase
pthread_mutex_t wd = PTHREAD_MUTEX_INITIALIZER;
unsigned int average_clients;
unsigned int service_time;
unsigned int withdrawal_prob;
unsigned int simulation_time;
int sockfd, newsockfd, clilen, childpid, servlen;
struct sockaddr_un cli_addr, serv_addr;

//recebe a posicao do array e retorna o nome do produto
int trans_prod_name(int pos)
{
  return pos + 65;
}

int withdrawal(int id, int time, pthread_mutex_t* mut1, int* num_wait)
{
  int sval;
  int ret;
  //printf("time = %d\n", time);
  double l = nwd*(time);
  int prob = (int)((l/exp(l))*100);
  int random = (rand()%100) + 1;
  //  printf("Probabilidade = %d, random = %d\n", prob, random);
  if(random <= prob){
    pthread_mutex_lock(mut1);
    printf("O cliente %d desistiu durante a espera\n", id);
    --(*num_wait);
    pthread_mutex_unlock(mut1);
    pthread_mutex_lock(&wd);
    enqueue(my_store.withdraw, id);
    pthread_mutex_unlock(&wd);
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
  char* message = "12-32-34-23";
  sem_wait(&sem_store);
  pthread_mutex_lock(&mut_id);
  n++;
  int id = n;
  printf("Cliente geral %d entrou na loja\n", id);
  pthread_mutex_unlock(&mut_id);
  
  pthread_mutex_lock(&mut_waitgc);
  ++ms->cli_gen_wait;
  printf("Numero de clientes gerais a espera: %d\n", ms->cli_gen_wait);
  pthread_mutex_unlock(&mut_waitgc);
  //send_message(sockfd, message);
  time(&end);

  if(!withdrawal(id, end-beg, &mut_waitgc, &ms->cli_gen_wait)){
    pthread_mutex_lock(&mut_advancegc);

    printf("Cliente geral %d passa ao balcao\n", id);
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
  char* message = "12-32-34-23";
  time(&beg);
  sem_wait(&sem_store);
  
  pthread_mutex_lock(&mut_id); //fecha trinco
  n++;
  int id = n;
  printf("Cliente preferencial %d entrou na loja\n", id);
  pthread_mutex_unlock(&mut_id);//trinco_abre
  
  pthread_mutex_lock(&mut_waitpc);
  ++ms->cli_pre_wait;
  printf("Numero de clientes preferencias a espera: %d\n", ms->cli_pre_wait);
  pthread_mutex_unlock(&mut_waitpc);
  time(&end);
  //  send_message(sockfd, message);
  if(!withdrawal(id, end-beg +1, &mut_waitpc, &ms->cli_pre_wait))
    {
      pthread_mutex_lock(&mut_advancepc);
      printf("Cliente preferecial %d passou ao balcao\n", id);
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

print_error(char* description) {
  printf("%s\n", description);
  exit(1);
}
int main(int argc, char* argv[])
{	
  time_t beg_sim, this_inst;
  time(&beg_sim);
  pthread_t thread_id[MAX_CLIENTS];
  my_store.withdraw = create_queue(MAX_CLIENTS);
  sem_init(&sem_store, 0, NP);
  /* Cria socket stream */

  if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
    err_dump("server: can't open stream socket");

  /* Primeiro uma limpeza preventiva!
     Dados para o socket stream: tipo + nome do ficheiro.
     O ficheiro serve para que os clientes possam identificar o servidor */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;
  strcpy(serv_addr.sun_path, UNIXSTR_PATH);

  /* O servidor  quem cria o ficheiro que identifica o socket.
     Elimina o ficheiro, para o caso de algo ter ficado pendurado.
     Em seguida associa o socket ao ficheiro. 
     A dimensao o a indicar ao bind no  a da estrutura, pois depende
     do nome do ficheiro */

  servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
  unlink(UNIXSTR_PATH);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
    err_dump("server, can't bind local address");

  /* Servidor pronto a aceitar 5 clientes para o socket stream */
  printf("A espera da ligação do monitor");
  listen(sockfd, 1);

  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
		       &clilen);
  if (newsockfd < 0)
    err_dump("server: accept error");

  printf("Number of arguments = %d\n", argc);
  if(argc > 1 && argc <= 4) {
    char* filename = (char*)malloc(sizeof(char)*strlen(argv[1] + 1));
    
    if(filename != NULL) {
      //      printf("in if filename != null\n");
      strcpy(filename, argv[1]);
      //      printf("filename = %s\n", filename);
      FILE* file = fopen(filename, "r");
      if(file != NULL) {
	//printf("Opened the file\n");
	int i = 0;
	char string[50];
	char* variable_name[PARAMETERS];
	int values[PARAMETERS];
	while(!feof(file) && i < PARAMETERS) {
	  fscanf(file, "%s\n", string);
	  //printf("string = %s\n", string);
	  char** token = str_split(string, ':');
	  //printf("string1 = %s, string2 = %s\n", token[0], token[1]);
	  variable_name[i] = token[0];
	  values[i] = str_to_int(token[1]);
	  i++;
	}

	if(strcmp(variable_name[0], "average_clients") == 0)
	  average_clients = values[0];
	else
	  print_error("Erro de formatação do ficheiro de configuração");

	if(strcmp(variable_name[1], "service_time") == 0)
	  service_time = values[1];
	else
	  print_error("Erro de formatação do ficheiro de configuração");

	if(strcmp(variable_name[2], "withdrawal_prob") == 0)
	  withdrawal_prob = values[2];
	else
	  print_error("Erro de formatação do ficheiro de configuração");
	
	if(strcmp(variable_name[3], "simulation_time") == 0)
	  simulation_time = values[3];
	else
	  print_error("Erro de formatação do ficheiro de configuração");
	
	fclose(file);
      }
      else {
	printf("Cannot open the file\n");
      }
      if(argc == 4) {
	if(strcmp(argv[2], "-t") == 0) {
	  simulation_time = str_to_int(argv[3]);
	  printf("Time = %d\n", simulation_time);
	}
      }
    }
  }
  else {
    char* filename = "simulacao.conf";
    FILE* file;
    printf("Introduza o numero medio de clientes que chegam por minuto\n");
    scan_int(&average_clients);
    printf("Introduza o tempo medio de atendimento\n");
    scan_int(&service_time);
    printf("Introduza a probabilidade de desistencia dos clientes (de 0 a 100)\n");
    scan_int(&withdrawal_prob);
    printf("Introduza o tempo de simulacao\n");
    scan_int(&simulation_time);
    printf("Values: %d, %d, %d, %d\n", average_clients, service_time,
	   withdrawal_prob, simulation_time);
    if((file = fopen(filename, "w")) == NULL) {
      printf("Error openning the file\n");
      exit(0);
    }
    else {
      fprintf(file, "average_clients:%d\n", average_clients);
      fprintf(file, "service_time:%d\n", service_time);
      fprintf(file, "withdrawal_prob:%d\n", withdrawal_prob);
      fprintf(file, "simulation_time:%d\n", simulation_time);
      fclose(file);
    }
  }
  int num_client_threads = 0;
  time(&this_inst);
  while(num_client_threads < MAX_CLIENTS &&
	(this_inst - beg_sim) > simulation_time ) {
    int j = rand() % 10;
    if(j >= 0 && j < 2) {
      if(pthread_create(&thread_id[num_client_threads], NULL, client_pre, (void*)&my_store)
	 != 0)
	{
	  printf("erro na criação da tarefa\n");
	  exit(1);
	}
    }
    else {
      if(pthread_create(&thread_id[num_client_threads], NULL, client_gen, (void*)&my_store)
	 != 0)
	{
	  printf("erro na criação da tarefa\n");
	  exit(1);
	}
    }
    num_client_threads++;
  }

  for(int j = 0; j < num_client_threads; j++) {
    if(!isin(my_store.withdraw, thread_id[j])){
      pthread_join(thread_id[j], NULL);
    }
  }
  sem_destroy(&sem_store);
  //  print_queue(my_store.withdraw);
  //  printf("length of the queue = %d\n", queue_length(my_store.withdraw));

  close(newsockfd);
    
  return 0;
}
