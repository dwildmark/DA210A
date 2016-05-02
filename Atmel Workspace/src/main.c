/*
 * Author: Jonathan
 */
#include <asf.h>

#include "config_functions/adc_func.h"
#include "config_functions/console_func.h"
#include "config_functions/pwm_func.h"
#include "tasks/task_com.h"
#include "tasks/task_regulate.h"
#include "sync.h"
#include "utilities.h"


/* "Global" variables shared between tasks declared here */
xSemaphoreHandle sync = 1;
uint16_t adc_to_mm[CONV_ARR_LENGTH] = {0};
uint16_t meas_distance = 0;
int err = 0;
uint16_t output = 0;
uint16_t timer;
float prop_gain;
float int_gain;
float der_gain;
int offset;
int antiwindup;
uint16_t set_point;

int main (void)
{
	/* Initialize board and all functions that will be used */
	sysclk_init();
	board_init();
	configure_console();
	ioport_init();		
	adc_config();
	pwm_config();
	delay_init(sysclk_get_cpu_hz());
	
	/* Receive variables that will be used for PID-controller */
	matlab_recieve_values();
	/* Start fan with provided offset for a few seconds to reach measurable span */
	regulate_init();
	
	/* Configure RTOS tasks and start tasks */
	vSemaphoreCreateBinary(sync);
	xTaskCreate(task_com, (const signed char * const) "Com", TASK_STACKSIZE, NULL, 1, NULL);	
	xTaskCreate(task_regulate, (const signed char * const) "Regulate", TASK_STACKSIZE,NULL,2,NULL);	
	vTaskStartScheduler();	
}
