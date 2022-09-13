#include <GL/internal/zgl.h>

#include "msghandling.h"

void glopMaterial(GLContext *c,GLParam *p)
{
  int mode=p[1].i;
  int type=p[2].i;
  GLfloat *v=&p[3].f;
  int i;
  GLMaterial *m;

  if (mode == GL_FRONT_AND_BACK) {
    p[1].i=GL_FRONT;
    glopMaterial(c,p);
    mode=GL_BACK;
  }
  m = &c->materials[(mode == GL_FRONT) ? 0 : 1];
  
  switch(type) {
  case GL_EMISSION:
    for(i=0;i<4;i++)
      m->emission.v[i]=v[i];
    break;
  case GL_AMBIENT:
    for(i=0;i<4;i++)
      m->ambient.v[i]=v[i];
    break;
  case GL_DIFFUSE:
    for(i=0;i<4;i++)
      m->diffuse.v[i]=v[i];
    break;
  case GL_SPECULAR:
    for(i=0;i<4;i++)
      m->specular.v[i]=v[i];
    break;
  case GL_SHININESS:
    m->shininess=v[0];
    m->shininess_i = sll2int(sllmul(slldiv(v[0], int2sll(128)), int2sll(SPECULAR_BUFFER_RESOLUTION)));
    break;
  case GL_AMBIENT_AND_DIFFUSE:
    for(i=0;i<4;i++)
      m->diffuse.v[i]=v[i];
    for(i=0;i<4;i++)
      m->ambient.v[i]=v[i];
    break;
  default:
    assert(0);
  }
}

void glopColorMaterial(GLContext *c,GLParam *p)
{
  int mode=p[1].i;
  int type=p[2].i;

  c->current_color_material_mode=mode;
  c->current_color_material_type=type;
}

void glopLight(GLContext *c,GLParam *p)
{
  int light=p[1].i;
  int type=p[2].i;
  V4 v;
  GLLight *l;
  int i;
  
  assert(light >= GL_LIGHT0 && light < GL_LIGHT0+MAX_LIGHTS );

  l=&c->lights[light-GL_LIGHT0];

  for(i=0;i<4;i++) v.v[i]=p[3+i].f;

  switch(type) {
  case GL_AMBIENT:
    l->ambient=v;
    break;
  case GL_DIFFUSE:
    l->diffuse=v;
    break;
  case GL_SPECULAR:
    l->specular=v;
    break;
  case GL_POSITION:
    {
      V4 pos;
      gl_M4_MulV4(&pos,c->matrix_stack_ptr[0],&v);

      l->position=pos;

      if (sllvalue(l->position.v[3]) == sllvalue(int2sll(0))) {
        l->norm_position.X=pos.X;
        l->norm_position.Y=pos.Y;
        l->norm_position.Z=pos.Z;
        
        gl_V3_Norm(&l->norm_position);
      }
    }
    break;
  case GL_SPOT_DIRECTION:
    for(i=0;i<3;i++) {
      l->spot_direction.v[i]=v.v[i];
      l->norm_spot_direction.v[i]=v.v[i];
    }
    gl_V3_Norm(&l->norm_spot_direction);
    break;
  case GL_SPOT_EXPONENT:
    l->spot_exponent=v.v[0];
    break;
  case GL_SPOT_CUTOFF:
    {
#define SLL_M_PI dbl2sll(M_PI)
      GLfloat a=v.v[0], tmp180=int2sll(180);
      assert(sllvalue(a) == sllvalue(tmp180) || (sllvalue(a)>=sllvalue(int2sll(0)) && sllvalue(a)<=sllvalue(int2sll(90))));
      l->spot_cutoff=a;
      if (sllvalue(a) != sllvalue(tmp180)) l->cos_spot_cutoff=sllcos(slldiv(sllmul(a, SLL_M_PI), tmp180));
    }
    break;
  case GL_CONSTANT_ATTENUATION:
    l->attenuation[0]=v.v[0];
    break;
  case GL_LINEAR_ATTENUATION:
    l->attenuation[1]=v.v[0];
    break;
  case GL_QUADRATIC_ATTENUATION:
    l->attenuation[2]=v.v[0];
    break;
  default:
    assert(0);
  }
}
  

