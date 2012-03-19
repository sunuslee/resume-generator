#include <stdlib.h>
#include <assert.h>
#define malloc(n) MALLOC(n)
#define calloc(nmemb, size) CALLOC(nmemb, size)
#define free(ptr)           FREE(ptr)

#define MALLOC_S(type, ptr, size)               \
do                                              \
{                                               \
        ptr = (type *)malloc(size);             \
        assert(ptr);                            \
}while(0)                                       

int readline(char *input, char *buf);
char *readtext(char *buf);
char *strjoin(char *buf, char *delim, ...);
char *strbetween(char *buf, const char *str, const char *start, const char *end, int n);
char *strrep(char *str, const char *old, const char *new, int times);
void free_all();
void *MALLOC(size_t size);
void *CALLOC(size_t nmemb, size_t size);
void FREE(void *ptr);
