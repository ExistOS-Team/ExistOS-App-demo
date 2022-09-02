#include <stdint.h>
#include <stdio.h>

#include "syscall.h"
#include "ff.h"

uint8_t vram[256*127];

FIL *f;

int main() 
{
    ll_cpu_slowdown_enable(false);

    for(int y = 0; y < 127; y++)
    {
        for(int x = 0; x < 256; x++)
        {
            vram[x + y * 256] = x;
        }
    }

    ll_disp_put_area(vram, 0, 0, 255, 126);

    f = pvPortMalloc(sizeof(FIL));

    f_open(f, "/test.txt", FA_CREATE_ALWAYS | FA_WRITE);

    f_printf(f, "hello world!\n");

    f_close(f);

    vTaskDelay(pdMS_TO_TICKS(3000));

    vPortFree(f);

    return 0;
}


