/*
 * task_regulate.c
 *
 * Created: 2016-02-19 15:03:14
 *  Author: Jonathan
 */ 
#include <asf.h>
#include "sync.h"
#include "task_regulate.h"
#include "utils.h"

/* This task reads an analog value, filters it 
 * and calculates PID value for fan PWM 
 */
void task_regulate(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = 50;
	xLastWakeTime = xTaskGetTickCount();
	
	while(1){		
		if (new_value)
		{
			set_speed(CHA_PWM, cha_setpoint);
			set_speed(CHB_PWM, chb_setpoint);
			new_value = 0;
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}
