#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

/* Disk (adapted from Mesa) */

void gluDisk( GLUquadricObj *qobj,
              GLdouble innerRadius, GLdouble outerRadius,
              GLint slices, GLint loops )
{
   GLdouble a, da;
   GLfloat dr;
   GLfloat r1, r2, dtc;
   GLint s, l;
   GLfloat sa,ca;

   sll tmp05=dbl2sll(0.5);

   /* Normal vectors */
	 glNormal3f( int2sll(0), int2sll(0), int2sll(1) );

   da = slldiv(dbl2sll(2.0*M_PI), int2sll(slices));
   dr = slldiv(sllsub(outerRadius, innerRadius), int2sll(loops));

   /* texture of a gluDisk is a cut out of the texture unit square */
   /* x, y in [-outerRadius, +outerRadius]; s, t in [0, 1] (linear mapping) */
   dtc = sllmul(int2sll(2), outerRadius);

   r1 = innerRadius;
   for (l=0;l<loops;l++) {
	    r2 = slladd(r1, dr);
	       glBegin( GL_QUAD_STRIP );
	       for (s=0;s<=slices;s++) {
		  if (s==slices) a = int2sll(0);
		  else  a = sllmul(int2sll(s), da);
		  sa = sllsin(a); ca = sllcos(a);
                  glTexCoord2f(slladd(tmp05, slldiv(sllmul(sa,r2),dtc)),
			       slladd(tmp05, slldiv(sllmul(ca,r2),dtc)));
                  glVertex2f( sllmul(r2,sa), sllmul(r2,ca) );
                  glTexCoord2f(slladd(tmp05, slldiv(sllmul(sa,r1),dtc)),
			       slladd(tmp05, slldiv(sllmul(ca,r1),dtc)));
                  glVertex2f( sllmul(r1,sa), sllmul(r1,ca) );
	       }
	       glEnd();
	    r1 = r2;
	 }

}

