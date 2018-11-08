#include <stdio.h>
#include <string.h>
#include "file.h"

int main(void)
{
  int reval = SUCCESS;
  int length = 30;
  char filename[30];
  char input[60];
  
  FILE* fptr;
  char c;
  
  printf("Please enter the name of the file you want to create\n");
  fgetCleanS(filename, sizeof filename, stdin);
  
  if((fptr = fopen(filename, "w")) == NULL) {
    printf("Error creating the file.\n");
    reval = FAIL;
  }
  else {
    printf("Please enter the string you want to put in the file\n");
    StdWritetoFile(fptr);
  }
  fclose(fptr);

  char* filename2 = "file2.txt";
  FILE* fptr2; 
  if((fptr = fopen(filename, "r")) == NULL) {
    printf("Error opening %s\n", filename);
  }
  else if((fptr2 = fopen(filename2, "w")) == NULL) {
    printf("Error opening %s\n", filename2);
  }
  else {
    LineReadWrite(fptr, fptr2, MAX_LEN);  
  }
  fclose(fptr);
  fclose(fptr2);
  return reval;
}


/*  Exercise 1
int reval = SUCCESS;
  char filename[] = "text.txt";
  FILE* fptr;
  char c;
  int counter = 0;
  if((fptr = fopen(filename, "r")) == NULL) {
    printf("Error openning the file.\n");
    reval = FAIL;
  }
  else {
    while((c = fgetc(fptr)) != EOF ) {
      counter++;
      putchar(c);
    }
  }
  fclose(fptr);
  printf("The document has %d characters.\n", counter);
  return reval;*/
