
#include <sys/types.h>
#include <stdlib.h>

#include "syscall.h"

apiFuncList_t *sysApiFuncList;

extern unsigned int _sbss;
extern unsigned int _ebss;
extern unsigned int __init_data;
extern unsigned int __data_start;
extern unsigned int __data_end;

int main();

int __attribute__((section(".init"))) __attribute__((naked)) __attribute__((target("arm"))) _entry() 
{
    __asm volatile(".word 0xA55AAA55"); 
    __asm volatile(".word 1936291909"); 
    __asm volatile(".word 1347436916"); //"ExistAPP"

    __asm volatile("push {r14}");
    __asm volatile("push {r0}");

    for (char *i = (char *)&_sbss; i < (char *)&_ebss; i++) {
        *i = 0; // clear bss
    }

    uint32_t *pui32Src, *pui32Dest;
    pui32Src = (uint32_t *)&__init_data;
    for (pui32Dest =  (uint32_t *)&__data_start; pui32Dest <  (uint32_t *)&__data_end;) {
        *pui32Dest++ = *pui32Src++;
    }

    typedef void (*pfunc)();
    extern pfunc __ctors_start__[];
    extern pfunc __ctors_end__[];
    pfunc *p;
    for (p = __ctors_start__; p < __ctors_end__; p++) {
        (*p)();
    }

    __asm volatile("pop {r0}");
    __asm volatile("ldr r1,=sysApiFuncList");
    __asm volatile("str r0, [r1]");

    main();

    __asm volatile("pop {r15}");


}

