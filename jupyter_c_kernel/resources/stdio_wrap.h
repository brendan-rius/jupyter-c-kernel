#include <stdio.h>

/* Replace all the necessary input functions */
#define scanf(...) printf("<inputRequest>");\
fflush(stdout);\
scanf(__VA_ARGS__);

#define getchar() printf("<inputRequest>");\
fflush(stdout);\
getchar();
