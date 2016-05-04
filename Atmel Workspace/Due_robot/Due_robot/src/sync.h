/*
 * sync.h
 *
 * Created: 2016-02-19 14:46:06
 *  Author: Jonathan
 */ 
#include <asf.h>

#ifndef SYNC_H_
#define SYNC_H_

/* "Global" variables declared in main file is also declared here */
extern xSemaphoreHandle sync;
extern uint16_t cha_reading;
extern uint16_t chb_reading;
extern float cha_setpoint;
extern float chb_setpoint;
extern uint16_t new_value;

/* The conversion array length for calculating ball distance */
#define CONV_ARR_LENGTH 100
/* Length of string buffer when sending and receiving strings */
#define BUF_LEN 16
/* Stack size for tasks */
#define TASK_STACKSIZE	(2048/sizeof(portSTACK_TYPE))
/* How long the fan will blow before any task executes */
#define FAN_INIT_MS 5000

#endif /* SYNC_H_ */