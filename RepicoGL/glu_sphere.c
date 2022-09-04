#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

/*
 * Sphère (adapted from Mesa)
 */

void gluSphere(GLUquadricObj *qobj,
               GLfloat radius,int slices,int stacks)
{
   GLfloat rho, drho, theta, dtheta;
   GLfloat x, y, z;
   GLfloat s, t, ds, dt;
   int i, j, imin, imax;
   int normals;
   GLfloat nsign;
  
   normals=1;
   nsign=int2sll(1);

   drho = slldiv(dbl2sll(M_PI), int2sll(stacks));
   dtheta = slldiv(dbl2sll(2.0*M_PI), int2sll(slices));

  /* draw +Z end as a triangle fan */
  glBegin( GL_TRIANGLE_FAN );
  glNormal3f( int2sll(0), int2sll(0), int2sll(1) );
  glTexCoord2f(dbl2sll(0.5), int2sll(0));
  glVertex3f( int2sll(0), int2sll(0), sllmul(nsign, radius) );
  for (j=0;j<=slices;j++) {
	 theta = (j==slices) ? int2sll(0) : sllmul(int2sll(j), dtheta);
	 x = sllmul(sllneg(sllsin(theta)), sllsin(drho));
	 y = sllmul(sllcos(theta), sllsin(drho));
	 z = sllmul(nsign, sllcos(drho));
	 if (normals)  glNormal3f( sllmul(x,nsign), sllmul(y,nsign), sllmul(z,nsign) );
	 glVertex3f( sllmul(x,radius), sllmul(y,radius), sllmul(z,radius) );
      }
   glEnd();


      ds = slldiv(int2sll(1), int2sll(slices));
      dt = slldiv(int2sll(1), int2sll(stacks));
      t = int2sll(1);  /* because loop now runs from 0 */
      if (1) {
        imin = 0;
        imax = stacks;
      }
      else {
        imin = 1;
        imax = stacks-1;
      }

      /* draw intermediate stacks as quad strips */
      for (i=imin;i<imax;i++) {
	 rho = sllmul(int2sll(i), drho);
	 glBegin( GL_QUAD_STRIP );
         s = int2sll(0);
	 for (j=0;j<=slices;j++) {
	    theta = (j==slices) ? int2sll(0) : sllmul(int2sll(j), dtheta);
	    x = sllmul(sllneg(sllsin(theta)), sllsin(rho));
	    y = sllmul(sllcos(theta), sllsin(rho));
	    z = sllmul(nsign, sllcos(rho));
	    if (normals)  glNormal3f( sllmul(x,nsign), sllmul(y,nsign), sllmul(z,nsign) );
	    glTexCoord2f(s,sllsub(int2sll(1),t));
	    glVertex3f( sllmul(x,radius), sllmul(y,radius), sllmul(z,radius) );
	    x = sllmul(sllneg(sllsin(theta)), sllsin(slladd(rho,drho)));
	    y = sllmul(sllcos(theta), sllsin(slladd(rho,drho)));
	    z = sllmul(nsign, sllcos(slladd(rho,drho)));
	    if (normals)  glNormal3f( sllmul(x,nsign), sllmul(y,nsign), sllmul(z,nsign) );
	    glTexCoord2f(s,sllsub(int2sll(1), sllsub(t,dt)));
            s = slladd(s, ds);
	    glVertex3f( sllmul(x,radius), sllmul(y,radius), sllmul(z,radius) );
	 }
	 glEnd();
	 t = sllsub(t, dt);
      }

/* draw -Z end as a triangle fan */
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( int2sll(0), int2sll(0), int2sll(-1) );
      glTexCoord2f(dbl2sll(0.5), int2sll(1));
      glVertex3f( int2sll(0), int2sll(0), sllneg(sllmul(radius,nsign)) );
      rho = sllsub(dbl2sll(M_PI), drho);
      s = int2sll(1);
      t = dt;
      for (j=slices;j>=0;j--) {
	 theta = (j==slices) ? int2sll(0) : sllmul(int2sll(j), dtheta);
	 x = sllmul(sllneg(sllsin(theta)), sllsin(rho));
	 y = sllmul(sllcos(theta), sllsin(rho));
	 z = sllmul(nsign, sllcos(rho));
	 if (normals)  glNormal3f( sllmul(x,nsign), sllmul(y,nsign), sllmul(z,nsign) );
	 glTexCoord2f(s,sllsub(int2sll(1), t));
         s = sllsub(s, ds);
	 glVertex3f( sllmul(x,radius), sllmul(y,radius), sllmul(z,radius) );
      }
      glEnd();
}
