/**
 * FBDev GLX-like backend
 */

#ifndef GLX_H
#define GLX_H

#include <assert.h>
#include <errno.h>
#include <GL/gl.h>
#include <math.h>

#include <string.h>
#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct TinyFBDevGLXContext FBDevGLXContext;

extern FBDevGLXContext *fbdev_glXCreateContext();

extern void fbdev_glXDestroyContext();

extern int fbdev_glXMakeCurrent(FBDevGLXContext *ctx);

extern void fbdev_glXSwapBuffers(FBDevGLXContext *ctx);

extern int fbdev_getLineLength(FBDevGLXContext *ctx);


#ifdef __cplusplus
}
#endif

#endif
