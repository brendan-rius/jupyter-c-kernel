#ifndef STDIO_WRAP_H
#define STDIO_WRAP_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Figure out used C standard.
  __STDC_VERSION__ is not always defined until C99.
  If it is not defined, set standard to C89.
  It is safest to set it by hand, to make sure */
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ <= 199409L
#define C89_SUPPORT
#endif /* __STDC_VERSION__ <= 199409L */
#else /* __STDC_VERSION__ */
#define C89_SUPPORT
#endif /* __STDC_VERSION__ */

/* Need input buffer to know whether we need another input request */
static char inputBuff[1<<10] = "";

/* read remaining input into buffer so it can be used in next call */
void readIntoBuffer() {
  long length = strlen(inputBuff);
  char nextChar = 0;
  while((nextChar = getchar()) != '\n' && nextChar != EOF){
    inputBuff[length++] = nextChar;
  }
  inputBuff[length++] = '\n';
  inputBuff[length] = '\0';
}

/* check whether input request is needed */
char checkInputRequest() {
  /* unget chars in buffer */
  char doRequest = 1;
  char leadingNewline = 1;
  long index = strlen(inputBuff) - 1;
  for(; index >= 0; --index) {
    ungetc(inputBuff[index], stdin);
    /* if there already is a newline in buffer
      we need no input request */
    if(inputBuff[index] == '\n') {
      if(!leadingNewline){
        doRequest = 0;
      }
    } else {
      leadingNewline = 0;
    }
  }

  return doRequest;
}

/* Define the functions to overload the old ones */

/* Wrapping of scanf depends on standard */
#ifdef C89_SUPPORT
/* Need to define vscanf for c89.
  TODO: This is a bit risky, since the underlying glibc does not
  have to include this if it is old. If it does not, linking will fail.
  The only safe way is reimplementing the whole function. */

/* Read formatted input from stdin into argument list ARG.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int vscanf (const char *__restrict __format, _G_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) __wur;
#endif /* C89_SUPPORT */

int scanf_wrap(const char *format, ...) {
  char doRequest = checkInputRequest();

  if(doRequest) {
    printf("<inputRequest>");
    fflush(stdout);
  }

  {
    va_list arglist;
    int result;
    va_start( arglist, format );
    result = vscanf( format, arglist );
    va_end( arglist );

    /* Put the remaining input into the input buffer */
    readIntoBuffer();

    return result;
  }
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
    int i = 1;

    input = inputBuff[0];
    /* shift all chars one to the left */
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
  depending on the language version used */
#ifndef C89_SUPPORT
/* Need double hashes in case there are no __VA_ARGS__*/
#define scanf(format, ...) scanf_wrap(format, ##__VA_ARGS__)
#else /* C89_SUPPORT */
/* Since there are no variadic macros in C89, this is the only way
  although it is horrible */
#define scanf scanf_wrap
#endif /* C89_SUPPORT */

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