void glopLightModel(GLContext *c,GLParam *p)
{
  int pname=p[1].i;
  GLfloat *v=&p[2].f;
  int i;

  switch(pname) {
  case GL_LIGHT_MODEL_AMBIENT:
    for(i=0;i<4;i++) 
      c->ambient_light_model.v[i]=v[i];
    break;
  case GL_LIGHT_MODEL_LOCAL_VIEWER:
    c->local_light_model=sll2int(v[0]);
    break;
  case GL_LIGHT_MODEL_TWO_SIDE:
    c->light_model_two_side = sll2int(v[0]);
    break;
  default:
    tgl_warning("glopLightModel: illegal pname: 0x%x\n", pname);
    //assert(0);
    break;
  }
}


static inline GLfloat clampf(GLfloat a,GLfloat min,GLfloat max)
{
  if (sllvalue(a)<sllvalue(min)) return min;
  else if (sllvalue(a)>sllvalue(max)) return max;
  else return a;
}

void gl_enable_disable_light(GLContext *c,int light,int v)
{
  GLLight *l=&c->lights[light];
  if (v && !l->enabled) {
    l->enabled=1;
    l->next=c->first_light;
    c->first_light=l;
    l->prev=NULL;
  } else if (!v && l->enabled) {
    l->enabled=0;
    if (l->prev == NULL) c->first_light=l->next;
    else l->prev->next=l->next;
    if (l->next != NULL) l->next->prev=l->prev;
  }
}

