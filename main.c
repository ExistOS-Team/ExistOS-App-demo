#include <stdint.h>
#include <stdio.h>

#include "syscall.h"

extern void dlsyn_test(int a);
extern void dlsyn_test2(int a);

uint8_t vram[256*127];

int main() 
{

    void lcd_test();
    lcd_test();

    for(int y = 0; y < 127; y++)
    {
        for(int x = 0; x < 256; x++)
        {
            vram[x + y * 256] = x;
        }
    }

    ll_disp_put_area(vram, 0, 0, 255, 126);

    ll_put_str("hello 4\n"); 

    //sysApiFuncList->taskSleepMs(3000);

    //ll_disp_put_area(0, 0, 0, 255, 126);


    dlsyn_test(1);
    dlsyn_test2(2);
    printf("qwe\n");

    return 0;
}


