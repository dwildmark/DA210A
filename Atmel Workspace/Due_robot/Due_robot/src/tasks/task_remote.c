/*
 * task_remote.c
 *
 * Created: 2016-05-04 10:28:12
 *  Author: DennisW
 */ 

#include <asf.h>
#include "utils.h"
#include "sync.h"

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
			itoa(recieved, str, 10);
			printf(str);
			printf("\n");
			switch(recieved)
			{
			case 7:
				cha_setpoint = 50;
				chb_setpoint = 50;
				break;
			case 11:
				cha_setpoint = 0;
				chb_setpoint = 0;
				break;
			default:
				break;
			}
			new_value = 1;
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}