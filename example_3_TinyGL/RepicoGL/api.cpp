#include <GL/internal/zgl.h>
#include <stdio.h>
#include "debugger.h"
/* glVertex */

void glVertex4f(GLfloat x,GLfloat y,GLfloat z,GLfloat w)
{
  GLParam p[5];

  p[0].op=OP_Vertex;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;
  p[4].f=w;

  gl_add_op(p);
}

void glVertex2f(GLfloat x,GLfloat y) 
{
  glVertex4f(x,y,int2sll(0),int2sll(1));
}

void glVertex3f(GLfloat x,GLfloat y,GLfloat z) 
{
  glVertex4f(x,y,z,int2sll(1));
}

void glVertex3fv(GLfloat *v) 
{
  glVertex4f(v[0],v[1],v[2],int2sll(1));
}

/* glNormal */

void glNormal3f(GLfloat x,GLfloat y,GLfloat z)
{
  GLParam p[4];

  p[0].op=OP_Normal;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

  gl_add_op(p);
}

void glNormal3fv(GLfloat *v) 
{
  glNormal3f(v[0],v[1],v[2]);
}

/* glColor */

void glColor4f(GLfloat r,GLfloat g,GLfloat b,GLfloat a)
{
  GLParam p[8];

  p[0].op=OP_Color;
  p[1].f=r;
  p[2].f=g;
  p[3].f=b;
  p[4].f=a;
  /* direct convertion to integer to go faster if no shading */
  p[5].ui = (unsigned int) (sll2int(
			  	slladd(
			  		sllmul(r,int2sll(ZB_POINT_RED_MAX - ZB_POINT_RED_MIN)),
                            		int2sll(ZB_POINT_RED_MIN)
				      )
			           )
		 	   );
  p[6].ui = (unsigned int) (sll2int(
			  	slladd(
					sllmul(g, int2sll(ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN)),
                            		int2sll(ZB_POINT_GREEN_MIN)
				      )
				   )
		            );
  p[7].ui = (unsigned int) (sll2int(
			  	slladd(
					sllmul(b, int2sll(ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN)), 
                            		int2sll(ZB_POINT_BLUE_MIN)
				      )
				   )
		            );
  gl_add_op(p);
}

void glColor4fv(GLfloat *v)
{
  GLParam p[8];

  p[0].op=OP_Color;
  p[1].f=v[0];
  p[2].f=v[1];
  p[3].f=v[2];
  p[4].f=v[3];
  /* direct convertion to integer to go faster if no shading */
  p[5].ui = (unsigned int) (sll2int(
			  	slladd(
			  		sllmul(v[0], int2sll(ZB_POINT_RED_MAX - ZB_POINT_RED_MIN)), 
                            		int2sll(ZB_POINT_RED_MIN)
				      )
				    )
		  	   );
  p[6].ui = (unsigned int) (sll2int(
			  	slladd(
					sllmul(v[1], int2sll(ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN)), 
                            		int2sll(ZB_POINT_GREEN_MIN)
				      )
				   )
		           );
  p[7].ui = (unsigned int) (sll2int(
			  	slladd(
					sllmul(v[2], int2sll(ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN)), 
                            		int2sll(ZB_POINT_BLUE_MIN)
				      )
				    )
		            );
  gl_add_op(p);
}

void glColor3f(GLfloat x,GLfloat y,GLfloat z) 
{
  glColor4f(x,y,z,int2sll(1));
}

void glColor3fv(GLfloat *v) 
{
  glColor4f(v[0],v[1],v[2],int2sll(1));
}


/* TexCoord */

void glTexCoord4f(GLfloat s,GLfloat t,GLfloat r,GLfloat q)
{
  GLParam p[5];

  p[0].op=OP_TexCoord;
  p[1].f=s;
  p[2].f=t;
  p[3].f=r;
  p[4].f=q;

  gl_add_op(p);
}

void glTexCoord2f(GLfloat s,GLfloat t)
{
  glTexCoord4f(s,t,int2sll(0),int2sll(1));
}

void glTexCoord2fv(GLfloat *v)
{
  glTexCoord4f(v[0],v[1],int2sll(0),int2sll(1));
}

void glEdgeFlag(int flag)
{
  GLParam p[2];

  p[0].op=OP_EdgeFlag;
  p[1].i=flag;

  gl_add_op(p);
}

/* misc */

void glShadeModel(int mode)
{
  GLParam p[2];

  assert(mode == GL_FLAT || mode == GL_SMOOTH);

  p[0].op=OP_ShadeModel;
  p[1].i=mode;

  gl_add_op(p);
}

void glCullFace(int mode)
{
  GLParam p[2];

  assert(mode == GL_BACK || 
         mode == GL_FRONT || 
         mode == GL_FRONT_AND_BACK);

  p[0].op=OP_CullFace;
  p[1].i=mode;

  gl_add_op(p);
}

