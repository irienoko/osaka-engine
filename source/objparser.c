#include "backbone/objparser.h"
#include "backbone/str.h"

#include <stdint.h>
#include "thirdparty/vec.h"


static int32_t parseint(Str s);
static float parseflot(Str s);


static vec3_vert parse_vec3_vert(Str s);
static vec2_vert parse_vec2_vert(Str s);


//TODO:
// removing duplicates

static Face parseface(Str s, ptrdiff_t nverts, ptrdiff_t nnorms, ptrdiff_t ntexs);

static int compareVertex(Mesh_index mesh, Model model, int index);

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
    vec3_vert *pos_verts =  vector_create();
    vec3_vert *norm_verts = vector_create();

    vec2_vert *uv_verts = vector_create();

    Face *_tmp_indices_vec = vector_create();

    int pos_indices_index = 0;

    lines.tail = obj;
    while(lines.tail.len)
    {
        lines = cut(lines.tail, '\n');
        Cut fields= cut(trimright(lines.head), ' ');
        Str kind = fields.head;
        if(equals(S("v"), kind))
        {
            vector_add(&pos_verts, parse_vec3_vert(fields.tail));
            m.nverts = vector_size(pos_verts);
        }else if(equals(S("vn"), kind))
        {
            vector_add(&norm_verts, parse_vec3_vert(fields.tail));
            m.nnorms = vector_size(norm_verts);
        }else if(equals(S("vt"), kind))
        {
            vector_add(&uv_verts, parse_vec2_vert(fields.tail));
            m.nuvs = vector_size(uv_verts);
        }
        else if(equals(S("f"), kind))
        {
            pos_indices_index++;
            vector_add(&_tmp_indices_vec,parseface(fields.tail, m.nverts, m.nnorms, m.nuvs));
        }
    }

    

    unsigned int *vertex_indices = vector_create();
    unsigned int *normal_indices = vector_create();
    unsigned int *uv_indices = vector_create();

    vec3_vert *out_vertex = vector_create();
    vec3_vert *out_norm = vector_create();
    vec2_vert *out_uv = vector_create();

    for(int f = 0; f < pos_indices_index; f++)
    {
        for(int i =0; i < 3; i++)
        {
            vector_add(&vertex_indices, _tmp_indices_vec[f].v[i]-1);
            vector_add(&normal_indices, _tmp_indices_vec[f].n[i]-1);
            vector_add(&uv_indices, _tmp_indices_vec[f].t[i]-1);

           
        }
    }

    int vertex_indices_size = vector_size(vertex_indices);
    m.n_face = vertex_indices_size;
    vector_free(_tmp_indices_vec);


    for(int v = 0; v < vertex_indices_size; v+=3)
    {
        for(int i = 0; i < 3; i+=1)
        {
            int vertex_indices_index = vertex_indices[v+i];
            int normal_indices_index = normal_indices[v+i];
            int uv_indices_index = uv_indices[v+i];
            vector_add(&out_vertex, pos_verts[vertex_indices_index]);
            vector_add(&out_norm, norm_verts[normal_indices_index]);
            vector_add(&out_uv, uv_verts[uv_indices_index]);
        }
    }



    m.out_verts = out_vertex;
    m.nverts = vector_size(out_vertex);
    m.out_norms = out_norm;
    m.nnorms = vector_size(out_norm);
    m.out_uvs = out_uv;
    m.nuvs = vector_size(out_uv);


    free(f_content);
    vector_free(vertex_indices);
    vector_free(normal_indices);
    vector_free(uv_indices);

    vector_free(pos_verts);

    vector_free(norm_verts);

    vector_free(uv_verts);

    return m;
}



Mesh_index indexVBO(Model m)
{
    Mesh_index mesh = {0};

    unsigned int *out_indices = vector_create();

    vec3_vert *out_vertex = vector_create();
    vec3_vert *out_norm = vector_create();
    vec2_vert *out_uv = vector_create();
    int found_index = -1;
    
    vector_add(&out_vertex, m.out_verts[0]);
    vector_add(&out_norm, m.out_norms[0]);
    vector_add(&out_uv, m.out_uvs[0]);

    mesh.out_verts = out_vertex;
    mesh.out_norms = out_norm;
    mesh.out_uvs = out_uv;

    float *packed_vertex = vector_create();


    for(int i = 0; i < m.nverts; i++)
    {
        for (int j =0; j < 3; j++)
        {
            vector_add(&packed_vertex,  m.out_verts[i].v[j]);
        }
    }


    for(int i =0; i < m.nverts; i++)
    {
        if(compareVertex(mesh, m, i))
        {
            vector_add(&out_indices, i);
            //
            mesh.indices = out_indices;
        }else
        {
            vector_add(&out_vertex, m.out_verts[i]);
            vector_add(&out_norm, m.out_norms[i]);
            vector_add(&out_uv, m.out_uvs[i]);

            mesh.out_verts = out_vertex;
            mesh.nverts = vector_size(out_vertex);
            mesh.out_norms = out_norm;
            mesh.nnorms = vector_size(out_norm);
            mesh.out_uvs = out_uv;
            mesh.nuvs = vector_size(out_uv);

            vector_add(&out_indices,  mesh.nverts -1);
            mesh.indices = out_indices;

            mesh.indices_num = vector_size(out_indices);
        }
    }


    return mesh;
}

static int compareVertex(Mesh_index mesh, Model model, int index)
{
    return
    (
        mesh.out_verts[index].v[0] == model.out_verts[index].v[0] &&
        mesh.out_verts[index].v[1] == model.out_verts[index].v[1] &&
        mesh.out_verts[index].v[2] == model.out_verts[index].v[2] &&
        
        mesh.out_norms[index].v[0] == model.out_norms[index].v[0] &&
        mesh.out_norms[index].v[1] == model.out_norms[index].v[1] &&
        mesh.out_norms[index].v[2] == model.out_norms[index].v[2] &&

        mesh.out_uvs[index].v[0] == model.out_uvs[index].v[0] &&
        mesh.out_uvs[index].v[1] == model.out_uvs[index].v[1]
    );
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

static vec2_vert parse_vec2_vert(Str s)
{
    vec2_vert r = {0};
    Cut c = cut(trimleft(s), ' ');
    r.v[0] = parseflot(c.head);
    c = cut(trimleft(c.tail), ' ');
    r.v[1] = parseflot(c.head);
    r.v[1] = -r.v[1];

    return r;
}
static vec3_vert parse_vec3_vert(Str s)
{
    vec3_vert r = {0};

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
