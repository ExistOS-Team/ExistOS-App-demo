#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "existosapi/basic_api.h"
#include "existosapi/keyboard_gii39.h"
#include "existosapi/syscall.h"
#include "existosapi/SystemUI.h"

#include "FreeRTOS.h"
#include "task.h"

static uint8_t vram[256*128];

static void task_A(void *par) 
{
    int i = 0;
    char buf[64];
    for(;;)
    {
        sprintf(buf, "Thread A %d Count:%d", (int)par, i);
        api_vram_put_string(0, 32 + ((int)par)*16, buf, 0, 255, 16);    //在屏幕缓存中上输出线程1的计数值
        i++;
        vTaskDelay(pdMS_TO_TICKS(50));  //延迟50ms
    }
    //线程中禁止返回，需要使用vTaskDelete(NULL)销毁自身，或在其它线程中销毁本线程
    
}

static void task_B(void *par)
{
    int i = 0;
    char buf[64];
    for(;;)
    {
        sprintf(buf, "Thread B %d Count:%d", (int)par, i);
        api_vram_put_string(0, 32 + ((int)par)*16, buf, 0, 255, 16);    //在屏幕缓存中上输出线程2的计数值
        i++;
        vTaskDelay(pdMS_TO_TICKS(50));  //延迟50ms
    }
    //线程中禁止返回，需要使用vTaskDelete(NULL)销毁自身，或在其它线程中销毁本线程
}

int main()
{
    TaskHandle_t t1, t2, t3, t4;

    api_vram_initialize(vram);  //初始化帧缓存
    api_vram_clear(0xFF);       //用白色清屏
    api_vram_put_string(0,0, "Multi Thread Test", 0, 255, 12);
    api_vram_put_string(0,12, "Press F6 to Exit", 0, 255, 12);

    //创建两个计数的线程
    //          函数指针   线程名    线程栈大小   传入参数         优先级(0~4)              线程句柄
    xTaskCreate(task_A, "Thread 1", 1000,        (void *)1,  configMAX_PRIORITIES - 3,  &t1);
    xTaskCreate(task_B, "Thread 2", 1000,        (void *)2,  configMAX_PRIORITIES - 3,  &t2);
    xTaskCreate(task_A, "Thread 3", 1000,        (void *)3,  configMAX_PRIORITIES - 3,  &t3);
    xTaskCreate(task_B, "Thread 4", 1000,        (void *)4,  configMAX_PRIORITIES - 3,  &t4);

    while(1)    //主线程循环复制刷新屏幕和判断按键
    {
        api_vram_flush();             //将帧缓存刷新到屏幕上显示
        if(api_get_key(KEY_F6))       //空转等待判断按下F6后退出
        {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    vTaskDelete(t1);    //销毁线程
    vTaskDelete(t2);    
    vTaskDelete(t3);    
    vTaskDelete(t4);    

    return 0;
}
