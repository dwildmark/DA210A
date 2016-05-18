/*
 * sync.h
 *
 * Created: 2016-02-19 14:46:06
 *  Author: Jonathan
 */ 
#include <asf.h>

#ifndef SYNC_H_
#define SYNC_H_

#define CHA_PIN0	PIO_PB26_IDX	//Pin 22
#define CHA_PIN1	PIO_PA14_IDX	//Pin 23
#define CHA_PIN2	PIO_PA15_IDX	//Pin 24
#define CHA_PIN3	PIO_PD0_IDX		//Pin 25
#define CHB_PIN0	PIO_PD1_IDX		//Pin 26
#define CHB_PIN1	PIO_PD2_IDX		//Pin 27
#define CHB_PIN2	PIO_PD3_IDX		//Pin 28
#define CHB_PIN3	PIO_PD6_IDX		//Pin 29
#define RS_PIN		PIO_PD10_IDX	//Pin 32
#define VT_PIN		PIO_PC15_IDX	//Pin 48
#define RX_PIN0		PIO_PC19_IDX	//Pin 44
#define	RX_PIN1		PIO_PC18_IDX	//Pin 45
#define RX_PIN2		PIO_PC17_IDX	//Pin 46
#define RX_PIN3		PIO_PC16_IDX	//Pin 47
#define TEST_PIN	PIO_PD9_IDX		//Pin 30 for TEST
/* "Global" variables declared in main file is also declared here */
extern xSemaphoreHandle sync;
extern uint16_t cha_reading;
extern uint16_t chb_reading;
extern float cha_setpoint;
extern float chb_setpoint;
extern uint16_t new_value;
extern int running;

/* The conversion array length for calculating ball distance */
#define CONV_ARR_LENGTH 100
/* Length of string buffer when sending and receiving strings */
#define BUF_LEN 16
/* Stack size for tasks */
#define TASK_STACKSIZE	(2048/sizeof(portSTACK_TYPE))
/* How long the fan will blow before any task executes */
#define FAN_INIT_MS 5000

#endif /* SYNC_H_ */