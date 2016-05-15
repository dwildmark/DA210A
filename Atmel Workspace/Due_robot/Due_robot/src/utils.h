/*
 * utils.h
 *
 * Created: 2016-05-02 14:10:22
 *  Author: DennisW
 */ 
#include <asf.h>

#ifndef UTILS_H_
#define UTILS_H_

void read_counters(void);
void set_speed(ioport_pin_t pin, uint16_t speed);
void init_pins(void);
float get_speed_vector(int oldval, float speed);


#endif /* UTILS_H_ */