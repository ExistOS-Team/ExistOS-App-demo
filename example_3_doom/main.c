#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "existosapi/syscall.h"
#include "existosapi/basic_api.h"
#include "existosapi/keyboard_gii39.h"

int putchar(int c)
{
    fwrite(&c, 1, 1, stdout);
    return c;
}

uint8_t vram[320*256];
int main() 
{
    printf("start\n");
    ll_cpu_slowdown_enable(false);
    api_vram_initialize(vram);

    for(int y = 0; y < 127; y++)
    {
        for(int x = 0; x < 256; x++)
        { 
            vram[x + y * 256] = x;
        }
    }

    api_vram_put_string(5,6,"Hello World", 0, 255, 16);

    ll_disp_put_area(vram, 0, 0, 255, 126);

    extern void doom_main(int argc, char *argv[]);

    char *argv[] = {"doom", "-iwad", "doom.wad", NULL};
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    doom_main(argc, argv);
    
    printf("exit..\n");


    return 0;
}