/* non optimized lightening model */
void gl_shade_vertex(GLContext *c,GLVertex *v)
{
  GLfloat R,G,B,A;
  GLMaterial *m;
  GLLight *l;
  V3 n,s,d;
  GLfloat dist,tmp,att,dot,dot_spot,dot_spec;
  int twoside = c->light_model_two_side;

  m=&c->materials[0];

  n.X=v->normal.X;
  n.Y=v->normal.Y;
  n.Z=v->normal.Z;

  R=slladd(m->emission.v[0], sllmul(m->ambient.v[0], c->ambient_light_model.v[0]));
  G=slladd(m->emission.v[1], sllmul(m->ambient.v[1], c->ambient_light_model.v[1]));
  B=slladd(m->emission.v[2], sllmul(m->ambient.v[2], c->ambient_light_model.v[2]));
  A=clampf(m->diffuse.v[3], int2sll(0), int2sll(1));

  for(l=c->first_light;l!=NULL;l=l->next) {
    GLfloat lR,lB,lG;
    
    /* ambient */
    lR=sllmul(l->ambient.v[0], m->ambient.v[0]);
    lG=sllmul(l->ambient.v[1], m->ambient.v[1]);
    lB=sllmul(l->ambient.v[2], m->ambient.v[2]);

    if (sllvalue(l->position.v[3]) == sllvalue(int2sll(0))) {
      /* light at infinity */
      d.X=l->position.v[0];
      d.Y=l->position.v[1];
      d.Z=l->position.v[2];
      att=int2sll(1);
    } else {
      /* distance attenuation */
      d.X=sllsub(l->position.v[0], v->ec.v[0]);
      d.Y=sllsub(l->position.v[1], v->ec.v[1]);
      d.Z=sllsub(l->position.v[2], v->ec.v[2]);
      dist=sllsqrt(slladd(slladd(sllmul(d.X,d.X), sllmul(d.Y,d.Y)), sllmul(d.Z,d.Z)));
      if (sllvalue(dist)>sllvalue(dbl2sll(1E-3))) {
        tmp=slldiv(int2sll(1),dist);
        d.X=sllmul(d.X, tmp);
        d.Y=sllmul(d.Y, tmp);
        d.Z=sllmul(d.Z, tmp);
      }
      att=slldiv(int2sll(1), slladd(l->attenuation[0], 
		sllmul(dist, slladd(l->attenuation[1],
				sllmul(dist, l->attenuation[2])))));
    }
    dot=slladd(slladd(sllmul(d.X,n.X), sllmul(d.Y,n.Y)), sllmul(d.Z,n.Z));
    if (twoside && sllvalue(dot) < sllvalue(int2sll(0))) dot = sllneg(dot);
    if (sllvalue(dot)>sllvalue(int2sll(0))) {
      /* diffuse light */
      lR = slladd(lR, sllmul(sllmul(dot, l->diffuse.v[0]), m->diffuse.v[0]));
      lG = slladd(lG, sllmul(sllmul(dot, l->diffuse.v[1]), m->diffuse.v[1]));
      lB = slladd(lB, sllmul(sllmul(dot, l->diffuse.v[2]), m->diffuse.v[2]));

      /* spot light */
      if (sllvalue(l->spot_cutoff) != sllvalue(int2sll(180))) {
        dot_spot=sllneg(
			slladd(
				slladd(
			 		sllmul(d.X,l->norm_spot_direction.v[0]),
                   	 		sllmul(d.Y,l->norm_spot_direction.v[1])
				),
                   	 	sllmul(d.Z,l->norm_spot_direction.v[2])));
        if (twoside && sllvalue(dot_spot) < sllvalue(int2sll(0))) dot_spot = sllneg(dot_spot);
        if (sllvalue(dot_spot) < sllvalue(l->cos_spot_cutoff)) {
          /* no contribution */
          continue;
        } else {
          /* TODO: optimize */
          if (sllvalue(l->spot_exponent) > sllvalue(int2sll(0))) {
            att=sllmul(att, sllpow(dot_spot,l->spot_exponent));
          }
        }
      }

      /* specular light */
      
      if (c->local_light_model) {
        V3 vcoord;
        vcoord.X=v->ec.X;
        vcoord.Y=v->ec.Y;
        vcoord.Z=v->ec.Z;
        gl_V3_Norm(&vcoord);
        s.X=sllsub(d.X, vcoord.X);
        s.Y=sllsub(d.Y, vcoord.X);
        s.Z=sllsub(d.Z, vcoord.X);
      } else {
        s.X=d.X;
        s.Y=d.Y;
        s.Z=slladd(d.Z, int2sll(1));
      }
      dot_spec=slladd(slladd(sllmul(n.X,s.X), sllmul(n.Y,s.Y)), sllmul(n.Z,s.Z));
      if (twoside && sllvalue(dot_spec) < sllvalue(int2sll(0))) dot_spec = sllneg(dot_spec);
      if (sllvalue(dot_spec)>sllvalue(int2sll(0))) {
        GLSpecBuf *specbuf;
        int idx;
        tmp=sllsqrt(slladd(slladd(sllmul(s.X,s.X), sllmul(s.Y,s.Y)), sllmul(s.Z,s.Z)));
        if (sllvalue(tmp) > sllvalue(dbl2sll(1E-3))) {
          dot_spec=slldiv(dot_spec, tmp);
        }
      
        /* TODO: optimize */
        /* testing specular buffer code */
        /* dot_spec= pow(dot_spec,m->shininess);*/
        specbuf = specbuf_get_buffer(c, m->shininess_i, m->shininess);
        idx = sll2int(sllmul(dot_spec, int2sll(SPECULAR_BUFFER_SIZE)));
        if (idx > SPECULAR_BUFFER_SIZE) idx = SPECULAR_BUFFER_SIZE;
        dot_spec = specbuf->buf[idx];
        lR=slladd(lR, sllmul(sllmul(dot_spec, l->specular.v[0]), m->specular.v[0]));
        lG=slladd(lG, sllmul(sllmul(dot_spec, l->specular.v[1]), m->specular.v[1]));
        lB=slladd(lB, sllmul(sllmul(dot_spec, l->specular.v[2]), m->specular.v[2]));
      }
    }

    R=slladd(R, sllmul(att, lR));
    G=slladd(G, sllmul(att, lG));
    B=slladd(B, sllmul(att, lB));
  }

  v->color.v[0]=clampf(R, int2sll(0), int2sll(1));
  v->color.v[1]=clampf(G, int2sll(0), int2sll(1));
  v->color.v[2]=clampf(B, int2sll(0), int2sll(1));
  v->color.v[3]=A;
}

