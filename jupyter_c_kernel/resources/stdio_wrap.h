#ifndef STDIO_WRAP_H
#define STDIO_WRAP_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

/* output functions to replicate terminal behaviour */
#ifdef BUFFERED_OUTPUT
/* buffer for all output */
/* TODO allocate this dynamically */
static char outputBuff[1<<10] = "";
static char attachedOutputFlush = 0;

void flush_all_output() {
  printf("%s", outputBuff);
  fflush(stdout);
  outputBuff[0] = '\0';
}

/* Flush all output on exit */
void attachOutputFlush() {
  if(attachedOutputFlush == 0){
    int error = atexit(flush_all_output);
    if(error != 0) {
      fprintf(stderr, "ERROR: Could not set exit function! Error %d\n", error);
    }
    attachedOutputFlush = 1;
  }
}

/* this function is called to check whether there
  is a '\n' in the output that should be flushed */
void flush_until_newline() {
  long i = 0;
  long length = strlen(outputBuff);
  for(; i < length; ++i) {
    if(outputBuff[i] == '\n') {
      char *printBuff = malloc(i+2);
      strncpy(printBuff, outputBuff, i+1);
      printBuff[i+1] = '\0';
      printf("%s", printBuff);
      free(printBuff);
      /* now remove the printed string from the buffer
        and start again */
      {
        long a = 0;
        ++i;
        /* +1 to include \0 */
        for(; i < length + 1; ++a, ++i) {
          outputBuff[a] = outputBuff[i];
        }
        i = 0;
        length = strlen(outputBuff);
      }
    }
  }
}

/* for printf, print all to a string.
  Then cycle through all chars and see if \n is
  written. If there is one, flush the output, otherwise
  write to buffer */
int printf_wrap(const char *format, ...) {
  /* append output to buffer */
  va_list arglist;
  int result;
  va_start( arglist, format );
  result = vsprintf(outputBuff + strlen(outputBuff), format, arglist);
  va_end( arglist );

  /* Now flush if there is a reason to */
  flush_until_newline();

  attachOutputFlush();

  return result;
}

int putchar_wrap(int c) {
  long length = strlen(outputBuff);
  outputBuff[length] = (char)c;
  outputBuff[length+1] = '\0';
  if(c == '\n') {
    flush_until_newline();
  }

  attachOutputFlush();

  return c;
}

int fflush_wrap(FILE* stream) {
  if(stream == stdout) {
    flush_all_output();
  }
  return fflush(stream);
}

int fclose_wrap(FILE* stream) {
  if(stream == stdout) {
    flush_all_output();
  }
  return fclose(stream);
}
#endif /* BUFFERED_OUTPUT */

/* Need input buffer to know whether we need another input request */
/* TODO allocate this dynamically */
static char inputBuff[1<<10] = "";
static long scanf_wrap_number_read = 0;

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
  const long length = strlen(inputBuff);
  long i = 0;
  for(; i < length && isspace(inputBuff[i]); ++i);
  return i == length;
}

/* Define the input functions to overload the old ones */
/* Wrapping of scanf depends on standard */
#ifdef C89_SUPPORT
/* Need to define vscanf for c89.
  TODO: This is a bit risky, since the underlying glibc does not
  have to include this if it is old. If it does not, linking will fail.
  The robust way would be readin via sscanf. */

/* Read formatted input from stdin into argument list ARG.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
   extern int vsscanf (const char *__restrict __s,
                       const char *__restrict __format, _G_va_list __arg)
        __THROW __attribute__ ((__format__ (__scanf__, 2, 0)));

  /* replace all % with %* to suppress read in and do test run */
  long find_scanf_length(const char *format) {
    const long length = strlen(format);
    /* allow for maximum of 50 format specifiers */
    char *formatString = malloc(length + 53);
    long index = 0;
    long formatIndex = 0;
    for(; index < length; ++index, ++formatIndex) {
      formatString[formatIndex] = format[index];
      if(format[index] == '%' &&
        (index + 1 < length && format[index + 1] != '%')) {
        formatString[++formatIndex] = '*';
      }
    }
    /* add number readin */
    formatString[formatIndex++] = '%';
    formatString[formatIndex++] = 'n';
    formatString[formatIndex] = '\0';

    /* now run and record how many characters were read */
    {
      int readLength = 0;
      sscanf(inputBuff, formatString, &readLength);
      free(formatString);

      return readLength;
    }
  }
#endif /* C89_SUPPORT */

int scanf_wrap(const char *format, ...) {
  char doRequest = checkInputRequest();
  char *formatString = 0;

  if(doRequest) {
#ifdef BUFFERED_OUTPUT
    flush_all_output();
#endif
    printf("<inputRequest>");
    fflush(stdout);
    /* read everything from stdin into buffer */
    readIntoBuffer();
  }

  /* add %n to format string to get number of written chars */
  {
    const long length = strlen(format);
    formatString = malloc(length + 3);
    strcpy(formatString, format);
#ifndef C89_SUPPORT
    formatString[length] = '%';
    formatString[length + 1] = 'n';
    formatString[length + 2] = '\0';
#else /* C89_SUPPORT */
    formatString[length] = '\0';
    /* In C89 we need to find how far scanf will read, by hand */
    scanf_wrap_number_read = find_scanf_length(format);
#endif /* C89_SUPPORT */
  }

  {
    va_list arglist;
    int result;
    va_start(arglist, format);
    result = vsscanf(inputBuff, formatString, arglist);
    va_end(arglist);

    /* now move inputBuff up */
    {
      const long length = strlen(inputBuff);
      long index = scanf_wrap_number_read;
      long a = 0;
      /* +1 to include \0 */
      for(; index < length + 1; ++a, ++index) {
        inputBuff[a] = inputBuff[index];
      }
    }

    free(formatString);
    return result;
  }
}

int getchar_wrap(){
  /* check if there is still something in the input buffer*/
  char input = 0;
  long length = strlen(inputBuff);
  if(length <= 0) {
#ifdef BUFFERED_OUTPUT
    flush_all_output();
#endif
    printf("<inputRequest>");
    fflush(stdout);

    readIntoBuffer();
  }

  input = inputBuff[0];
  {
    long i = 1;
    long length = strlen(inputBuff) + 1;
    /* shift all chars one to the left */
    for(; i < length; ++i){
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
#define scanf(format, ...) scanf_wrap(format, ##__VA_ARGS__, &scanf_wrap_number_read)
#else /* C89_SUPPORT */
/* Since there are no variadic macros in C89, this is the only way
  although it is horrible */
#define scanf scanf_wrap
#endif /* C89_SUPPORT */

#define getchar() getchar_wrap()

/* Output defines */
#ifdef BUFFERED_OUTPUT
#define printf printf_wrap
#define putchar putchar_wrap
#define fflush fflush_wrap
#define fclose fclose_wrap
#endif /* BUFFERED_OUTPUT */

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
