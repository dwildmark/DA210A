/*
 * task_com.c
 *
 * Created: 2016-02-19 14:35:52
 *  Author: Jonathan
 */ 
#include <asf.h>
#include "sync.h"
#include <string.h>

/* This task sends controller values to PC for logging*/
void task_com(void *pvParameters)
{	
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = 50;	
	xLastWakeTime = xTaskGetTickCount();
	char *cp;
	while(1)
	{	
		char str[20] = {0};
		gets(str);
		cp = strdup(str);
		char *tok = cp, *end = cp;
		strsep(&end, ":");
		cha_setpoint = atoi(tok);
		tok = end;
		strsep(&end, ":");
		chb_setpoint = atoi(tok);
// 		if(running == 1) {
// 			printf("Running");
// 		} else if(running == 0){
// 			printf("Stopped");
// 		}
		/* Sleep for some time */
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}