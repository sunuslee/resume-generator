#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "utility.h"
/*
 *  sunus lee
 *  https://github.com/sunuslee/SunuSfuncS/
 *  Mar 10, 2012
 *  call this function in this format: 
 *  p = strjoin(buf, "^^", "string1", "string2", "string3", NULL);
 *  the parameters MUST END WITH NULL.
 *  No safety check for this function, buf MUST BE BIG ENOUGH!
 */
char *strjoin(char *buf, char *delim, ...)
{
    char *p, *res, *d;
    va_list ap;
    va_start(ap, delim);
    res = buf;
    p = va_arg(ap, char *);
    while(p)
    {
        while((*res++ = *p++))
            /* do nothing */;
        res--;
        if((p = va_arg(ap, char *)))
        {
            d = delim;
            while((*res++ = *d++))
                /* do nothing */;
            res--;
        }
    }
    *res = '\0';
    va_end(ap);
    return buf;
}
/* 
 * get the No.'n' substring between 'start' and 'end'
 * n starts from 1.
 */
char *strbetween(char *buf, const char *str, const char *start, const char *end, int n)
{
    int step;
    char *s, *e;
    char *res = buf;
    step = strlen(start);
    while(n--)
    {
        s = strstr(str, start);
        str = (s + step);
    };
    if(!s)
        return NULL;
    e = strstr(s, end);
    if(!e)
        return NULL;
    s += step;
    while(s < e)
        *buf++ = *s++;
    *buf = 0;
    return res;
}

int readline(char *input, char *buf)
{
    char *s = input;
    while((*s != 0) && (*s != '\n'))
    {
        *buf++ = *s++;
    }
    *buf = '\n';
    *(buf + 1) = '\0';
    if(*s == 0)
        *(buf + 2) = EOF;
    return s - input + 1;
}
static char linebuf[2048];
static char *plinebuf;
char *readtext(char *buf)
{
    int offset = 0;
    plinebuf = linebuf;
    while(1)
    {
        plinebuf = gets(plinebuf);
        if(plinebuf)
            offset += sprintf(buf + offset, "%s\n", plinebuf);
        else
        {
            buf[offset - 1] = '\0';
            return buf;
        }
    }
}

char *strrep(char *str, const char *old, const char *new, int times)
{
    int all, stepo, stepn;
    char *s;
    char *res = str;
    static char b[1024];
    char *buf = b;
    all = (times == 0 ? 1 : 0);
    stepo = strlen(old);
    stepn = strlen(new);
    while(times-- || all)
    {
        s = strstr(str, old);
        if(!s)
        {
            break;
        }
        while(str < s)
        {
            *buf++ = *str++;
        }
        while((*buf++ = *new++))
            ;
        buf--;
        new -= (stepn + 1);
        str = (s + stepo);
    }
    while((*buf++ = *str++)) ;
    *buf = 0;
    strcpy(res, b);
    return res;
}
unsigned int ptrlist[1024];
unsigned int *pptrlist = ptrlist;
void *MALLOC(size_t size)
{
    #ifdef malloc
    #undef malloc
    #endif
    void *p;
    p = malloc(size);
    #ifndef malloc
    #define malloc(size)    MALLOC(size)
    #endif
    if(p)
    {
        *pptrlist++ = (unsigned int)p;
        return p;
    }
    else
        return NULL;
}

void *CALLOC(size_t nmemb, size_t size)
{
    #ifdef calloc
    #undef calloc
    #endif
    void *p;
    p = calloc(nmemb, size);
    #ifndef calloc
    #define calloc(nmemb, size)    CALLOC(nmemb, size)
    #endif
    if(p)
    {
        *pptrlist++ = (unsigned int)p;
        return p;
    }
    else
        return NULL;
}

void FREE(void *ptr)
{
    #ifdef free
    #undef free
    #endif
    unsigned int *p;
    for(p = ptrlist; p < pptrlist; p++)
    {
        if(*p == (unsigned int)ptr)
        {
            free(ptr);
            *p = 0;
            #ifndef free
            #define free(ptr) FREE(ptr)
            #endif
            return ;
        }
    }
}

void free_all()
{
    unsigned int *p;
    for(p = ptrlist; p < pptrlist; p++)
    {
        free((void *)p);
    }
}
