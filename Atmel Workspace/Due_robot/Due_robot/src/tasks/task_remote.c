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
	while(1)
	{
		if(ioport_get_pin_level(VT_PIN) == true)
		{
			uint8_t recieved = read_remote();
			switch(recieved)
			{
			case 7:
				running = 1;
				break;
			case 11:
				running = 0;
				break;
			case 13:
				//TODO: Control up-motion of addon
				addon_up = true;
				addon_down = false;
				break;
			case 14:
				//TODO: Control down-motion of addon
				addon_down = true;
				addon_up = false;
				break;
			default:
				break;
			}
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}

uint8_t read_remote(void)
{
	uint8_t reading = ioport_get_pin_level(RX_PIN0) | (ioport_get_pin_level(RX_PIN1)<<1) | (ioport_get_pin_level(RX_PIN2)<<2) | (ioport_get_pin_level(RX_PIN3)<<3);
	return reading;
}