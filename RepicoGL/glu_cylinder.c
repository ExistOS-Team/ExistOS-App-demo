#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

static inline void normal3f( GLfloat x, GLfloat y, GLfloat z )
{
   GLdouble mag;

   mag = sllsqrt( slladd(slladd(sllmul(x,x), sllmul(y,y)), sllmul(z,z)) );
   if (sllvalue(mag)>sllvalue(dbl2sll(0.00001))) {
      x = slldiv(x, mag);
      y = slldiv(y, mag);
      z = slldiv(z, mag);
   }
   glNormal3f( x, y, z );
}


void gluCylinder( GLUquadricObj *qobj,
                  GLdouble baseRadius, GLdouble topRadius, GLdouble height,
                  GLint slices, GLint stacks )
{
   GLdouble da, r, dr, dz;
   GLfloat z, nz, nsign;
   GLint i, j;
   GLfloat du = slldiv(int2sll(1), int2sll(slices));
   GLfloat dv = slldiv(int2sll(1), int2sll(stacks));
   GLfloat tcx = int2sll(0), tcy = int2sll(0);
   
   nsign = int2sll(1);

   da = slldiv(dbl2sll(2.0*M_PI), int2sll(slices));
   dr = slldiv(sllsub(topRadius, baseRadius), int2sll(stacks));
   dz = slldiv(height, int2sll(stacks));
   nz = slldiv(sllsub(baseRadius, topRadius), height);  /* Z component of normal vectors */

   for (i=0;i<slices;i++) {
	 GLfloat ifl = int2sll(i);
	 GLfloat i1fl = int2sll(i+1);
	 GLfloat x1 = sllneg(sllsin(sllmul(ifl, da)));
	 GLfloat y1 = sllcos(sllmul(ifl, da));
	 GLfloat x2 = sllneg(sllsin(sllmul(i1fl, da)));
	 GLfloat y2 = sllcos(sllmul(i1fl, da));
	 z = int2sll(0);
	 r = baseRadius;
	 tcy = int2sll(0);
	 glBegin( GL_QUAD_STRIP );
	 for (j=0;j<=stacks;j++) {
	    if (sllvalue(nsign)==sllvalue(int2sll(1))) {
	       normal3f( sllmul(x1,nsign), sllmul(y1,nsign), sllmul(nz,nsign) );
	       glTexCoord2f(tcx, tcy);
	       glVertex3f( sllmul(x1,r), sllmul(y1,r), z );
	       normal3f( sllmul(x2,nsign), sllmul(y2,nsign), sllmul(nz,nsign) );
	       glTexCoord2f( slladd(tcx,du), tcy);
	       glVertex3f( sllmul(x2,r), sllmul(y2,r), z );
	    }
	    else {
	       normal3f( sllmul(x2,nsign), sllmul(y2,nsign), sllmul(nz,nsign) );
	       glTexCoord2f(tcx, tcy);
	       glVertex3f( sllmul(x2,r), sllmul(y2,r), z );
	       normal3f( sllmul(x1,nsign), sllmul(y1,nsign), sllmul(nz,nsign) );
	       glTexCoord2f( slladd(tcx,du), tcy);
	       glVertex3f( sllmul(x1,r), sllmul(y1,r), z );
	    }
	    z = slladd(z, dz);
	    r = slladd(r, dr);
	    tcy = slladd(tcy, dv);
	 }
	 glEnd();
	 tcx = slladd(tcx, du);
      }
}

