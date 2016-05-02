/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#define PIN_13	IOPORT_CREATE_PIN(PORTB, 5)//TE-pin
#define PIN_12	IOPORT_CREATE_PIN(PORTB, 4)//Data pins
#define PIN_11	IOPORT_CREATE_PIN(PORTB, 3)
#define PIN_10	IOPORT_CREATE_PIN(PORTB, 2)
#define	PIN_9	IOPORT_CREATE_PIN(PORTB, 1)//---------
#define PIN_8	IOPORT_CREATE_PIN(PORTB, 0)//Knapp 1
#define PIN_7	IOPORT_CREATE_PIN(PORTD, 7)//Knapp 2
#define PIN_6	IOPORT_CREATE_PIN(PORTD, 6)//Knapp 3
#define PIN_5	IOPORT_CREATE_PIN(PORTD, 5)//Knapp 4
int main( void )
{
	sysclk_init();
	board_init();
	ioport_init();
	delay_init(16000000UL);
	
	ioport_set_pin_dir(PIN_13, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_12, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_11, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_10, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_9, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_8, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_7, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_6, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_5, IOPORT_DIR_INPUT);
	
	ioport_set_pin_level(PIN_13, true);
	ioport_set_pin_level(PIN_12, true);
	ioport_set_pin_level(PIN_11, true);
	ioport_set_pin_level(PIN_10, true);
	ioport_set_pin_level(PIN_9, true);
	
	bool btn_1;
	bool btn_2;
	bool btn_3;
	bool btn_4;
	
	while(1)
	{
		btn_1 = ioport_get_pin_level(PIN_8);
		btn_2 = ioport_get_pin_level(PIN_7);
		btn_3 = ioport_get_pin_level(PIN_6);
		btn_4 = ioport_get_pin_level(PIN_5);
		if(btn_1 || btn_2 || btn_3 || btn_4)
		{
			ioport_set_pin_level(PIN_12, !btn_1);
			ioport_set_pin_level(PIN_11, !btn_2);
			ioport_set_pin_level(PIN_10, !btn_3);
			ioport_set_pin_level(PIN_9, !btn_4);
			
			delay_ms(2);
			
			ioport_set_pin_level(PIN_13, false);
			
			delay_ms(10);
			
			ioport_set_pin_level(PIN_13, true);
			
			delay_ms(300);
			
			ioport_set_pin_level(PIN_12, true);
			ioport_set_pin_level(PIN_11, true);
			ioport_set_pin_level(PIN_10, true);
			ioport_set_pin_level(PIN_9, true);
		}

	}
}
