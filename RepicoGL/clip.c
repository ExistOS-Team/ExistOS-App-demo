#include <GL/internal/zgl.h>

#define CLIP_XMIN   (1<<0)
#define CLIP_XMAX   (1<<1)
#define CLIP_YMIN   (1<<2)
#define CLIP_YMAX   (1<<3)
#define CLIP_ZMIN   (1<<4)
#define CLIP_ZMAX   (1<<5)

void gl_transform_to_viewport(GLContext *c,GLVertex *v)
{
  GLfloat winv;

  /* coordinates */
  winv=slldiv(int2sll(1),v->pc.W);
  v->zp.x= sll2int(slladd(sllmul(sllmul(v->pc.X, winv), c->viewport.scale.X), 
                   	c->viewport.trans.X));
  v->zp.y= sll2int(slladd(sllmul(sllmul(v->pc.Y, winv), c->viewport.scale.Y), 
                   	c->viewport.trans.Y));
  v->zp.z= sll2int(slladd(sllmul(sllmul(v->pc.Z, winv), c->viewport.scale.Z), 
                   	c->viewport.trans.Z));
  /* color */
  if (c->lighting_enabled) {
      v->zp.r=sll2int(slladd(
		     	sllmul(v->color.v[0],
				int2sll(ZB_POINT_RED_MAX - ZB_POINT_RED_MIN)
			), 
                     int2sll(ZB_POINT_RED_MIN)));
      v->zp.g=sll2int(slladd(
			sllmul(v->color.v[1],
				int2sll(ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) 
			),
                     int2sll(ZB_POINT_GREEN_MIN)));
      v->zp.b=sll2int(slladd(
			sllmul(v->color.v[2],
				int2sll(ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) 
			),
                     int2sll(ZB_POINT_BLUE_MIN)));
  } else {
      /* no need to convert to integer if no lighting : take current color */
      v->zp.r = c->longcurrent_color[0];
      v->zp.g = c->longcurrent_color[1];
      v->zp.b = c->longcurrent_color[2];
  }
  
  /* texture */

  if (c->texture_2d_enabled) {
    v->zp.s=sll2int(slladd(
			sllmul(v->tex_coord.X,
				int2sll(ZB_POINT_S_MAX - ZB_POINT_S_MIN) 
			),
     			int2sll(ZB_POINT_S_MIN)));
    v->zp.t=sll2int(slladd(
			sllmul(v->tex_coord.Y,
				int2sll(ZB_POINT_T_MAX - ZB_POINT_T_MIN) 
			),
			int2sll(ZB_POINT_T_MIN)));
  }
}


static void gl_add_select1(GLContext *c,int z1,int z2,int z3)
{
//  unsigned int min,max;
  int min,max;
  min=max=z1;
  if (z2<min) min=z2;
  if (z3<min) min=z3;
  if (z2>max) max=z2;
  if (z3>max) max=z3;

//  gl_add_select(c,0xffffffff-min,0xffffffff-max);
  gl_add_select(c,-1-min,-1-max);
}

/* point */

void gl_draw_point(GLContext *c,GLVertex *p0)
{
  if (p0->clip_code == 0) {
    if (c->render_mode == GL_SELECT) {
      gl_add_select(c,p0->zp.z,p0->zp.z);
    } else {
      ZB_plot(c->zb,&p0->zp);
    }
  }
}

/* line */

static inline void interpolate(GLVertex *q,GLVertex *p0,GLVertex *p1,GLfloat t)
{
  q->pc.X=slladd(p0->pc.X, sllmul(sllsub(p1->pc.X, p0->pc.X), t));
  q->pc.Y=slladd(p0->pc.Y, sllmul(sllsub(p1->pc.Y, p0->pc.Y), t));
  q->pc.Z=slladd(p0->pc.Z, sllmul(sllsub(p1->pc.Z, p0->pc.Z), t));
  q->pc.W=slladd(p0->pc.W, sllmul(sllsub(p1->pc.W, p0->pc.W), t));

  q->color.v[0]=slladd(p0->color.v[0], sllmul(sllsub(p1->color.v[0], p0->color.v[0]), t));
  q->color.v[1]=slladd(p0->color.v[1], sllmul(sllsub(p1->color.v[1], p0->color.v[1]), t));
  q->color.v[2]=slladd(p0->color.v[2], sllmul(sllsub(p1->color.v[2], p0->color.v[2]), t));
}

