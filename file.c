#include <stdio.h>
#include <string.h>
#include "file.h"

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
