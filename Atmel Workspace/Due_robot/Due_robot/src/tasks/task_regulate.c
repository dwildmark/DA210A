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
		//regulate_PID(cha_setpoint, chb_setpoint);
		read_counters();
		int speed = (int) calc_speed(cha_reading);
		itoa(speed, str, 10);
// 		printf(str);
// 		printf("\n");
		
		if(new_value)
		{
			pwm_set_value_A(cha_setpoint);
			pwm_set_value_B(chb_setpoint);
		}
		
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

void regulate_PID(float setpoint_A, float setpoint_B)
{
	/* Only for debugging */
	char str[30] = {0};
	
	static int sum_err_A = 0;	//Variable that holds the sum of errors
	static int sum_err_B = 0;
	static int old_err_A = 0; //Variable that holds the error from last function call
	static int old_err_B = 0;
	const float dT = (float) 50/1000; //Calculate the time step
	read_counters();
	float speed_A = calc_speed(cha_reading); //ADC-value converted to millimeters
	float speed_B = calc_speed(chb_reading);
	float new_err_A = setpoint_A - speed_A; //Current error
	float new_err_B = setpoint_B - speed_B;
	sum_err_A += new_err_A; //add the new error to the error sum
	sum_err_B += new_err_B;
	//sum_err = max(min(sum_err, INTEG_LIM), -INTEG_LIM); //Limit the error sum from INTEG_LIM to (-INTEG_LIM)
	//int d_err_ = old_err - new_err; //difference in error from last function call, used in the derivative part
	
	/* Calculates the proportional, integral and derivative parts of the controller value */
	float prop_A = (K_PROP) * (float)new_err_A;
	float integ_A = 0;//(K_PROP) * (float)((dT * sum_err_A) * K_INT);
	
	float prop_B = (K_PROP) * (float)new_err_B;
	float integ_B = (K_PROP) * (float)((dT * sum_err_B) * K_INT);
	//float deriv = (K_PROP) * (float)((d_err/dT) * K_DERIV);
	
	int pwm_outval_A = OFFSET + (int)(prop_A); //
	int pwm_outval_B = OFFSET + (int)(prop_B + integ_B);
	itoa(pwm_outval_A, str, 10);
	printf(str);
	printf("\n");
	itoa(speed_A, str, 10);
	printf(str);
	printf("\n");
	pwm_set_value_A(pwm_outval_A); //Write control value to pwm
	pwm_set_value_B(pwm_outval_B);
	old_err_A = new_err_A; //Set old error to new error
	old_err_B = new_err_B;
}