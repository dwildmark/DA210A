/*
 * task_com.c
 *
 * Created: 2016-02-19 14:35:52
 *  Author: Jonathan
 */ 
#include <asf.h>
#include "sync.h"

/* This task sends controller values to PC for logging*/
void task_com(void *pvParameters)
{	
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = timer;	
	xLastWakeTime = xTaskGetTickCount();
	uint8_t str[BUF_LEN] = {0};
	
	while(1)
	{	
		if(xSemaphoreTake(sync, portMAX_DELAY)){
			itoa(meas_distance, str, 10);
			printf(str);
			printf("\n");
			
			itoa(err, str, 10);
			printf(str);
			printf("\n");
			
			itoa(output, str, 10);
			printf(str);
			printf("\n");
			xSemaphoreGive(sync);
		}		
		/* Sleep for some time */
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);		
	}
}