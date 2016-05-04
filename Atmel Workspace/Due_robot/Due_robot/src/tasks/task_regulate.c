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
	
	/* Move value buffer one sample forward */
	for(int k = 5; k > 0; k--){
		xbuff[k] = xbuff[k-1];
	}
	xbuff[0] = reading;
	
	/* Filter the signal */
	for(int k = 0; k < 6; k++){
		temp_sum += xbuff[k];
	}
	
	float mean_value = temp_sum / 6.0;
	float speed = mean_value * c;
	return speed;
}

void regulate_PID_A(uint16_t setpoint)
{
	static int sum_err = 0;	//Variable that holds the sum of errors
	static int old_err = 0; //Variable that holds the error from last function call
	const float dT = (float) 50/1000; //Calculate the time step
	int speed = calc_speed(cha_reading); //ADC-value converted to millimeters
	int new_err = setpoint - speed; //Current error
	sum_err += new_err; //add the new error to the error sum
	sum_err = max(min(sum_err, INTEG_LIM), -INTEG_LIM); //Limit the error sum from INTEG_LIM to (-INTEG_LIM)
	int d_err = old_err - new_err; //difference in error from last function call, used in the derivative part
	
	/* Calculates the proportional, integral and derivative parts of the controller value */
	float prop = (k_prop) * (float)new_err;
	float integ = (k_prop) * (float)((dT * sum_err)/k_int);
	float deriv = (k_prop) * (float)((d_err/dT) * k_deriv);
	
	int pwm_outval = OFFSET + (int)(prop + integ + deriv); //
	pwm_set_value_A(pwm_outval); //Write control value to pwm
	update_vars(new_err, pwm_outval, real_dist); //Update variables for plot
	old_err = new_err; //Set old error to new error
}

void regulate_PID_B(uint16_t adcvalue)
{
	static int sum_err = 0;	//Variable that holds the sum of errors
	static int old_err = 0; //Variable that holds the error from last function call
	const float dT = (float) periodicity/1000; //Calculate the time step
	int real_dist = calc_dist(adcvalue); //ADC-value converted to millimeters
	int new_err = real_dist - setpoint; //Current error
	sum_err += new_err; //add the new error to the error sum
	sum_err = max(min(sum_err, INTEG_LIM), -INTEG_LIM); //Limit the error sum from INTEG_LIM to (-INTEG_LIM)
	int d_err = old_err - new_err; //difference in error from last function call, used in the derivative part
	
	/* Calculates the proportional, integral and derivative parts of the controller value */
	float prop = (k_prop) * (float)new_err;
	float integ = (k_prop) * (float)((dT * sum_err)/k_int);
	float deriv = (k_prop) * (float)((d_err/dT) * k_deriv);
	
	int pwm_outval = OFFSET + (int)(prop + integ + deriv); //
	pwm_set_value(pwm_outval); //Write control value to pwm
	update_vars(new_err, pwm_outval, real_dist); //Update variables for plot
	old_err = new_err; //Set old error to new error
}