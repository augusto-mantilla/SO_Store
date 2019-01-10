#include <stdio.h>

#ifndef FILE_H
#define FILE_H

enum {SUCCESS, FAIL, MAX_LEN = 80};

int str_to_int(char* str);
void fgetCleanS(char* str, int length, FILE* input);
void LineReadWrite(FILE* fin, FILE* fout, int max_len);
void CharReadWrite(FILE* fin, FILE* fout);
void StdWritetoFile(FILE* fout);
char** str_split(char* a_str, const char a_delim);

#endif