/*
 * Line Clipping 
 */

/* Line Clipping algorithm from 'Computer Graphics', Principles and
   Practice */
static inline int ClipLine1(GLfloat denom,GLfloat num,GLfloat *tmin,GLfloat *tmax)
{
  GLfloat t;
	 
  if (sllvalue(denom)>sllvalue(int2sll(0))) {
    t=slldiv(num, denom);
    if (sllvalue(t)>sllvalue((*tmax))) return 0;
    if (sllvalue(t)>sllvalue((*tmin))) *tmin=t;
  } else if (sllvalue(denom)<sllvalue(int2sll(0))) {
    t=slldiv(num, denom);
    if (sllvalue(t)<sllvalue((*tmin))) return 0;
    if (sllvalue(t)<sllvalue((*tmax))) *tmax=t;
  } else if (sllvalue(num)>sllvalue(int2sll(0))) return 0;
  return 1;
}

void gl_draw_line(GLContext *c,GLVertex *p1,GLVertex *p2)
{
  GLfloat dx,dy,dz,dw,x1,y1,z1,w1;
  GLfloat tmin,tmax;
  GLVertex q1,q2;
  int cc1,cc2;

  cc1=p1->clip_code;
  cc2=p2->clip_code;

  if ( (cc1 | cc2) == 0) {
    if (c->render_mode == GL_SELECT) {
      gl_add_select1(c,p1->zp.z,p2->zp.z,p2->zp.z);
    } else {
        if (c->depth_test)
            ZB_line_z(c->zb,&p1->zp,&p2->zp);
        else
            ZB_line(c->zb,&p1->zp,&p2->zp);
    }
  } else if ( (cc1&cc2) != 0 ) {
    return;
  } else {
    dx=sllsub(p2->pc.X, p1->pc.X);
    dy=sllsub(p2->pc.Y, p1->pc.Y);
    dz=sllsub(p2->pc.Z, p1->pc.Z);
    dw=sllsub(p2->pc.W, p1->pc.W);
    x1=p1->pc.X;
    y1=p1->pc.Y;
    z1=p1->pc.Z;
    w1=p1->pc.W;
    
    tmin=int2sll(0);
    tmax=int2sll(1);
    if (ClipLine1( slladd(dx, dw), sllsub(sllneg(x1), w1), &tmin, &tmax) &&
        ClipLine1( slladd(sllneg(dx), dw), sllsub(x1, w1), &tmin, &tmax) &&
        ClipLine1( slladd(dy, dw), sllsub(sllneg(y1), w1), &tmin, &tmax) &&
        ClipLine1( slladd(sllneg(dy), dw), sllsub(y1, w1), &tmin, &tmax) &&
        ClipLine1( slladd(dz, dw), sllsub(sllneg(z1), w1), &tmin, &tmax) && 
        ClipLine1( slladd(sllneg(dz), dw), sllsub(z1, w1), &tmin, &tmax)) {

      interpolate(&q1,p1,p2,tmin);
      interpolate(&q2,p1,p2,tmax);
      gl_transform_to_viewport(c,&q1);
      gl_transform_to_viewport(c,&q2);

      if (c->depth_test)
          ZB_line_z(c->zb,&q1.zp,&q2.zp);
      else
          ZB_line(c->zb,&q1.zp,&q2.zp);
    }
  }
}

	 
/* triangle */

/*
 * Clipping
 */

/* We clip the segment [a,b] against the 6 planes of the normal volume.
 * We compute the point 'c' of intersection and the value of the parameter 't'
 * of the intersection if x=a+t(b-a). 
 */

