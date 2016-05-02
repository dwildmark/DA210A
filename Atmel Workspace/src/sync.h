/*
 * sync.h
 *
 * Created: 2016-02-19 14:46:06
 *  Author: Jonathan
 */ 


#ifndef SYNC_H_
#define SYNC_H_

/* "Global" variables declared in main file is also declared here */
extern xSemaphoreHandle sync;
extern uint16_t adc_to_mm[];
extern uint16_t meas_distance;
extern int err;
extern uint16_t output;
extern uint16_t timer;
extern float prop_gain;
extern float int_gain;
extern float der_gain;
extern int offset;
extern int antiwindup;
extern uint16_t set_point;

/* The conversion array length for calculating ball distance */
#define CONV_ARR_LENGTH 100
/* Length of string buffer when sending and receiving strings */
#define BUF_LEN 16
/* Stack size for tasks */
#define TASK_STACKSIZE	(2048/sizeof(portSTACK_TYPE))
/* How long the fan will blow before any task executes */
#define FAN_INIT_MS 5000

#endif /* SYNC_H_ */