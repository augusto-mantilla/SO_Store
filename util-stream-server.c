#define MAXLINE 512

/* Servidor do tipo socket stream.
   Manda linhas recebidas de volta para o cliente */

void str_echo(int sockfd)
{
  int n, i;
  char line[MAXLINE];

  for (;;) {

    /* LŠ uma linha do socket */

    n = readline(sockfd, line, MAXLINE);
    if (n == 0)
      return;
    else if (n < 0)
      err_dump("str_echo: readline error");

    if(strcmp(line, "start") != 0) {
      printf("Introduza no monitor a opÃ§Ã£o 'start' para iniciar a simulacao");
    }
    else {
      
    }

    /* Manda linha de volta para o socket. n conta com
       o \0 da string, caso contrßrio perdia-se sempre 
       um caracter! */
    if (writen(sockfd, line, n) != n)
      err_dump("str_echo: writen error");
  }
}

void send_message(int socket, char* message)
{
  int n = strlen(message) + 1;
  if(writen(socket, message, n) != n)
    err_dump("error sending the message\n");
}

