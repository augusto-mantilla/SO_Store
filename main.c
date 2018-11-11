#include <stdio.h>
#include <string.h>
#include "file.h"

int main(void)
{
  int reval = SUCCESS;
  char filename[30];
  char* filename2 = "file2.txt";
  FILE* fptr;
  FILE* fptr2; 

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


  if((fptr = fopen(filename, "r")) == NULL) {
    printf("Error opening %s\n", filename);
    reval = FAIL;
  }
  else if((fptr2 = fopen(filename2, "w")) == NULL) {
    printf("Error opening %s\n", filename2);
    reval = -FAIL;
  }
  else {
    LineReadWrite(fptr, fptr2, MAX_LEN);
  }
  fclose(fptr);
  fclose(fptr2);

  return reval;
}
