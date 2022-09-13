#include <GL/internal/zgl.h>

void gl_print_matrix( const GLfloat *m)
{
  int i;

  for (i=0;i<4;i++) {
    fprintf(
      stderr,
      "%f %f %f %f\n", 
      sll2dbl(m[i]), 
      sll2dbl(m[4+i]), 
      sll2dbl(m[8+i]), 
      sll2dbl(m[12+i]) );
   }
}

static inline void gl_matrix_update(GLContext *c)
{
  c->matrix_model_projection_updated = (c->matrix_mode <= 1);
}


void glopMatrixMode(GLContext *c,GLParam *p)
{
  int mode=p[1].i;
  switch(mode) {
  case GL_MODELVIEW:
    c->matrix_mode=0;
    break;
  case GL_PROJECTION:
    c->matrix_mode=1;
    break;
  case GL_TEXTURE:
    c->matrix_mode=2;
    break;
  default:
    assert(0);
  }
}

void glopLoadMatrix(GLContext *c,GLParam *p)
{
  M4 *m;
  int i;
  
  GLParam *q;

  m=c->matrix_stack_ptr[c->matrix_mode];
  q=p+1;

  for(i=0;i<4;i++) {
    m->m[0][i]=q[0].f;
    m->m[1][i]=q[1].f;
    m->m[2][i]=q[2].f;
    m->m[3][i]=q[3].f;
    q+=4;
  }

  gl_matrix_update(c);
}

void glopLoadIdentity(GLContext *c,GLParam *p)
{

  gl_M4_Id(c->matrix_stack_ptr[c->matrix_mode]);

  gl_matrix_update(c);
}