#ifndef SLL_DEBUG
#define clip_func(name,sign,dir,dir1,dir2) \
static GLfloat name(V4 *c,V4 *a,V4 *b) \
{\
  GLfloat t,dX,dY,dZ,dW,den;\
  dX = sllsub(b->X, a->X);\
  dY = sllsub(b->Y, a->Y);\
  dZ = sllsub(b->Z, a->Z);\
  dW = sllsub(b->W, a->W);\
  den = slladd(sllneg(sign d ## dir), dW);\
  if (sllvalue(den) == sllvalue(int2sll(0))) t=int2sll(0);\
  else t = slldiv(sllsub( sign a->dir, a->W), den);\
  c->dir1 = slladd(a->dir1, sllmul(t, d ## dir1));\
  c->dir2 = slladd(a->dir2, sllmul(t, d ## dir2));\
  c->W = slladd(a->W, sllmul(t, dW));\
  c->dir = sign c->W;\
  return t;\
}
#else
#define clip_func(name,sign,dir,dir1,dir2) \
static GLfloat name(V4 *c,V4 *a,V4 *b) \
{\
  GLfloat t,dX,dY,dZ,dW,den;\
  dX = sllsub(b->X, a->X);\
  dY = sllsub(b->Y, a->Y);\
  dZ = sllsub(b->Z, a->Z);\
  dW = sllsub(b->W, a->W);\
  den = slladd(sllneg(c->dir), dW);\
  if (sllvalue(den) == sllvalue(int2sll(0))) t=int2sll(0);\
  else t = slldiv(sllsub( a->dir, a->W), den);\
  c->dir = c->W;\
  c->dir1 = slladd(a->dir1, sllmul(t, d ## dir1));\
  c->dir2 = slladd(a->dir2, sllmul(t, d ## dir2));\
  c->W = slladd(a->W, sllmul(t, dW));\
  return t;\
}
#endif

clip_func(clip_xmin,-,X,Y,Z)

clip_func(clip_xmax,+,X,Y,Z)

clip_func(clip_ymin,-,Y,X,Z)

clip_func(clip_ymax,+,Y,X,Z)

clip_func(clip_zmin,-,Z,X,Y)

clip_func(clip_zmax,+,Z,X,Y)


GLfloat (*clip_proc[6])(V4 *,V4 *,V4 *)=  {
    clip_xmin,clip_xmax,
    clip_ymin,clip_ymax,
    clip_zmin,clip_zmax
};

static inline void updateTmp(GLContext *c,
			     GLVertex *q,GLVertex *p0,GLVertex *p1,GLfloat t)
{
  if (c->current_shade_model == GL_SMOOTH) {
    q->color.v[0]=slladd(
		    p0->color.v[0],
		    sllmul(
			    sllsub(p1->color.v[0], p0->color.v[0]),
			    t
			)
		  );
    q->color.v[1]=slladd(
		    p0->color.v[1],
		    sllmul(
			    sllsub(p1->color.v[1], p0->color.v[1]),
			    t
			)
		  );
    q->color.v[2]=slladd(
		    p0->color.v[2],
		    sllmul(
			    sllsub(p1->color.v[2], p0->color.v[2]),
			    t
			)
		  );
  } else {
    q->color.v[0]=p0->color.v[0];
    q->color.v[1]=p0->color.v[1];
    q->color.v[2]=p0->color.v[2];
  }

  if (c->texture_2d_enabled) {
    q->tex_coord.X=slladd(
		    p0->tex_coord.X,
		    sllmul(
			    sllsub(p1->tex_coord.X, p0->tex_coord.X),
			    t
			)
		    );
    q->tex_coord.Y=slladd(
		    p0->tex_coord.Y,
		    sllmul(
			    sllsub(p1->tex_coord.Y, p0->tex_coord.Y),
			    t
			)
		    );
  }

  q->clip_code=gl_clipcode(q->pc.X,q->pc.Y,q->pc.Z,q->pc.W);
  if (q->clip_code==0)
    gl_transform_to_viewport(c,q);
}

static void gl_draw_triangle_clip(GLContext *c,
                                  GLVertex *p0,GLVertex *p1,GLVertex *p2,int clip_bit);

void gl_draw_triangle(GLContext *c,
                      GLVertex *p0,GLVertex *p1,GLVertex *p2)
{
  int co,c_and,cc[3],front;
  GLfloat norm;
  
  cc[0]=p0->clip_code;
  cc[1]=p1->clip_code;
  cc[2]=p2->clip_code;
  
  co=cc[0] | cc[1] | cc[2];

  /* we handle the non clipped case here to go faster */
  if (co==0) {
    
      norm=sllsub(
	      sllmul(int2sll(p1->zp.x-p0->zp.x), int2sll(p2->zp.y-p0->zp.y)) ,
              sllmul(int2sll(p2->zp.x-p0->zp.x), int2sll(p1->zp.y-p0->zp.y))
	     );
      
      if (sllvalue(norm) == sllvalue(int2sll(0))) return;

      front = sllvalue(norm) < sllvalue(int2sll(0));
      front = front ^ c->current_front_face;
  
      /* back face culling */
      if (c->cull_face_enabled) {
        /* most used case first */
        if (c->current_cull_face == GL_BACK) {
          if (front == 0) return;
          c->draw_triangle_front(c,p0,p1,p2);
        } else if (c->current_cull_face == GL_FRONT) {
          if (front != 0) return;
          c->draw_triangle_back(c,p0,p1,p2);
        } else {
          return;
        }
      } else {
        /* no culling */
        if (front) {
          c->draw_triangle_front(c,p0,p1,p2);
        } else {
          c->draw_triangle_back(c,p0,p1,p2);
        }
      }
  } else {
    c_and=cc[0] & cc[1] & cc[2];
    if (c_and==0) {
      gl_draw_triangle_clip(c,p0,p1,p2,0);
    }
  }
}

static void gl_draw_triangle_clip(GLContext *c,
                                  GLVertex *p0,GLVertex *p1,GLVertex *p2,int clip_bit)
{
  int co,c_and,co1,cc[3],edge_flag_tmp,clip_mask;
  GLVertex tmp1,tmp2,*q[3];
  GLfloat tt;
  
  cc[0]=p0->clip_code;
  cc[1]=p1->clip_code;
  cc[2]=p2->clip_code;
  
  co=cc[0] | cc[1] | cc[2];
  if (co == 0) {
    gl_draw_triangle(c,p0,p1,p2);
  } else {
    c_and=cc[0] & cc[1] & cc[2];
    /* the triangle is completely outside */
    if (c_and!=0) return;

    /* find the next direction to clip */
    while (clip_bit < 6 && (co & (1 << clip_bit)) == 0)  {
      clip_bit++;
    }

    /* this test can be true only in case of rounding errors */
    if (clip_bit == 6) {
#if 0
      printf("Error:\n");
      printf("%f %f %f %f\n",p0->pc.X,p0->pc.Y,p0->pc.Z,p0->pc.W);
      printf("%f %f %f %f\n",p1->pc.X,p1->pc.Y,p1->pc.Z,p1->pc.W);
      printf("%f %f %f %f\n",p2->pc.X,p2->pc.Y,p2->pc.Z,p2->pc.W);
#endif
      return;
    }
  
    clip_mask = 1 << clip_bit;
    co1=(cc[0] ^ cc[1] ^ cc[2]) & clip_mask;
    
    if (co1)  { 
      /* one point outside */

      if (cc[0] & clip_mask) { q[0]=p0; q[1]=p1; q[2]=p2; }
      else if (cc[1] & clip_mask) { q[0]=p1; q[1]=p2; q[2]=p0; }
      else { q[0]=p2; q[1]=p0; q[2]=p1; }
      
      tt=clip_proc[clip_bit](&tmp1.pc,&q[0]->pc,&q[1]->pc);
      updateTmp(c,&tmp1,q[0],q[1],tt);

      tt=clip_proc[clip_bit](&tmp2.pc,&q[0]->pc,&q[2]->pc);
      updateTmp(c,&tmp2,q[0],q[2],tt);

      tmp1.edge_flag=q[0]->edge_flag;
      edge_flag_tmp=q[2]->edge_flag;
      q[2]->edge_flag=0;
      gl_draw_triangle_clip(c,&tmp1,q[1],q[2],clip_bit+1);

      tmp2.edge_flag=1;
      tmp1.edge_flag=0;
      q[2]->edge_flag=edge_flag_tmp;
      gl_draw_triangle_clip(c,&tmp2,&tmp1,q[2],clip_bit+1);
    } else {
      /* two points outside */

      if ((cc[0] & clip_mask)==0) { q[0]=p0; q[1]=p1; q[2]=p2; }
      else if ((cc[1] & clip_mask)==0) { q[0]=p1; q[1]=p2; q[2]=p0; } 
      else { q[0]=p2; q[1]=p0; q[2]=p1; }
      
      tt=clip_proc[clip_bit](&tmp1.pc,&q[0]->pc,&q[1]->pc);
      updateTmp(c,&tmp1,q[0],q[1],tt);

      tt=clip_proc[clip_bit](&tmp2.pc,&q[0]->pc,&q[2]->pc);
      updateTmp(c,&tmp2,q[0],q[2],tt);
      
      tmp1.edge_flag=1;
      tmp2.edge_flag=q[2]->edge_flag;
      gl_draw_triangle_clip(c,q[0],&tmp1,&tmp2,clip_bit+1);
    }
  }
}


void gl_draw_triangle_select(GLContext *c,
                             GLVertex *p0,GLVertex *p1,GLVertex *p2)
{
  gl_add_select1(c,p0->zp.z,p1->zp.z,p2->zp.z);
}

void gl_draw_triangle_fill(GLContext *c,
                           GLVertex *p0,GLVertex *p1,GLVertex *p2)
{
  if (c->texture_2d_enabled) {
    ZB_setTexture(c->zb,(PIXEL *)c->current_texture->images[0].pixmap);
    ZB_fillTriangleMappingPerspective(c->zb,&p0->zp,&p1->zp,&p2->zp);
  } else if (c->current_shade_model == GL_SMOOTH) {
    ZB_fillTriangleSmooth(c->zb,&p0->zp,&p1->zp,&p2->zp);
  } else {
    ZB_fillTriangleFlat(c->zb,&p0->zp,&p1->zp,&p2->zp);
  }
}

/* Render a clipped triangle in line mode */  

void gl_draw_triangle_line(GLContext *c,
                           GLVertex *p0,GLVertex *p1,GLVertex *p2)
{
    if (c->depth_test) {
        if (p0->edge_flag) ZB_line_z(c->zb,&p0->zp,&p1->zp);
        if (p1->edge_flag) ZB_line_z(c->zb,&p1->zp,&p2->zp);
        if (p2->edge_flag) ZB_line_z(c->zb,&p2->zp,&p0->zp);
    } else {
        if (p0->edge_flag) ZB_line(c->zb,&p0->zp,&p1->zp);
        if (p1->edge_flag) ZB_line(c->zb,&p1->zp,&p2->zp);
        if (p2->edge_flag) ZB_line(c->zb,&p2->zp,&p0->zp);
    }
}



/* Render a clipped triangle in point mode */
void gl_draw_triangle_point(GLContext *c,
                            GLVertex *p0,GLVertex *p1,GLVertex *p2)
{
  if (p0->edge_flag) ZB_plot(c->zb,&p0->zp);
  if (p1->edge_flag) ZB_plot(c->zb,&p1->zp);
  if (p2->edge_flag) ZB_plot(c->zb,&p2->zp);
}




