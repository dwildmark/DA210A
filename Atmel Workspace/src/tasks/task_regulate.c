/*
 * task_regulate.c
 *
 * Created: 2016-02-19 15:03:14
 *  Author: Jonathan
 */ 
#include <asf.h>
#include "config_functions/pwm_func.h"
#include "sync.h"
#include "task_regulate.h"
#include "utilities.h"

/* This task reads an analog value, filters it 
 * and calculates PID value for fan PWM 
 */
void task_regulate(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = timer;
	xLastWakeTime = xTaskGetTickCount();
	
	while(1){		
		/*Start ADC and read the value */
		adc_start(ADC);
		while ((adc_get_status(ADC) & 0x1<<24) == 0);		
		int invalue = adc_get_latest_value(ADC);		
		/* filter the signal */
		invalue = signal_filter(invalue);
		/* Calculate ball distance */
		int calc_distance = calculate_distance(invalue);		
		/* Control calculation */
		int calc_output = pid_controller(calc_distance);		
		/* Write output */
		pwm_set_duty_cycle(calc_output);
		/* Sleep for some time */
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}