void glopMultMatrix(GLContext *c,GLParam *p)
{
  M4 m;
  int i;

  GLParam *q;
  q=p+1;

  for(i=0;i<4;i++) {
    m.m[0][i]=q[0].f;
    m.m[1][i]=q[1].f;
    m.m[2][i]=q[2].f;
    m.m[3][i]=q[3].f;
    q+=4;
  }

  gl_M4_MulLeft(c->matrix_stack_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}


void glopPushMatrix(GLContext *c,GLParam *p)
{
  int n=c->matrix_mode;
  M4 *m;

  assert( (c->matrix_stack_ptr[n] - c->matrix_stack[n] + 1 )
	   < c->matrix_stack_depth_max[n] );

  m=++c->matrix_stack_ptr[n];
  
  gl_M4_Move(&m[0],&m[-1]);

  gl_matrix_update(c);
}

void glopPopMatrix(GLContext *c,GLParam *p)
{
  int n=c->matrix_mode;

  assert( c->matrix_stack_ptr[n] > c->matrix_stack[n] );
  c->matrix_stack_ptr[n]--;
  gl_matrix_update(c);
}


void glopRotate(GLContext *c,GLParam *p)
{
#define SLL_M_PI dbl2sll(M_PI)
  M4 m;
  GLfloat u[3];
  GLfloat angle;
  int dir_code;

  angle = slldiv(sllmul(p[1].f, SLL_M_PI), int2sll(180));
  u[0]=p[2].f;
  u[1]=p[3].f;
  u[2]=p[4].f;

  /* simple case detection */
  dir_code = ((sllvalue(u[0]) != sllvalue(int2sll(0)))<<2) | ((sllvalue(u[1]) != sllvalue(int2sll(0)))<<1) | (sllvalue(u[2]) != sllvalue(int2sll(0)));

  switch(dir_code) {
  case 0:
    gl_M4_Id(&m);
    break;
  case 4:
    if (sllvalue(u[0]) < sllvalue(int2sll(0))) angle=sllneg(angle);
    gl_M4_Rotate(&m,angle,0);
    break;
  case 2:
    if (sllvalue(u[1]) < sllvalue(int2sll(0))) angle=sllneg(angle);
    gl_M4_Rotate(&m,angle,1);
    break;
  case 1:
    if (sllvalue(u[2]) < sllvalue(int2sll(0))) angle=sllneg(angle);
    gl_M4_Rotate(&m,angle,2);
    break;
  default:
    {
      GLfloat cost, sint;

      /* normalize vector */
      GLfloat len = slladd(
		      slladd(
			      sllmul(u[0],u[0]),
			      sllmul(u[1],u[1])
			),
		     sllmul(u[2],u[2]));
      if (sllvalue(len) == sllvalue(int2sll(0))) return;
      len = slldiv(int2sll(1), sllsqrt(len));
      u[0] = sllmul(u[0], len);
      u[1] = sllmul(u[1], len);
      u[2] = sllmul(u[2], len);

      /* store cos and sin values */
      cost=sllcos(angle);
      sint=sllsin(angle);

      /* fill in the values */
      m.m[3][0]=m.m[3][1]=m.m[3][2]=
        m.m[0][3]=m.m[1][3]=m.m[2][3]=int2sll(0);
      m.m[3][3]=int2sll(1);

      /* do the math */
      m.m[0][0]=slladd(sllmul(u[0],u[0]), sllmul(cost, sllsub(int2sll(1), sllmul(u[0],u[0]))));
      m.m[1][0]=sllsub(sllmul(sllmul(u[0],u[1]), sllsub(int2sll(1), cost)), sllmul(u[2],sint));
      m.m[2][0]=slladd(sllmul(sllmul(u[2],u[0]), sllsub(int2sll(1), cost)), sllmul(u[1],sint));
      m.m[0][1]=slladd(sllmul(sllmul(u[0],u[1]), sllsub(int2sll(1), cost)), sllmul(u[2],sint));
      m.m[1][1]=slladd(sllmul(u[1],u[1]), sllmul(cost, sllsub(int2sll(1), sllmul(u[1],u[1]))));
      m.m[2][1]=sllsub(sllmul(sllmul(u[1],u[2]), sllsub(int2sll(1), cost)), sllmul(u[0],sint));
      m.m[0][2]=sllsub(sllmul(sllmul(u[2],u[0]), sllsub(int2sll(1), cost)), sllmul(u[1],sint));
      m.m[1][2]=slladd(sllmul(sllmul(u[1],u[2]), sllsub(int2sll(1), cost)), sllmul(u[0],sint));
      m.m[2][2]=slladd(sllmul(u[2],u[2]), sllmul(cost, sllsub(int2sll(1), sllmul(u[2],u[2]))));
    }
  }

  gl_M4_MulLeft(c->matrix_stack_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}

void glopScale(GLContext *c,GLParam *p)
{
  GLfloat *m;
  GLfloat x=p[1].f,y=p[2].f,z=p[3].f;

  m=&c->matrix_stack_ptr[c->matrix_mode]->m[0][0];

  m[0] = sllmul(m[0],x); m[1] = sllmul(m[1],y); m[2] = sllmul(m[2],z);
  m[4] = sllmul(m[4],x); m[5] = sllmul(m[5],y); m[6] = sllmul(m[6],z);
  m[8] = sllmul(m[8],x); m[9] = sllmul(m[9],y); m[10] = sllmul(m[10],z);
  m[12] = sllmul(m[12],x); m[13] = sllmul(m[13],y); m[14] = sllmul(m[14],z);
  gl_matrix_update(c);
}

void glopTranslate(GLContext *c,GLParam *p)
{
  GLfloat *m;
  GLfloat x=p[1].f,y=p[2].f,z=p[3].f;

  m=&c->matrix_stack_ptr[c->matrix_mode]->m[0][0];

  m[3] = slladd(slladd(slladd(sllmul(m[0], x), sllmul(m[1], y)), sllmul(m[2], z)), m[3]);
  m[7] = slladd(slladd(slladd(sllmul(m[4], x), sllmul(m[5], y)), sllmul(m[6], z)), m[7]);
  m[11] = slladd(slladd(slladd(sllmul(m[8], x), sllmul(m[9], y)), sllmul(m[10], z)), m[11]);
  m[15] = slladd(slladd(slladd(sllmul(m[12], x), sllmul(m[13], y)), sllmul(m[14], z)), m[15]);

  gl_matrix_update(c);
}


void glopFrustum(GLContext *c,GLParam *p)
{
  GLfloat *r;
  M4 m;
  GLfloat left=p[1].f;
  GLfloat right=p[2].f;
  GLfloat bottom=p[3].f;
  GLfloat top=p[4].f;
  GLfloat near=p[5].f;
  GLfloat farp=p[6].f;
  GLfloat x,y,A,B,C,D,tmp2=int2sll(2);

  x = slldiv(sllmul(tmp2,near), sllsub(right,left));
  y = slldiv(sllmul(tmp2,near), sllsub(top,bottom));
  A = slldiv(slladd(right,left), sllsub(right,left));
  B = slldiv(slladd(top,bottom), sllsub(top,bottom));
  C = slldiv(sllneg(slladd(farp,near)), sllsub(farp,near));
  D = slldiv(sllneg(sllmul(sllmul(tmp2,farp),near)), sllsub(farp,near));

  r=&m.m[0][0];
  r[0]= x; r[1]=int2sll(0);  r[2]=A;            r[3]=int2sll(0);
  r[4]= int2sll(0); r[5]=y;  r[6]=B;            r[7]=int2sll(0);
  r[8]= int2sll(0); r[9]=int2sll(0);  r[10]=C;           r[11]=D;
  r[12]=int2sll(0); r[13]=int2sll(0); r[14]=int2sll(-1); r[15]=int2sll(0);

  gl_M4_MulLeft(c->matrix_stack_ptr[c->matrix_mode],&m);

  gl_matrix_update(c);
}
  
