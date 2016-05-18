/*
 * utils.h
 *
 * Created: 2016-05-02 14:10:22
 *  Author: DennisW
 */ 
#include <asf.h>

#ifndef UTILS_H_
#define UTILS_H_

typedef enum {
	T800,
	OPTIMUS_PRIME,
	ROBOCOP,
	NOT_DETETCTED
	} addon_t;

void read_counters(void);
void set_speed(ioport_pin_t pin, uint16_t speed);
void init_pins(void);
float get_speed_vector(int oldval, float speed);
void adc_config(void);
addon_t detect_addon(void);
void init_properties(void);


#endif /* UTILS_H_ */