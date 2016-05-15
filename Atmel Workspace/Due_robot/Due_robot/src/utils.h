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
#define VT_PIN		PIO_PC15_IDX	//Pin 48
#define RX_PIN0		PIO_PC19_IDX	//Pin 44
#define	RX_PIN1		PIO_PC18_IDX	//Pin 45
#define RX_PIN2		PIO_PC17_IDX	//Pin 46
#define RX_PIN3		PIO_PC16_IDX	//Pin 47

void read_counters(void);
void set_speed(ioport_pin_t pin, uint16_t speed);
void init_pins(void);
uint8_t read_remote(void);
float get_speed_vector(int oldval, float speed);


#endif /* UTILS_H_ */