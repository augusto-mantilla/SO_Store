#ifndef FILE_H
#define FILE_H

enum {SUCCESS, FAIL, MAX_LEN = 80};

void fgetCleanS(char* str, int length, FILE* input);
void LineReadWrite(FILE* fin, FILE* fout, int max_len);
void CharReadWrite(FILE* fin, FILE* fout);
void StdWritetoFile(FILE* fout);

#endif
