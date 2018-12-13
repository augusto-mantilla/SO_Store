#include <stdio.h>
#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#define MAXLINE 512

/* Cliente do tipo socket stream.
   LŠ string de fp e envia para sockfd.
   LŠ string de sockfd e envia para stdout */

void str_cli(fp, sockfd)
FILE *fp;
int sockfd;
{
  int n;
  char sendline[MAXLINE], recvline[MAXLINE+1];

  while (fgets(sendline, MAXLINE, fp) != NULL) {

    // ########################################
    
    char** client = malloc(sizeof sendline);
    *(client) = strtok(sendline, "-");
    int i = 0;
  
    while(*(client + i)) {
      i += 1;
      *(client + i) = strtok(NULL, "-");
      }
    
    printf("Client com id %s entrou na loja\n", *(client));
    printf("Client com id %s fez um pedido\n", *(client + 1));
    printf("Client com id %s recebeu o pedido\n", *(client + 2));
    printf("Client com id %s desistiu\n", *(client + 3));
    free(client);
    // ########################################

    /* Envia string para sockfd. Note-se que o \0 nƒo 
       ‰ enviado */

    n = strlen(sendline);
    if (writen(sockfd, sendline, n) != n)
      err_dump("str_cli: writen error on socket");

    /* Tenta ler string de sockfd. Note-se que tem de 
       terminar a string com \0 */

    n = readline(sockfd, recvline, MAXLINE);
    if (n<0)
      err_dump("str_cli:readline error");
    recvline[n] = 0;

    /* Envia a string para stdout */
    
    
    
    
    fputs(recvline, stdout);
  }
  if (ferror(fp))
    err_dump("str_cli: error reading file");
}
