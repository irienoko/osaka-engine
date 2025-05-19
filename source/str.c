#include "backbone/str.h"


Str span(char *beg, char *end)
{
    Str r = {0};
    r.data = beg;
    r.len = beg ? end-beg : 0;
    return r;
}

_Bool equals(Str a, Str b)
{
    return a.len==b.len && (!a.len || !memcmp(a.data, b.data, a.len));
}

Str trimleft(Str s)
{  

    for(; s.len && *s.data <=' '; s.data++, s.len--){}
    return s;
}

Str trimright(Str s)
{
    for(; s.len && s.data[s.len-1]<=' '; s.len--){}
    return s;
}

Str substring(Str s, ptrdiff_t i)
{
    if(i)
    {
        s.data += i;
        s.len -= i;
    }
    return s;
}

Cut cut(Str s, char c)
{
    Cut r = {0};
    if(!s.len) return r;
    char *beg = s.data;
    char *end = s.data + s.len;
    char *cut = beg;
    //iterate though string from *cut starting from beg until it reaches the end of the string or i finds the first the first occurenc of the character "c"
    for(; cut<end && *cut!=c; cut++){}
    r.ok = cut < end;
    //create new string up to but not including found character
    r.head = span(beg, cut);
    //create new string at first occurrence at c or end of string if c is not found
    r.tail = span(cut+r.ok, end);
    return r;
}
