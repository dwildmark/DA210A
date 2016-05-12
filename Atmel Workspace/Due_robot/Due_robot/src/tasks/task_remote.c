/*
 * task_remote.c
 *
 * Created: 2016-05-04 10:28:12
 *  Author: DennisW
 */ 

#include <asf.h>
#include "utils.h"
#include "sync.h"
#include "pwm_func.h"

void task_remote(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = 50;
	xLastWakeTime = xTaskGetTickCount();
	char str[20] = {0};
	while(1)
	{
		if(ioport_get_pin_level(VT_PIN) == true)
		{
			int recieved = read_remote();
			switch(recieved)
			{
			case 7:
				running = 1;
				break;
			case 11:
				running = 0;
				break;
			default:
				break;
			}
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}