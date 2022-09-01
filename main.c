#include <stdint.h>

#include "syscall.h"
extern apiFuncList_t *sysApiFuncList;

uint8_t vram[256*127];


int main()
{
    for(int y = 0; y < 127; y++)
    {
        for(int x = 0; x < 256; x++)
        {
            vram[x + y * 256] = x;
        }
    }

    ll_disp_put_area(vram, 0, 0, 255, 126);

    sysApiFuncList->taskSleepMs(3000);

    return 0;
}


