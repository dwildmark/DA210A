/*
 * task_remote.c
 *
 * Created: 2016-05-04 10:28:12
 *  Author: DennisW
 */ 

#include <asf.h>
#include "utils.h"

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
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}