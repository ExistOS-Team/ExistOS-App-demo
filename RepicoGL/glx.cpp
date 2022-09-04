/**
 *  FBDev GLX-like backend
 */

#include <GL/glx.h>
#include <GL/internal/zgl.h>
#include "screen_config.h"
#include "glx_impl.h"
#include "debugger.h"

struct TinyFBDevGLXContext {
    GLContext *gl_context; /* base class */
    void *screen;
    ZBuffer *zb; // 绘制用buffer
    int line_length;
    unsigned int *palette;
    unsigned char *indexes;
};


/* Create context */
FBDevGLXContext *fbdev_glXCreateContext() {
    FBDevGLXContext *ctx;

    ctx = (FBDevGLXContext *) malloc(sizeof(FBDevGLXContext));
    if (ctx == NULL)
        return NULL;

    ctx->gl_context = NULL;
    ctx->screen = NULL;
    ctx->zb = NULL;
    ctx->line_length = 0;
    return ctx;
}

/*! Destroy context */
void fbdev_glXDestroyContext(FBDevGLXContext *ctx) {
    if (ctx->gl_context != NULL) {
        glClose();
    }

    free(ctx);
}

/* resize the glx viewport */
static int glX_resize_viewport(GLContext *c,
                               int *xsize_ptr,
                               int *ysize_ptr) {
    FBDevGLXContext *ctx;
    int xsize;
    int ysize;

    ctx = (FBDevGLXContext *) c->opaque;

    xsize = *xsize_ptr;
    ysize = *ysize_ptr;

    /* we ensure that xsize and ysize are multiples of 2 for the zbuffer.
       TODO: find a better solution */
    xsize &= ~3;
    ysize &= ~3;

    if (xsize == 0 || ysize == 0)
        return -1;

    *xsize_ptr = xsize;
    *ysize_ptr = ysize;

    /* resize the Z buffer */
    ZB_resize(c->zb, ctx->zb->pbuf, xsize, ysize);
    return 0;
}


/* we assume here that drawable is a buffer */
int fbdev_glXMakeCurrent(FBDevGLXContext *ctx) {
    int mode;
    int xsize;
    int ysize;
    int n_colors = 0;
    int bpp;
    int line_length;
    ZBuffer *zb;
    extern void *FrameBuffer;

    if (ctx->gl_context == NULL) {
        printf("info:glx.cpp: start make gl_context\n");
        
        /* create the PicoGL context */
        xsize = SCREEN_WIDTH;
        ysize = SCREEN_HEIGHT;
        bpp = SCREEN_BITS_PER_PIXEL;

        line_length = SCREEN_WIDTH;
        switch (bpp) {
            case 8:
                ctx->indexes =
                        (unsigned char *) malloc(ZB_NB_COLORS);
                if (ctx->indexes == NULL)
                    return 0;
                for (mode = 0; mode < ZB_NB_COLORS ;mode++)
                    ctx->indexes[mode] = mode;

                ctx->palette =
                        (unsigned int *) calloc(
                                ZB_NB_COLORS,
                                sizeof(int));
                if (ctx->palette == NULL) {
                    free(ctx->indexes);
                    ctx->indexes = NULL;
                    return 0;
                }

                line_length = line_length * 2;
                n_colors = ZB_NB_COLORS;
                mode = ZB_MODE_INDEX;
                break;
            case 16:
                line_length = line_length * 2;
                mode = ZB_MODE_5R6G5B;
                break;
            case 32:
                mode = ZB_MODE_RGBA;
                break;
            case 24:
                line_length = (line_length * 2) / 3;
                mode = ZB_MODE_RGB24;
                break;
            default:
                mode = ZB_MODE_5R6G5B;
                break;
        }
        
        printf("debug:glx.cpp: start make zbuffer (%d, %d, %d, %d)\n", xsize, ysize, mode, n_colors);
        
        zb = ZB_open(xsize, ysize, mode, n_colors,
                     (unsigned char*) ctx->indexes, (int *)ctx->palette, NULL);

        if (zb == NULL) {
            if (ctx->indexes != NULL) {
                free(ctx->indexes);
                ctx->indexes = NULL;
            }
            if (ctx->palette != NULL) {
                free(ctx->palette);
                ctx->palette = NULL;
            }
            printf("error:glx.cpp: cannot create zbuffer\n");
            return 0;
        }

        /* initialisation of the TinyGL interpreter */
        glInit(zb);
        
        ctx->gl_context = gl_get_context();
        printf("info:glx.cpp: succ create gl_context at %x\n", ctx->gl_context);
        
        ctx->gl_context->opaque = (void *) ctx;
        ctx->gl_context->gl_resize_viewport = glX_resize_viewport;

        /* set the viewport */
        /*  TIS: !!! HERE SHOULD BE -1 on both to force reshape  */
        /*  which is needed to make sure initial reshape is  */
        /*  called, otherwise it is not called..  */
        ctx->gl_context->viewport.xsize = xsize;
        ctx->gl_context->viewport.ysize = ysize;
        ctx->line_length = line_length; // line_length;
        glViewport(0, 0, xsize, ysize);
    }

    ctx->screen = FrameBuffer;

    ctx->zb = zb;
    return 1;
}

int cnt = 0;

/* Swap buffers */
void fbdev_glXSwapBuffers(FBDevGLXContext *ctx) {
    GLContext *gl_context;

    assert(ctx->gl_context);
    assert(ctx->screen);
    assert(ctx->zb);

    gl_context = gl_get_context();
    ctx = (FBDevGLXContext *) gl_context->opaque;

    ZB_copyFrameBuffer(ctx->zb, ctx->screen, ctx->line_length);
}

int fbdev_getLineLength(FBDevGLXContext *ctx) {
    return ctx->line_length;
}

