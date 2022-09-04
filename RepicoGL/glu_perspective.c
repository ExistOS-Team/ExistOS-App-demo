#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

void gluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = sllmul(zNear, slltan( sllmul(fovy, dbl2sll(M_PI/360.0)) ));
   ymin = sllneg(ymax);

   xmin = sllmul(ymin, aspect);
   xmax = sllmul(ymax, aspect);

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

