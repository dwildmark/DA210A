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
	static int old_reading_a = 0;
	static int old_reading_b = 0;
	
	int new_reading_a = ioport_get_pin_level(CHA_PIN0) | (ioport_get_pin_level(CHA_PIN1)<<1) | (ioport_get_pin_level(CHA_PIN2)<<2) | (ioport_get_pin_level(CHA_PIN3)<<3);
	int new_reading_b = ioport_get_pin_level(CHB_PIN0) | (ioport_get_pin_level(CHB_PIN1)<<1) | (ioport_get_pin_level(CHB_PIN2)<<2) | (ioport_get_pin_level(CHB_PIN3)<<3);
	
	if(old_reading_a > new_reading_a){
		cha_reading = 15 - old_reading_a + new_reading_a;
	} else {
		cha_reading = new_reading_a - old_reading_a;
	}
	
	if(old_reading_b > new_reading_b){
		chb_reading = 15 - old_reading_b + new_reading_b;
	} else {
		chb_reading = new_reading_b - old_reading_b;
	}
	
	old_reading_a = new_reading_a;
	old_reading_b = new_reading_b;
}

void init_pins(void)
{
	/* Input pins for reading the counters */
	ioport_set_pin_dir(CHA_PIN0, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHA_PIN1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHA_PIN2, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHA_PIN3, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN0, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN2, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CHB_PIN3, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RS_PIN, IOPORT_DIR_OUTPUT);
	/* Input pins for reading the remote controller */
	ioport_set_pin_dir(RX_PIN0, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RX_PIN1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RX_PIN2, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RX_PIN3, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(VT_PIN, IOPORT_DIR_INPUT);
}

uint8_t read_remote(void)
{
	uint8_t reading = ioport_get_pin_level(RX_PIN0) | (ioport_get_pin_level(RX_PIN1)<<1) | (ioport_get_pin_level(RX_PIN2)<<2) | (ioport_get_pin_level(RX_PIN3)<<3);
	return reading;
}