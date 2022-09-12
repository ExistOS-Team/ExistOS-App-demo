#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "existosapi/basic_api.h"
#include "existosapi/keyboard_gii39.h"
#include "existosapi/syscall.h"

#include "existosapi/SystemUI.h"

static uint8_t vram[256 * 128]; //创建帧缓存

int main()
{
    ll_cpu_slowdown_enable(false);  //关闭CPU自动降频

    api_vram_initialize(vram);      //初始化帧缓存

    for (int y = 0; y < 127; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            vram[x + y * 256] = x;  //绘制灰度渐变条纹
        }
    }

    api_vram_put_string(5, 6,  " Hello World ", 0, 255, 16);
    api_vram_put_string(5, 110, " Press F6 to Exit. ", 0, 255, 12);
    api_vram_flush();               //刷新显存显示
    
    while(1)
    {
        if(api_get_key(-1) == KEY_F6)       //判断按下F6后退出
        {
            return 0;
        }
    }

    return 0;
}
