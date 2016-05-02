/*
 * utils.c
 *
 * Created: 2016-05-02 14:09:57
 *  Author: DennisW
 */ 

#include "utils.h"
#include "sync.h"
#include <asf.h>

void read_counters(void)
{
	cha_reading = ioport_get_pin_level(CHA_PIN0) | (ioport_get_pin_level(CHA_PIN1)<<1) | (ioport_get_pin_level(CHA_PIN2)<<2) | (ioport_get_pin_level(CHA_PIN3)<<3);
	chb_reading = ioport_get_pin_level(CHA_PIN0) | (ioport_get_pin_level(CHA_PIN1)<<1) | (ioport_get_pin_level(CHA_PIN2)<<2) | (ioport_get_pin_level(CHA_PIN3)<<3);
	ioport_set_pin_level(RS_PIN, true);
	delay_ms(2);
	ioport_set_pin_level(RS_PIN, false);
}

void set_speed(ioport_pin_t pin, uint16_t speed)
{
	ioport_set_pin_level(pin, true);
	vTaskDelay((float)speed/1000);
	ioport_set_pin_level(pin, false);
}

void init_pins(void)
{
	ioport_set_pin_dir(CHA_PIN0, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHA_PIN1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHA_PIN2, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHA_PIN3, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN0, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN2, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN3, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RS_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CHA_PWM, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CHB_PWM, IOPORT_DIR_OUTPUT);
}

