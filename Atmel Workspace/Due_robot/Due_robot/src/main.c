/*
 * Author: Jonathan, Dennis Wildmark
 */
#include <asf.h>
#include "config_functions/console_func.h"
#include "tasks/task_com.h"
#include "tasks/task_regulate.h"
#include "sync.h"
#include "utils.h"
#include "pwm_func.h"
#include "tasks/task_remote.h"
#include "test.h"
#include "tasks/task_addon.h"


/* "Global" variables shared between tasks declared here */
uint16_t cha_reading = 0;
uint16_t chb_reading = 0;
float cha_setpoint = 0;
float chb_setpoint = 0;
uint16_t new_value = 0;
int running = 0;
int max_acceleration = 0;
addon_t addon;
bool addon_up = false;
bool addon_down = false;


int main (void)
{
	/* Initialize board and all functions that will be used */
	sysclk_init();
	board_init();
	configure_console();
	ioport_init();
	init_pins();
	pwm_config();
	delay_init(sysclk_get_cpu_hz());
	adc_config();
	addon = detect_addon();
	init_properties(addon);

	/* Configure RTOS tasks and start tasks */
	xTaskCreate(task_com, (const signed char * const) "Com", TASK_STACKSIZE, NULL, 2, NULL);
	xTaskCreate(task_regulate, (const signed char * const) "regulate", TASK_STACKSIZE, NULL, 4, NULL);
	xTaskCreate(task_remote, (const signed char * const) "remote", TASK_STACKSIZE, NULL, 3, NULL);
	xTaskCreate(task_addon, (const signed char * const) "addon", TASK_STACKSIZE, NULL, 3, NULL);
	vTaskStartScheduler();
}
