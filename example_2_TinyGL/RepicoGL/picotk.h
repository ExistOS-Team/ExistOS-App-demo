#ifndef PICOTK_H
#define PICOTK_H

#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * tk like ui
 */
void draw( void );
int loop( void );
void reshape( int width, int height );
void picotk_init( void );
int ui_loop(int argc, char **argv, const char *name);
void tkSwapBuffers(void);

#ifdef __cplusplus
}
#endif
/*
#define KEY_UP     0xe000
#define KEY_DOWN   0xe001
#define KEY_LEFT   0xe002
#define KEY_RIGHT  0xe003
#define KEY_ESCAPE 0xe004
*/
#endif
