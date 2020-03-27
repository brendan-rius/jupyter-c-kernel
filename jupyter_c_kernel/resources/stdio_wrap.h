#ifndef STDIO_WRAP_H
#define STDIO_WRAP_H

#include <stdio.h>
#include <stdarg.h>

/* Define the functions to overload the old ones */
int scanf_wrap(const char *format, ...) {
  printf("<inputRequest>");
  fflush(stdout);
  va_list arglist;
  va_start( arglist, format );
  int result = vscanf( format, arglist );
  va_end( arglist );
  return result;
}

int getchar_wrap(){
  printf("<inputRequest>");
  fflush(stdout);
  return getchar();
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
