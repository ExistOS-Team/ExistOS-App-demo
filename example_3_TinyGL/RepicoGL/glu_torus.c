#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

void drawTorus(GLfloat rc, int numc, GLfloat rt, int numt)
{
    int i, j, k;
    GLdouble s, t;
    GLdouble x, y, z;
    GLdouble pi, twopi;
    GLfloat fnumc=int2sll(numc);
    GLfloat fnumt=int2sll(numt);

    pi = dbl2sll(M_PI);
    twopi = sllmul(int2sll(2), pi);
 
    for (i = 0; i < numc; i++) {
	glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= numt; j++) {
	    for (k = 1; k >= 0; k--) {
		s = slladd(int2sll((i + k) % numc), dbl2sll(0.5));
		t = int2sll(j % numt);

		x = sllmul(sllcos(slldiv(sllmul(t,twopi),fnumt)), sllcos(slldiv(sllmul(s,twopi),fnumc)));
		y = sllmul(sllsin(slldiv(sllmul(t,twopi),fnumt)), sllcos(slldiv(sllmul(s,twopi),fnumc)));
		z = sllsin(slldiv(sllmul(s,twopi),fnumc));
		glNormal3f(x, y, z);

		x = sllmul(slladd(rt, sllmul(rc, sllcos(slldiv(sllmul(s,twopi),fnumc)))), sllcos(slldiv(sllmul(t,twopi),fnumt)));
		y = sllmul(slladd(rt, sllmul(rc, sllcos(slldiv(sllmul(s,twopi),fnumc)))), sllsin(slldiv(sllmul(t,twopi),fnumt)));
		z = sllmul(rc, sllsin(slldiv(sllmul(s,twopi),fnumc)));
		glVertex3f(x, y, z);
	    }
        }
	glEnd();
    }
}

