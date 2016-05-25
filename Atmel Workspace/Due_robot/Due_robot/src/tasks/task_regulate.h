/*
 * task_regulate.h
 *
 * Created: 2/19/2016 15:58:42
 *  Author: Jonathan
 */ 
#include "utils.h"

#ifndef TASK_REGULATE_H_
#define TASK_REGULATE_H_

#define OFFSET						1500
#define K_PROP						(1.8)
#define K_INT						(0.4)
#define BUFF_LENGTH					6
#define taskREG_PERIOD				100
#define INTEG_LIM					100000
#define MAX_SETPOINT_DEVIATION		(max_acceleration * ((float)taskREG_PERIOD/1000))
#define PULSE_TO_SPEED				(15.4 * 3.1415)/(((float) taskREG_PERIOD/1000) * 72)


void task_regulate(void *pvParameters);
float calc_speed_a(int new_value);
float calc_speed_b(int new_value);
void regulate_PID(float cha_setpoint, float chb_setpoint);
int limit_setpoint(int new_setpoint, int current_setpoint);

#endif /* TASK_REGULATE_H_ */