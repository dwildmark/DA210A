/*
 * task_regulate.c
 *
 * Created: 2016-02-19 15:03:14
 *  Author: Jonathan
 */ 
#include <asf.h>
#include "sync.h"
#include "pwm_func.h"
#include "task_regulate.h"
#include "utils.h"

/* This task reads an analog value, filters it 
 * and calculates PID value for motor control PWM 
 */
void task_regulate(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = 50;
	xLastWakeTime = xTaskGetTickCount();
	char str[30] = {0};
			
	while(1){		
		if (new_value)
		{
			pwm_set_value_A(cha_setpoint);
			pwm_set_value_B(chb_setpoint);
			new_value = 0;
		}
		read_counters();
		int speed = (int) calc_speed(cha_reading);
		itoa(speed, str, 10);
		printf(str);
		printf("\n");
		
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}

float calc_speed(int reading)
{
	static int xbuff[6] = {0};
	static float c = (15.4 * 3.1415)/(0.05 * 72);
	float temp_sum = 0;
	
	/* Move invalue buffer one sample forward */
	for(int k = 5; k > 0; k--){
		xbuff[k] = xbuff[k-1];
	}
	xbuff[0] = reading;
	
	/* filter the signal */
	for(int k = 0; k < 6; k++){
		temp_sum += xbuff[k];
	}
	
	float mean_value = temp_sum / 6.0;
	float speed = mean_value * c;
	return speed;
}