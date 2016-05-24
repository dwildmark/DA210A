/*
 * task_regulate.c
 *
 * Created: 2016-02-19 15:03:14
 *  Author: Dennis Wildmark
 */ 
#include <asf.h>
#include "sync.h"
#include "pwm_func.h"
#include "task_regulate.h"
#include "utils.h"

/* This task controls the wheels on the Arlo Robot. It uses one setpoint for each wheel, 
	and determines the wheel output value depending on the current speed. 
 */
void task_regulate(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = taskREG_PERIOD;
	xLastWakeTime = xTaskGetTickCount();
			
	while(1){
		ioport_set_pin_level(TEST_PIN, IOPORT_PIN_LEVEL_HIGH);
		if(running == 1) {
			
			regulate_PID(cha_setpoint, chb_setpoint);
			
		} else if(running == 0) {
			
			regulate_PID(0, 0);
			
		}
		ioport_set_pin_level(TEST_PIN, IOPORT_PIN_LEVEL_LOW);
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}

/************************************************************************/
/* These functions calculates a moving average of the speed readings.   */
/************************************************************************/
float calc_speed_a(int reading)
{
	static int xbuff[BUFF_LENGTH] = {0};
	float temp_sum = 0;
	
	/* Move value buffer one sample forward */
	for(int k = BUFF_LENGTH - 1; k > 0; k--){
		xbuff[k] = xbuff[k-1];
	}
	xbuff[0] = reading;
	
	/* Filter the signal */
	for(int k = 0; k < BUFF_LENGTH; k++){
		temp_sum += xbuff[k];
	}
	
	float mean_value = temp_sum / (float)BUFF_LENGTH;
	float speed = mean_value * PULSE_TO_SPEED;
	return speed;
}

float calc_speed_b(int reading)
{
	static int xbuff[BUFF_LENGTH] = {0};
	float temp_sum = 0;
	
	/* Move value buffer one sample forward */
	for(int k = BUFF_LENGTH - 1; k > 0; k--){
		xbuff[k] = xbuff[k-1];
	}
	xbuff[0] = reading;
	
	/* Filter the signal */
	for(int k = 0; k < BUFF_LENGTH; k++){
		temp_sum += xbuff[k];
	}
	
	float mean_value = temp_sum / (float)BUFF_LENGTH;
	float speed = mean_value * PULSE_TO_SPEED;
	return speed;
}

/************************************************************************/
/* Controller algorithm that takes care of wheel control using a        */
/* setpoint and a speed reading in a PI-algorithm.                      */
/************************************************************************/
void regulate_PID(float setpoint_A, float setpoint_B)
{
	
	static int sum_err_A = 0; //Variables that holds the sum of errors
	static int sum_err_B = 0; 
	
	static int old_outval_A = 0;
	static int old_outval_B = 0;
	
	static int current_setpoint_a = 0;
	static int current_setpoint_b = 0;
	
	current_setpoint_a += limit_setpoint(setpoint_A, current_setpoint_a);
	current_setpoint_b += limit_setpoint(setpoint_B, current_setpoint_b);

	const float dT = (float) taskREG_PERIOD/1000; //Calculate the time step
	
	read_counters();
	
	float speed_A = get_speed_vector(old_outval_A, calc_speed_a(cha_reading));
	float speed_B = get_speed_vector(old_outval_B, calc_speed_b(chb_reading));

	float new_err_A = current_setpoint_a - speed_A; //Current error
	float new_err_B = current_setpoint_b - speed_B;
	
	
	
	sum_err_A += new_err_A; //add the new error to the error sum
	sum_err_B += new_err_B;
	sum_err_A = max(min(sum_err_A, INTEG_LIM), -INTEG_LIM); //Limit the error sum from INTEG_LIM to (-INTEG_LIM)
	sum_err_B = max(min(sum_err_B, INTEG_LIM), -INTEG_LIM);
	
	/* Calculates the proportional, integral and derivative parts of the controller value */
	float prop_A = (K_PROP) * (float)new_err_A;
	float integ_A = (K_PROP) * (float)((dT * sum_err_A) / K_INT);

	float prop_B = (K_PROP) * (float)new_err_B;
	float integ_B = (K_PROP) * (float)((dT * sum_err_B) / K_INT);
	
	int pwm_outval_A = OFFSET + (int)(prop_A) + (int)(integ_A);
	int pwm_outval_B = OFFSET + (int)(prop_B) + (int)(integ_B);

	pwm_set_value_A(pwm_outval_A); //Write control value to pwm
	pwm_set_value_B(pwm_outval_B);

	old_outval_A = pwm_outval_A - OFFSET;
	old_outval_B = pwm_outval_B - OFFSET;
}

/************************************************************************/
/* Function to limit changes in setpoint.                               */
/* Used for limiting the acceleration                                   */
/************************************************************************/
int limit_setpoint(int new_setpoint, int current_setpoint) 
{
	int max_dev = (max_acceleration * ((float)taskREG_PERIOD/1000));
	if(new_setpoint >= current_setpoint){
		return min(max_dev, (new_setpoint - current_setpoint));
	} else {
		return -min(max_dev, (current_setpoint - new_setpoint));
	}
}