void glFrontFace(int mode)
{
  GLParam p[2];

  assert(mode == GL_CCW || mode == GL_CW);

  mode = (mode != GL_CCW);

  p[0].op=OP_FrontFace;
  p[1].i=mode;

  gl_add_op(p);
}

void glPolygonMode(int face,int mode)
{
  GLParam p[3];

  assert(face == GL_BACK || 
         face == GL_FRONT || 
         face == GL_FRONT_AND_BACK);
  assert(mode == GL_POINT || mode == GL_LINE || mode==GL_FILL);

  p[0].op=OP_PolygonMode;
  p[1].i=face;
  p[2].i=mode;

  gl_add_op(p);
}


/* glEnable / glDisable */

void glEnable(int cap)
{
  GLParam p[3];

  p[0].op=OP_EnableDisable;
  p[1].i=cap;
  p[2].i=1;

  gl_add_op(p);
}

void glDisable(int cap)
{
  GLParam p[3];

  p[0].op=OP_EnableDisable;
  p[1].i=cap;
  p[2].i=0;

  gl_add_op(p);
}

/* glBegin / glEnd */

void glBegin(int mode)
{
  GLParam p[2];

  p[0].op=OP_Begin;
  p[1].i=mode;

  gl_add_op(p);
}

void glEnd(void)
{
  GLParam p[1];

  p[0].op=OP_End;

  gl_add_op(p);
}

/* matrix */

void glMatrixMode(int mode)
{
  GLParam p[2];

  p[0].op=OP_MatrixMode;
  p[1].i=mode;

  gl_add_op(p);
}

void glLoadMatrixf(const GLfloat *m)
{
  GLParam p[17];
  int i;

  p[0].op=OP_LoadMatrix;
  for(i=0;i<16;i++) p[i+1].f=m[i];

  gl_add_op(p);
}

void glLoadIdentity(void)
{
  GLParam p[1];

  p[0].op=OP_LoadIdentity;

  gl_add_op(p);
}

void glMultMatrixf(const GLfloat *m)
{
  GLParam p[17];
  int i;

  p[0].op=OP_MultMatrix;
  for(i=0;i<16;i++) p[i+1].f=m[i];

  gl_add_op(p);
}

void glPushMatrix(void)
{
  GLParam p[1];

  p[0].op=OP_PushMatrix;

  gl_add_op(p);
}

void glPopMatrix(void)
{
  GLParam p[1];

  p[0].op=OP_PopMatrix;

  gl_add_op(p);
}

void glRotatef(GLfloat angle,GLfloat x,GLfloat y,GLfloat z)
{
  GLParam p[5];

  p[0].op=OP_Rotate;
  p[1].f=angle;
  p[2].f=x;
  p[3].f=y;
  p[4].f=z;

  gl_add_op(p);
}

void glTranslatef(GLfloat x,GLfloat y,GLfloat z)
{
  GLParam p[4];

  p[0].op=OP_Translate;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

  gl_add_op(p);
}

void glScalef(GLfloat x,GLfloat y,GLfloat z)
{
  GLParam p[4];

  p[0].op=OP_Scale;
  p[1].f=x;
  p[2].f=y;
  p[3].f=z;

  gl_add_op(p);
}


void glViewport(int x,int y,int width,int height)
{
  GLParam p[5];

  p[0].op=OP_Viewport;
  p[1].i=x;
  p[2].i=y;
  p[3].i=width;
  p[4].i=height;

  gl_add_op(p);
}

void glFrustum(GLdouble left,GLdouble right,GLdouble bottom,GLdouble top,
               GLdouble near,GLdouble farv)
{
  GLParam p[7];

  p[0].op=OP_Frustum;
  p[1].f=left;
  p[2].f=right;
  p[3].f=bottom;
  p[4].f=top;
  p[5].f=near;
  p[6].f=farv;

  gl_add_op(p);
}

/* lightening */

void glMaterialfv(int mode,int type,GLfloat *v)
{
  GLParam p[7];
  int i,n;

  assert(mode == GL_FRONT  || mode == GL_BACK || mode==GL_FRONT_AND_BACK);

  p[0].op=OP_Material;
  p[1].i=mode;
  p[2].i=type;
  n=4;
  if (type == GL_SHININESS) n=1;
  for(i=0;i<4;i++) p[3+i].f=v[i];
  for(i=n;i<4;i++) p[3+i].f=int2sll(0);

  gl_add_op(p);
}

void glMaterialf(int mode,int type,GLfloat v)
{
  GLParam p[7];
  int i;

  p[0].op=OP_Material;
  p[1].i=mode;
  p[2].i=type;
  p[3].f=v;
  for(i=0;i<3;i++) p[4+i].f=int2sll(0);

  gl_add_op(p);
}

void glColorMaterial(int mode,int type)
{
  GLParam p[3];

  p[0].op=OP_ColorMaterial;
  p[1].i=mode;
  p[2].i=type;

  gl_add_op(p);
}

