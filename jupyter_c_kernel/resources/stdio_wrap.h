#ifndef STDIO_WRAP_H
#define STDIO_WRAP_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Need input buffer to know whether we need another input request */
char inputBuff[1<<10] = "";

/* read remaining input into buffer so it can be used in next call */
void readIntoBuffer() {
  long length = strlen(inputBuff);
  char nextChar = 0;
    while((nextChar = getchar()) != '\n'){
      inputBuff[length++] = nextChar;
    }
    inputBuff[length++] = '\n';
    inputBuff[length] = '\0';
}

/* Define the functions to overload the old ones */
int scanf_wrap(const char *format, ...) {
  /* unget chars in buffer */
  int doRequest = 1;
  long index = strlen(inputBuff) - 1;
  for(; index >= 0; --index) {
    ungetc(inputBuff[index], stdin);
    /* if there already is a newline in buffer
      we need no input request */
    if(inputBuff[index] == '\n') {
      doRequest = 0;
    }
  }
  /* Buffer will always be empty after scanf call
    since there is no way to enter more than one
    newline in the frontend */
  inputBuff[0] = '\0';
  // printf("doRequest: %d\n", doRequest);
  if(doRequest) {
    printf("<inputRequest>");
    fflush(stdout);
  }
  va_list arglist;
  va_start( arglist, format );
  int result = vscanf( format, arglist );
  va_end( arglist );

  return result;
}

int getchar_wrap(){
  /* check if there is still something in the input buffer*/
  char input = 0;
  long length = strlen(inputBuff);
  if(length <= 0) {
    printf("<inputRequest>");
    fflush(stdout);

    input = getchar();
    readIntoBuffer();
  } else {
    input = inputBuff[0];
    /* shift all chars one to the left */
    int i = 1;
    for(; i < 100; ++i){
      inputBuff[i-1] = inputBuff[i];
      if(inputBuff[i] == '\0') {
        break;
      }
    }
  }

  return input;
}

/* Replace all the necessary input functions
  Need double hashes in case there are no __VA_ARGS__*/
#define scanf(format, ...) scanf_wrap(format, ##__VA_ARGS__)

#define getchar() getchar_wrap()

/* Replace FILE write operations for read-only systems */
#ifdef READ_ONLY_FILE_SYSTEM

/* Define wrapping functions */
/* Output that the fopen succeeded and return some valid pointer */
FILE *fopen_wrap(const char *filename, const char *modes) {
  static long stream = 0x1FFFF0000;
#ifdef SHOW_FILE_IO_VERBOSE
  printf("\x01b[42m");
  printf("\"%s\" opened in mode \"%s\"\n", filename, modes);
  printf("\x01b[0m");
#endif /* SHOW_FILE_IO_VERBOSE */
  return (FILE*)stream++;
}

int fprintf_wrap(FILE* stream, const char* format, ...) {
  printf("\x01b[42m");
  printf("%p:", stream);
  printf("\x01b[0m");
  va_list arglist;
  va_start( arglist, format );
  int result = vprintf(format, arglist);
  va_end( arglist );
  return result;
}

/* Replace all the necessary input functions */
#define fopen(file, mode) fopen_wrap(file, mode)

#define fprintf(stream, format, ...) fprintf_wrap(stream, format, ##__VA_ARGS__)

#endif  /* READ_ONLY_FILE_SYSTEM */

#endif /* STDIO_WRAP_H */
