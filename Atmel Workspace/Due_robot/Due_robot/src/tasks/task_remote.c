/*
 * task_remote.c
 *
 * Created: 2016-05-04 10:28:12
 *  Author: DennisW
 */ 

#include <asf.h>
#include "sync.h"
#include "pwm_func.h"
#include "tasks\task_remote.h"

void task_remote(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = 50;
	xLastWakeTime = xTaskGetTickCount();
	uint8_t last_received = -1;
	uint8_t iters = 0;
	while(1)
	{
		if(ioport_get_pin_level(VT_PIN) == true)
		{
			uint8_t received = read_remote();
			if(received != last_received) {
				switch(received)
				{
					case BTN_ON:
						running = 1;
						break;
					case BTN_OFF:
						running = 0;
						addon_up = false;
						addon_down = false;
						break;
					case BTN_UP:
						if(addon_down) addon_down = false;
						else addon_up = true;
						break;
					case BTN_DOWN:
						if(addon_up) addon_up = false;
						else addon_down = true;
						break;
					default:
						break;
				}
				last_received = received;
			} else if(iters > 4) {
				 last_received = -1;
				 iters = 0;
			}
			iters++;
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}

uint8_t read_remote(void)
{
	uint8_t reading = ioport_get_pin_level(RX_PIN0) | (ioport_get_pin_level(RX_PIN1)<<1) | (ioport_get_pin_level(RX_PIN2)<<2) | (ioport_get_pin_level(RX_PIN3)<<3);
	return reading;
}