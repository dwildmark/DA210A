/*
 * utils.h
 *
 * Created: 2016-05-02 14:10:22
 *  Author: DennisW
 */ 
#include <asf.h>

#ifndef UTILS_H_
#define UTILS_H_

#define CHA_PIN0	PIO_PB26_IDX	//Pin 22
#define CHA_PIN1	PIO_PA14_IDX	//Pin 23
#define CHA_PIN2	PIO_PA15_IDX	//Pin 24
#define CHA_PIN3	PIO_PD0_IDX		//Pin 25
#define CHB_PIN0	PIO_PD1_IDX		//Pin 26
#define CHB_PIN1	PIO_PD2_IDX		//Pin 27
#define CHB_PIN2	PIO_PD3_IDX		//Pin 28
#define CHB_PIN3	PIO_PD6_IDX		//Pin 29
#define RS_PIN		PIO_PD10_IDX	//Pin 32
#define CHA_PWM		PIO_PC2_IDX		//Pin 34
#define CHB_PWM		PIO_PC3_IDX		//Pin 35

void read_counters(void);
void set_speed(ioport_pin_t pin, uint16_t speed);
void init_pins(void);


#endif /* UTILS_H_ */