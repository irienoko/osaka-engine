#ifndef STR_INCLUDE
#define STR_INCLUDE


#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define S(s) (Str){s, sizeof(s)-1}

typedef struct
{
    char *data;
    ptrdiff_t len;
}Str;

typedef struct
{
    Str head;
    Str tail;
    _Bool ok;
} Cut;

Str span(char *beg, char *end);

_Bool equals(Str a, Str b);

Str trimleft(Str s);

Str trimright(Str s);

Str substring(Str s, ptrdiff_t i);

Cut cut(Str s, char c);


// end header file /////////////////
#endif // end STR_INCLUDE