#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "existosapi/basic_api.h"
#include "existosapi/keyboard_gii39.h"
#include "existosapi/syscall.h"

#include "existosapi/SystemUI.h"

static uint8_t vram[256 * 128]; //创建帧缓存

int cm_main(void);
extern float cm_score;
extern int cm_validated;
int main()
{
    char str[256];
    api_vram_initialize(vram);      //初始化帧缓存


    api_vram_clear(255);
    api_vram_put_string(5, 0,  "Core Mark Running...", 0, 255, 16);
    api_vram_flush();               //刷新显存显示
    cm_main(); 


    api_vram_clear(255);
    api_vram_put_string(5, 0,  "Finish.", 0, 255, 16);
    if(cm_validated)
    {
        sprintf(str,"Score:%f Iter/sec", cm_score);
        api_vram_put_string(5, 16*1,  str, 0, 255, 16);
        sprintf(str,"      %f CM/MHz", (float)cm_score / ll_get_cur_freq());
        api_vram_put_string(5, 16*2,  str, 0, 255, 16);
        sprintf(str,"CPU: %d MHz", ll_get_cur_freq());
        api_vram_put_string(5, 16*3,  str, 0, 255, 16);
        
    }else{
        api_vram_put_string(5, 16*1,  "ERROR", 0, 255, 16);
    }
    
    api_vram_put_string(5, 16*5,  "Press any key to exit.", 0, 255, 16);
    api_vram_flush();               //刷新显存显示


    while(1)
    {
        if(api_get_key(-1))       //判断按下F6后退出
        {
            return 0;
        }
    }

    return 0;
}
