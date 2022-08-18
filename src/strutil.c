#include <string.h>
#include "strutil.h"

char *split(char *str)
{
    char *s;

    for (s = str; *s > ' '; s++);

    if (!(*s)) {
        return s;
    }

    *s = 0;
    while (*(++s) <= ' ' && *s != 0);
    return s;
}

void limit(char *s, int max)
{
    if (strlen(s) > max) {
        s[max] = 0;
    }
}

void trim(char *str)
{
    char *s;

    s = strchr(str, '\n');
    if (s) {
        *s = 0;
    }
    s = strchr(str, '\r');
    if (s) {
        *s = 0;
    }
}

void lowercase(char *s)
{
    while (*(s++)) {
        if (*s >= 'A' && *s <= 'Z') {
            *s += 'A' - 'a';
        }
    }
}

int strstart(const char *s, char *ss)
{
    return ((strstr(s, ss) == s) && *ss);
}
