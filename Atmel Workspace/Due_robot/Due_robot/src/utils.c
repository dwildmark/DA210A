/*
 * utils.c
 *
 * Created: 2016-05-02 14:09:57
 *  Author: DennisW
 */ 

#include <asf.h>
#include "utils.h"
#include "sync.h"


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

float get_speed_vector(int oldval, float speed)
{
	if(oldval >= 0)
	{
		return speed;
	} else {
		return -speed;
	}
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
	
	/* TEST-pin for measuring execution time */
	ioport_set_pin_dir(TEST_PIN, IOPORT_DIR_OUTPUT);
}

void adc_config(void)
{
	pmc_enable_periph_clk(ID_ADC);
	adc_init(ADC, sysclk_get_main_hz(), 20000000, 0);
	adc_configure_timing(ADC, 0, 0, 0);
	adc_set_resolution(ADC, ADC_MR_LOWRES);
	adc_enable_channel(ADC, ADC_CHANNEL_12); //Analog Pin 10
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0);
}

addon_t detect_addon(void)
{
	adc_start(ADC);
	while((adc_get_status(ADC) & 0x1<<24) == 0);
	uint16_t adc_val = adc_get_latest_value(ADC);
	/* For some reason, the first value is incorrect. Yet to find a better solution. */
	adc_start(ADC);
	while((adc_get_status(ADC) & 0x1<<24) == 0);
	adc_val = adc_get_latest_value(ADC);
	if(adc_val >= 250 && adc_val < 550) {
		return T800;
	} else if(adc_val >= 550 && adc_val < 850) {
		return OPTIMUS_PRIME;
	} else if(adc_val >= 850 && adc_val < 1024){
		return ROBOCOP;
	} else {
		return NOT_DETETCTED;
	}
}

void init_properties(addon_t addon)
{
	switch(addon) {
		case T800:
			//TODO: Set properties for T800-addon.
			max_acceleration = 10;
			printf("module:T800\n");
			break;
		case OPTIMUS_PRIME:
			//TODO: Set properties for Optimus Prime-addon.
			max_acceleration = 300;
			printf("module:OPTIMUS\n");
			break;
		case ROBOCOP:
			//TODO: Set properties for Robocop-addon.
			max_acceleration = 10000;
			printf("module:ROBOCOP\n");
			break;
		case NOT_DETETCTED:
			//TODO: Set default properties.
			max_acceleration = 0;
			printf("module:NOT_DETECTED\n");
			break;
	}
}
