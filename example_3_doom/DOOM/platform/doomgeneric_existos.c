

#include "existosapi/syscall.h"
#include "existosapi/keyboard_gii39.h"
#include "existosapi/basic_api.h"

#include <FreeRTOS.h>
#include <task.h>
#include "doomkeys.h"



void DG_Init()
{

}

static int drew_first_frame = 0;

void DG_DrawFrame()
{

}

void DG_SleepMs(uint32_t ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t DG_GetTicksMs()
{
	uint32_t tmr = ll_get_time_ms();
	// printf("cms:%d\n", tmr);
	return tmr;
}


int DG_GetKey(int *pressed, unsigned char *key)
{
	extern bool menuactive;
	static uint32_t lastKey;
	uint32_t keys = ll_vm_check_key();

	if (keys != lastKey)
	{

		uint32_t up_key = keys & 0xFFFF;
		uint32_t press = keys >> 16;

		*pressed = press;

		switch (up_key)
		{
		case KEY_UP:
			*key = KEY_UPARROW;
			break;
		case KEY_DOWN:
			*key = KEY_DOWNARROW;
			break;
		case KEY_LEFT:
			*key = KEY_LEFTARROW;
			break;
		case KEY_RIGHT:
			*key = KEY_RIGHTARROW;
			break;
		case KEY_8:
			*key = KEY_UPARROW;
			break;
		case KEY_2:
			*key = KEY_DOWNARROW;
			break;
		case KEY_4:
			*key = KEY_LEFTARROW;
			break;
		case KEY_6:
			*key = KEY_RIGHTARROW;
			break;
		case KEY_7:
			*key = KEY_ESCAPE;
			break;
		case KEY_9:
			*key = KEY_TAB;
			break;
		case KEY_1:
			*key = KEYP_MINUS;
			break;
		case KEY_3:
			*key = KEYP_PLUS;
			break;

		case KEY_NUM:
		{
			if (menuactive)
			{
				*key = KEY_ENTER;
			}
			else
			{
				*key = KEY_FIRE;
			}

			break;
		}

		default:
			*pressed = 0;
			return 0;
			break;
		}
		lastKey = keys;
		return 1;
	}


	return 0;
}

void DG_SetWindowTitle(const char *title)
{

}
