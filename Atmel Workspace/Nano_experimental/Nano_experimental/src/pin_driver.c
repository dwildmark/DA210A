/*
 * pin_driver.c
 *
 * Created: 2016-04-26 19:31:40
 *  Author: Dennis
 */ 

#include <asf.h>
#include "pin_driver.h"

void init_pins(void)
{
	ioport_set_pin_dir(PIN_13, IOPORT_DIR_OUTPUT);
}