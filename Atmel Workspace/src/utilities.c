/*
 * utilities.c
 *
 * Created: 3/22/2016 13:36:42
 *  Author: jonathan
 */ 
#include <asf.h>
#include "sync.h"
#include "utilities.h"
#include "config_functions/pwm_func.h"

/* Reads global values from serial.
 * Received values are used in task_regulate
 */
void matlab_recieve_values(void)
{
	/* Buffer for receiving a string */
	uint8_t buffer[BUF_LEN] = {0};
	
	/* Receive the conversion array */
	for (int i = 0; i < CONV_ARR_LENGTH; i++)
	{
		gets(buffer);
		adc_to_mm[i] = atoi(buffer);
	}
	gets(buffer);
	prop_gain = (float)atoi(buffer) / 1000;
	gets(buffer);
	int_gain = (float)atoi(buffer) / 1000;
	gets(buffer);
	der_gain = (float)atoi(buffer) / 1000;
	gets(buffer);
	set_point = atoi(buffer);
	gets(buffer);
	offset = atoi(buffer);
	gets(buffer);
	antiwindup = atoi(buffer);
	gets(buffer);
	timer = atoi(buffer);
}

/* Sets a value (offset) to PWM duty cycle and waits a 
 * couple of seconds so that the ball will be in measurable range 
 * by the time the controller task starts
 */
void regulate_init(void)
{
	pwm_set_duty_cycle(offset);
	delay_ms(FAN_INIT_MS);
}

/* Filters the signal with a FIR filter 
 * designed with Matlabs fdatool 
 */
int signal_filter(int invalue)
{
	const float B[4] = {
		-0.096412298233823315, 
		0.58755662129250108, 
		0.58755662129250108, 
		-0.096412298233823315
	};
	static int xbuff[BL] = {0};
	float temp_sum = 0;
		
	/* Move invalue buffer one sample forward */	
	for(int k = BL - 1; k > 0; k--){
		xbuff[k] = xbuff[k-1];
	}
	xbuff[0] = invalue;
	
	/* filter the signal */	
	for(int k = 0; k < BL; k++){
		temp_sum += xbuff[k] * B[k];
	}	
	return (int) temp_sum;
}

/* Converts the AD signal to a 
 * distance in mm from the sensor 
 */
int calculate_distance(int invalue)
{
	/* The index of the array is the AD-value divided by 10 */
	/* The index is limited to array boundaries */
	int adc_to_mm_index = min(max((invalue/10) - 1, 0), CONV_ARR_LENGTH - 2);
	/* An interpolation is calculated to achieve an as correct value as possible */
	int diff = adc_to_mm[adc_to_mm_index] - adc_to_mm[adc_to_mm_index + 1];
	int interpol = (diff * (invalue % 10)) / 10;
	
	/*return (adc_to_mm[adc_to_mm_index] + interpol);*/
	return adc_to_mm[adc_to_mm_index] - interpol;
}

/* Calculates a PWM value with a PID algorithm
 * Then copies process variables to global variables
 * with a thread safe semaphore handling
 */
int pid_controller(int calc_distance)
{
	/* Recursive error sum */
	static int int_sum = 0;	
	/* Error since last function call */
	static int old_error = 0;
	/* Time between executions in seconds */
	const float dT = (float) timer/1000;
	/* Error is difference between set point and measured distance */
	int new_error = set_point - calc_distance;
	/* Error is added to error sum */
	int_sum += new_error;
	
	/* Limit integral sum */
	int_sum = max(min(int_sum, antiwindup), -antiwindup);	
	int delta_error = old_error - new_error;
	/* PID algorithm is based on Ziegler-Nichols implementation */
	float p_part = (float) (new_error * (-prop_gain));
	float i_part = (float) ((dT * int_sum)/int_gain) * (-prop_gain);
	float d_part = (float) ((delta_error/dT) * der_gain) * (-prop_gain);
	float sum = p_part + i_part + d_part;
	/* An offset is added to the PID sum */
	int calc_output = (int) (offset + sum);
	/* Limit output to 0-100% */
	calc_output = max(min(calc_output, PWM_RESOLUTION), 0);
	old_error = new_error;	
	/* Transfer values between tasks */
	if(xSemaphoreTake(sync, portMAX_DELAY)){
		meas_distance = calc_distance;
		err = new_error;
		output = calc_output;
		xSemaphoreGive(sync);
	}			
	return calc_output;
}