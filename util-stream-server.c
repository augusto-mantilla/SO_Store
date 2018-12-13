#define MAXLINE 512

/* Servidor do tipo socket stream.
   Manda linhas recebidas de volta para o cliente */

void str_echo(sockfd)
int sockfd;
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

    /*
    line[0] = '1';
    line[1] = '1';
    line[2] = '2';
    line[3] = '-';
    line[4] = '2';
    line[5] = '5';
    line[6] = '5';
    line[7] = '-';
    line[8] = '3';
    line[9] = '8';
    line[10] = '3';
    line[11] = '-';
    line[12] = '3';
    line[13] = '2';
    line[14] = '3';
    n = 15;*/

    /* Manda linha de volta para o socket. n conta com
       o \0 da string, caso contrßrio perdia-se sempre 
       um caracter! */
    if (writen(sockfd, line, n) != n)
      err_dump("str_echo: writen error");
  }
}
