#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "file.h"

//converte uma string numérica num inteiro
int str_to_int(char* str)
{
  int n = strlen(str);
  int dig;
  int val;
  int total = 0;
  int count = 0;
  for(int i = 0; i < n; i++) {
    if(str[i] >= '0' && str[i] <= '9') {
      count++;
    }
    else
      break;
  }
  for(int i = 0; i < count; i++) {
      dig = (int)str[i] - 48;
      val = dig*pow(10, count - 1 - i);
      total += val;
  }
  return total;
}

//le um valor inteiro da consola
void scan_int(unsigned int* d)
{
  if(scanf("%u", d)) {
    printf("Introduziu o valor: %u\n", *d);
  }
  else {
    printf("Format error: Introduza um número inteiro\n");
  } 
}
//separa uma string em varias delimitadas por a_delim
char** str_split(char* a_str, const char a_delim)
{
  char** result    = 0;
  size_t count     = 0;
  char* tmp        = a_str;
  char* last_comma = 0;
  char delim[2];
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp)
    {
      if (a_delim == *tmp)
	{
	  count++;
	  last_comma = tmp;
	}
      tmp++;
    }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;

  result = (char**)malloc(sizeof(char*) * count);
  
  if (result)
    {
      size_t idx  = 0;
      char* token = strtok(a_str, delim);

      while (token)
	{
	  assert(idx < count);
	  *(result + idx++) = strdup(token);
	  token = strtok(0, delim);
	}
      assert(idx == count - 1);
      *(result + idx) = 0;
    }
  
  return result;
}

void fgetCleanS(char* str, int length, FILE* input)
{
  fgets(str, length, input);
  if(str[(length = strlen(str) - 1)] == '\n')
    str[length] = '\0';
  else
    while(getchar() != '\n');
}
//Don't use with stdin 
void LineReadWrite(FILE* fin, FILE* fout, int max_len)
{
  char buff[max_len];
  while(fgets(buff, max_len, fin) != NULL) {
    fputs(buff, fout);
  }
}

void CharReadWrite(FILE* fin, FILE* fout)
{
  char c;

  while((c = fgetc(fin)) != EOF)
    fputc(c, fout);
}

void StdWritetoFile(FILE* fout)
{
  char c;
  while((c = getchar()) != '\n')
    fputc(c, fout);
  fputc('\n', fout);
}
