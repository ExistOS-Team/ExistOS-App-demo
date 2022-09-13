
#include <sys/types.h>
#include <stdlib.h>

#include "existosapi/syscall.h"

#include "FreeRTOS.h"
#include "task.h"

extern unsigned int _sbss;
extern unsigned int _ebss;
extern unsigned int __init_data;
extern unsigned int __data_start;
extern unsigned int __data_end;

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));
extern void (*__fini_array_start[])(void) __attribute__((weak));
extern void (*__fini_array_end[])(void) __attribute__((weak));

extern void exp_app_exit();
int main();

int __attribute__((section(".init"))) __attribute__((naked)) __attribute__((target("arm"))) _start()
{

    for (char *i = (char *)&_sbss; i < (char *)&_ebss; i++)
    {
        *i = 0; // clear bss
    }

    for (void (**pp)() = __preinit_array_start; pp < __preinit_array_end; ++pp)
        (**pp)();
    // TODO: Init.
    for (void (**pp)() = __init_array_start; pp < __init_array_end; ++pp)
        (**pp)();

    main();

    for (void (**pp)() = __fini_array_start; pp < __fini_array_end; ++pp)
        (**pp)();

    exp_app_exit();
    vTaskDelete(NULL);
    for (;;)
        ;
}
