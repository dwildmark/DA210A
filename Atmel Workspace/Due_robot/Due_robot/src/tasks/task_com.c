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
	char str[20] = {0};
	printf("Task initiated\n");
	char *cp;
	while(1)
	{	
		gets(str);
		cp = strdup(str);
		char *tok = cp, *end = cp;
		
		strsep(&end, ":");
		if(new_value==0) cha_setpoint = atoi(tok);
		printf(tok);
		printf("\n");
		tok = end;
		strsep(&end, ":");
		if(new_value==0) chb_setpoint = atoi(tok);
		printf(tok);
		printf("\n");
		new_value = 1;
		

		/* Sleep for some time */
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}