void glLightfv(int light,int type,GLfloat *v)
{
  GLParam p[7];
  int i;

  p[0].op=OP_Light;
  p[1].i=light;
  p[2].i=type;
  /* TODO: 3 composants ? */
  for(i=0;i<4;i++) p[3+i].f=v[i];

  gl_add_op(p);
}


void glLightf(int light,int type,GLfloat v)
{
  GLParam p[7];
  int i;

  p[0].op=OP_Light;
  p[1].i=light;
  p[2].i=type;
  p[3].f=v;
  for(i=0;i<3;i++) p[4+i].f=int2sll(0);

  gl_add_op(p);
}

void glLightModeli(int pname,int param)
{
  GLParam p[6];
  int i;

  p[0].op=OP_LightModel;
  p[1].i=pname;
  p[2].f=int2sll(param);
  for(i=0;i<4;i++) p[3+i].f=int2sll(0);

  gl_add_op(p);
}

void glLightModelfv(int pname,GLfloat *param)
{
  GLParam p[6];
  int i;

  p[0].op=OP_LightModel;
  p[1].i=pname;
  for(i=0;i<4;i++) p[2+i].f=param[i];

  gl_add_op(p);
}

/* clear */

void glClear(int mask)
{
  GLParam p[2];

  p[0].op=OP_Clear;
  p[1].i=mask;

  gl_add_op(p);
}

void glClearColor(GLfloat r,GLfloat g,GLfloat b,GLfloat a)
{
  GLParam p[5];

  p[0].op=OP_ClearColor;
  p[1].f=r;
  p[2].f=g;
  p[3].f=b;
  p[4].f=a;

  gl_add_op(p);
}

void glClearDepth(GLdouble depth)
{
  GLParam p[2];

  p[0].op=OP_ClearDepth;
  p[1].f=depth;

  gl_add_op(p);
}


/* textures */

void glTexImage2D( int target, int level, int components,
                   int width, int height, int border,
                   int format, int type, void *pixels)
{
  GLParam p[10];

  p[0].op=OP_TexImage2D;
  p[1].i=target;
  p[2].i=level;
  p[3].i=components;
  p[4].i=width;
  p[5].i=height;
  p[6].i=border;
  p[7].i=format;
  p[8].i=type;
  p[9].p=pixels;

  gl_add_op(p);
}


void glBindTexture(int target,int texture)
{
  GLParam p[3];

  p[0].op=OP_BindTexture;
  p[1].i=target;
  p[2].i=texture;

  gl_add_op(p);
}

void glTexEnvi(int target,int pname,int param)
{
  GLParam p[8];
  
  p[0].op=OP_TexEnv;
  p[1].i=target;
  p[2].i=pname;
  p[3].i=param;
  p[4].f=int2sll(0);
  p[5].f=int2sll(0);
  p[6].f=int2sll(0);
  p[7].f=int2sll(0);

  gl_add_op(p);
}

void glTexParameteri(int target,int pname,int param)
{
  GLParam p[8];
  
  p[0].op=OP_TexParameter;
  p[1].i=target;
  p[2].i=pname;
  p[3].i=param;
  p[4].f=int2sll(0);
  p[5].f=int2sll(0);
  p[6].f=int2sll(0);
  p[7].f=int2sll(0);

  gl_add_op(p);
}

void glPixelStorei(int pname,int param)
{
  GLParam p[3];

  p[0].op=OP_PixelStore;
  p[1].i=pname;
  p[2].i=param;

  gl_add_op(p);
}

/* selection */

void glInitNames(void)
{
  GLParam p[1];

  p[0].op=OP_InitNames;

  gl_add_op(p);
}

void glPushName(unsigned int name)
{
  GLParam p[2];

  p[0].op=OP_PushName;
  p[1].i=name;

  gl_add_op(p);
}

void glPopName(void)
{
  GLParam p[1];

  p[0].op=OP_PopName;

  gl_add_op(p);
}

void glLoadName(unsigned int name)
{
  GLParam p[2];

  p[0].op=OP_LoadName;
  p[1].i=name;

  gl_add_op(p);
}

void 
glPolygonOffset(GLfloat factor, GLfloat units)
{
  GLParam p[3];
  p[0].op = OP_PolygonOffset;
  p[1].f = factor;
  p[2].f = units;
}

/* Special Functions */

void glCallList(unsigned int list)
{
  GLParam p[2];

  p[0].op=OP_CallList;
  p[1].i=list;

  gl_add_op(p);
}

void glFlush(void)
{
  /* nothing to do */
}

void glHint(int target,int mode)
{
  GLParam p[3];

  p[0].op=OP_Hint;
  p[1].i=target;
  p[2].i=mode;

  gl_add_op(p);
}

/* Non standard functions */

void glDebug(int mode)
{
  GLContext *c=gl_get_context();
  c->print_flag=mode;
}

