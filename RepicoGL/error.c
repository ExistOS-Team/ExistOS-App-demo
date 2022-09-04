#include <stdarg.h>
#include <stdio.h>

void gl_fatal_error(char *format, ...)
{
  va_list ap;

  va_start(ap, format);

  fprintf(stderr, "PicoGL: fatal error: ");
  vfprintf(stderr, format,ap);
  fprintf(stderr, "\n");

  va_end(ap);
}
