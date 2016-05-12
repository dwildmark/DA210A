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
				//running = 1;
				pwm_set_value_A(1000);
				pwm_set_value_B(1000);
				break;
			case 11:
				//running = 0;
				pwm_set_value_A(1500);
				pwm_set_value_B(1500);
				break;
			default:
				break;
			}
			new_value = 1;
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}