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
	const portTickType xTimeIncrement = taskREG_PERIOD;
	xLastWakeTime = xTaskGetTickCount();
	char str[30] = {0};
			
	while(1){
		if(running == 1)
		{
			regulate_PID(cha_setpoint, chb_setpoint);
//  		printf("Running");
// 			printf("\n");
		} 
		else if(running == 0)
		{
			motor_controller(1500, 1500);
// 			printf("Stopped");
// 			printf("\n");
		}
		
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}

float calc_speed_a(int reading)
{
	static int xbuff[BUFF_LENGTH] = {0};
	static float c = (15.4 * 3.1415)/(((float)taskREG_PERIOD/1000) * 72);
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
	float speed = mean_value * c;
	return speed;
}

float calc_speed_b(int reading)
{
	static int xbuff[BUFF_LENGTH] = {0};
	static float c = (15.4 * 3.1415)/(((float)taskREG_PERIOD/1000) * 72);
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
	float speed = mean_value * c;
	return speed;
}

void regulate_PID(float setpoint_A, float setpoint_B)
{
	/* Only for debugging */
	char str[30] = {0};
	
	static int sum_err_A = 0; //Variable that holds the sum of errors
	static int sum_err_B = 0;
	static int old_err_A = 0; //Variable that holds the error from last function call
	static int old_err_B = 0;
	static int old_outval_A = 0;
	static int old_outval_B = 0;
	static float old_setpoint_A = 0;
	static float old_setpoint_B = 0;
	const float dT = (float) taskREG_PERIOD/1000; //Calculate the time step
	read_counters();
	float speed_A = get_speed_vector(old_outval_A, calc_speed_a(cha_reading), old_setpoint_A);
	float speed_B = get_speed_vector(old_outval_B, calc_speed_b(chb_reading), old_setpoint_B);
	/* DEBUGGING */
// 	itoa((int)speed_A, str, 10);
// 	printf(str);
// 	printf("\n");
// 	itoa((int)speed_B, str, 10);
// 	printf(str);
// 	printf("\n");
	/* END OF DEBUGGING */
	float new_err_A = setpoint_A - speed_A; //Current error
	float new_err_B = setpoint_B - speed_B;
	
	
	
	sum_err_A += new_err_A; //add the new error to the error sum
	sum_err_B += new_err_B;
	sum_err_A = max(min(sum_err_A, INTEG_LIM), -INTEG_LIM); //Limit the error sum from INTEG_LIM to (-INTEG_LIM)
	sum_err_B = max(min(sum_err_B, INTEG_LIM), -INTEG_LIM);
	//int d_err_ = old_err - new_err; //difference in error from last function call, used in the derivative part
	
	/* Calculates the proportional, integral and derivative parts of the controller value */
	float prop_A = (K_PROP) * (float)new_err_A;
	float integ_A = (K_PROP) * (float)((dT * sum_err_A) / K_INT);
	
	float prop_B = (K_PROP) * (float)new_err_B;
	float integ_B = (K_PROP) * (float)((dT * sum_err_B) / K_INT);
	//float deriv = (K_PROP) * (float)((d_err/dT) * K_DERIV);
	
	int pwm_outval_A = OFFSET + (int)(prop_A) + (int)(integ_A);
	int pwm_outval_B = OFFSET + (int)(prop_B) + (int)(integ_B);

	pwm_set_value_A(pwm_outval_A); //Write control value to pwm
	pwm_set_value_B(pwm_outval_B);
	old_err_A = new_err_A; //Set old error to new error
	old_err_B = new_err_B;
	old_outval_A = pwm_outval_A - OFFSET;
	old_outval_B = pwm_outval_B - OFFSET;
	old_setpoint_A = setpoint_A;
	old_setpoint_B = setpoint_B;
}

void motor_controller(uint16_t target_value_A, uint16_t target_value_B)
{
	static int buff_A[BUFF_LENGTH] = {0};
	static int buff_B[BUFF_LENGTH] = {0};
	int temp_sum_A = 0;
	int temp_sum_B = 0;
	/* Move value buffer one sample forward */
	for(int k = BUFF_LENGTH - 1; k > 0; k--){
		buff_A[k] = buff_A[k-1];
		buff_B[k] = buff_B[k-1];
	}
	buff_A[0] = target_value_A - MOTOR_ZERO;
	buff_B[0] = target_value_B - MOTOR_ZERO;
	
	/* Filter the signal */
	for(int k = 0; k < BUFF_LENGTH; k++){
		temp_sum_A += buff_A[k];
		temp_sum_B += buff_B[k];
	}
	
	int mean_value_A = (temp_sum_A / BUFF_LENGTH) + MOTOR_ZERO;
	int mean_value_B = (temp_sum_B / BUFF_LENGTH) + MOTOR_ZERO;
	pwm_set_value_A(mean_value_A);
	pwm_set_value_B(mean_value_B);
}

void beta_controller(float setpoint_A, float setpoint_B)
{
	char str[30] = {0};

	static int old_err_A = 0; //Variable that holds the error from last function call
	static int old_err_B = 0;
	static int old_outval_A = 0;
	static int old_outval_B = 0;
	static float old_setpoint_A = 0;
	static float old_setpoint_B = 0;
	uint16_t new_error_A;
	uint16_t new_error_B;
	static uint16_t pwm_outval_A = 0;
	static uint16_t pwm_outval_B = 0;
	if (setpoint_A != old_setpoint_A)
	{
		pwm_outval_A = (6.25 * setpoint_A) + OFFSET;
	}
	if (setpoint_B != old_setpoint_B)
	{
		pwm_outval_B = (6.25 * setpoint_B) + OFFSET;
	}
	read_counters();
	float speed_A = get_speed_vector(old_outval_A, calc_speed_a(cha_reading), old_setpoint_A);
	float speed_B = get_speed_vector(old_outval_B, calc_speed_b(chb_reading), old_setpoint_B);
	new_error_A = setpoint_A - speed_A;
	new_error_B = setpoint_B - speed_B;
	
	/* DEBUGGING */
	itoa((int)speed_A, str, 10);
	printf("A:");
	printf(str);
	printf("\n");
	itoa((int)speed_B, str, 10);
	printf("B:");
	printf(str);
	printf("\n");
	itoa((int)setpoint_A, str, 10);
	printf("Setpoint A:");
	printf(str);
	printf("\n");
	itoa((int)setpoint_B, str, 10);
	printf("Setpoint B:");
	printf(str);
	printf("\n");
	/* END OF DEBUGGING */
	
	pwm_outval_A += new_error_A;
	pwm_outval_B += new_error_B;
	
	pwm_set_value_A(pwm_outval_A); 
	pwm_set_value_B(pwm_outval_B);
	
	old_err_A = new_error_A; //Set old error to new error
	old_err_B = new_error_B;
	old_outval_A = pwm_outval_A - OFFSET;
	old_outval_B = pwm_outval_B - OFFSET;
	old_setpoint_A = setpoint_A;
	old_setpoint_B = setpoint_B;
	
}