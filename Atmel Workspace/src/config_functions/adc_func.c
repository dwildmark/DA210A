/*
 * adc_func.c
 *
 * Created: 2016-02-19 14:06:32
 *  Author: Jonathan
 */ 

#include <asf.h>
#include "adc_func.h"

/* Enables analog to digital conversion */
void adc_config(void)
{
	pmc_enable_periph_clk(ID_ADC);
	adc_init(ADC, sysclk_get_main_hz(), 20000000, 0);
	adc_configure_timing(ADC, 0, 0, 0);
	adc_set_resolution(ADC, ADC_MR_LOWRES);
	adc_enable_channel(ADC, ADC_CHANNEL_10);
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0);
}