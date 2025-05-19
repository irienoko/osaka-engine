#include "backbone/objparser.h"
#include "backbone/str.h"

#include <stdint.h>
#include "thirdparty/vec.h"



static int32_t parseint(Str s);
static float parseflot(Str s);
static Vert parsevert(Str s);
static Face parseface(Str s, ptrdiff_t nverts, ptrdiff_t nnorms, ptrdiff_t ntexs);

Model loadObjFromFile(const char *path)
{
    Model m = {0};
    Cut lines = {0};


    char  *f_content = NULL;

    int fsize = 0;

    FILE *fp;
    fp = fopen(path, "rb");
    if(fp)
    {
        fseek(fp , 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        f_content = (char*)calloc(fsize, sizeof(char));
        fread(f_content, fsize, 1, fp);

        fclose(fp);
    }

    Str obj = {0};
    obj.data = f_content;
    obj.len = fsize;




    //REMEMBER
    // look to see if str header can be implemented without breaking to many things
    // add rest of obj parser :)
    // should be done my tomorrow. i hope 
    float *pos_verts =  vector_create();
    float *tex_verts = vector_create();
    float *norm_verts = vector_create();

    Face *_tmp_indices_vec = vector_create();

    ptrdiff_t pos_indices_index = 0;

    lines.tail = obj;
    while(lines.tail.len)
    {
        lines = cut(lines.tail, '\n');
        Cut fields= cut(trimright(lines.head), ' ');
        Str kind = fields.head;
        if(equals(S("v"), kind))
        {
            Cut c = cut(trimleft(fields.tail), ' ');
            vector_add(&pos_verts, parseflot(c.head));
            c = cut(trimleft(c.tail), ' ');
            vector_add(&pos_verts, parseflot(c.head));
            c = cut(trimleft(c.tail), ' ');
            vector_add(&pos_verts, parseflot(c.head));

            m.nverts = vector_size(pos_verts);
        }else if(equals(S("vn"), kind))
        {
            Cut c = cut(trimleft(fields.tail), ' ');
            vector_add(&norm_verts, parseflot(c.head));
            c = cut(trimleft(c.tail), ' ');
            vector_add(&norm_verts, parseflot(c.head));
            c = cut(trimleft(c.tail), ' ');
            vector_add(&norm_verts, parseflot(c.head));

            m.nnorms = vector_size(norm_verts);
        }else if(equals(S("vt"), kind))
        {
            Cut c = cut(trimleft(fields.tail), ' ');
            vector_add(&tex_verts, parseflot(c.head));
            c = cut(trimleft(c.tail), ' ');
            vector_add(&tex_verts, parseflot(c.head));
            m.ntexs = vector_size(tex_verts);
        }
        else if(equals(S("f"), kind))
        {
            pos_indices_index++;
            vector_add(&_tmp_indices_vec,parseface(fields.tail, m.nverts, m.nnorms, m.ntexs));
        }
    }


    printf("nVerts: %lld\n", m.nverts);
    printf("nNorms: %lld\n", m.nnorms);
    printf("nTexts: %lld\n", m.ntexs);
    unsigned int *sort_indices = vector_create();
    for(ptrdiff_t f = 0; f < pos_indices_index; f++)
    {
        for(int i =0; i < 3; i++)
        {
            vector_add(&sort_indices, _tmp_indices_vec[f].v[i]-1);
        }
    }


    m.nfaces = vector_size(sort_indices);
    printf("nFaces: %lld\n", m.nfaces);
    m.verts = pos_verts;
    m.pos_indices = sort_indices;

    //printf("%f\n", m.verts->v[0]);
    free(f_content);

    return m;
}

static int32_t parseint(Str s)
{
    uint32_t r      = 0;
    int32_t  sign   = 1;
    for(ptrdiff_t i = 0; i < s.len; i++)
    {
        switch(s.data[i])
        {
            case '+':             break;
            case '-': sign = -1;  break;
            default: r = 10*r + s.data[i] - '0';
        }
    }
    return r * sign;
}

static float expt10(int32_t e)
{
    float y = 1.0f;
    float x = e<0 ? 0.1f : e>0 ? 10.0f : 1.0f;
    int32_t n = e<0 ? e : -e;
    for(; n < -1; n /= 2)
    {
        y *= n%2 ? x : 1.0f;
        x *= x;
    }
    return x * y;
}

static float parseflot(Str s)
{
    float r     = 0.0f;
    float sign  = 1.0f;
    float exp   = 0.0f;

    for(ptrdiff_t i = 0; i < s.len; i++)
    {
        switch(s.data[i])
        {
            case '+': break;
            case '-': sign = -1; break;
            case '.': exp = 1; break;
            case 'E':
            case 'e': return 0;
            default: r = 10.0f*r + (s.data[i] - '0');
                exp *= 0.1f;
        }
    }
    return sign * r * (exp ? exp: 1.0f);
}

static Vert parsevert(Str s)
{
    Vert r = {0};
    Cut c = cut(trimleft(s), ' ');
    r.v[0] = parseflot(c.head);
    c = cut(trimleft(c.tail), ' ');
    r.v[1] = parseflot(c.head);
    c = cut(trimleft(c.tail), ' ');
    r.v[2] = parseflot(c.head);
    return r;
}

static Face parseface(Str s, ptrdiff_t nverts, ptrdiff_t nnorms, ptrdiff_t ntexs)
{
    Face r      = {0};
    Cut fields  = {0};
    fields.tail = s;
    for(int i = 0; i < 3; i++)
    {
        fields = cut(trimleft(fields.tail), ' ');
        Cut elem = cut(fields.head, '/');
        r.v[i] = parseint(elem.head);
        elem = cut(elem.tail, '/');
        r.t[i] = parseint(elem.head);
        elem = cut(elem.tail, '/');
        r.n[i] = parseint(elem.head);

        if(r.v[i] < 0)
        {
            r.v[i] = (int32_t)(r.v[i] + 1 + nverts);
        }
        if(r.n[i]<0)
        {
            r.n[i] = (int32_t)(r.n[i] + 1 + nnorms);
        }
        if(r.t[i]<0)
        {
            r.t[i] = (int32_t)(r.t[i] + 1 + ntexs);
        }
    }
    
    return r;
}
