/*
 * pwm.c
 *
 * Created: 2016-02-19 14:12:43
 *  Author: Jonathan
 */ 
#include <asf.h>
#include "pwm_func.h"

static pwm_channel_t pwm_instance;

/* Enables PWM on DAC0 pin used for fan controlling */
void pwm_config(void)
{		
	if(pmc_enable_periph_clk(ID_PWM) == 0) {		
		pwm_clock_t pwmclock = {
			.ul_clka = PWM_FREQUENCY * PWM_RESOLUTION,
			.ul_clkb = 0,
			.ul_mck = sysclk_get_cpu_hz()
		};
		pwm_init(PWM, &pwmclock);
		pwm_instance.alignment = PWM_ALIGN_LEFT;
		pwm_instance.polarity = PWM_LOW;
		pwm_instance.ul_prescaler = PWM_CMR_CPRE_CLKA;
		pwm_instance.ul_period = PWM_RESOLUTION;
		pwm_instance.ul_duty = 0;
		pwm_instance.channel = PWM_CHANNEL_0;
		pwm_channel_init(PWM, &pwm_instance);
		pio_set_peripheral(PWM_PIO, PWM_PIO_PERIPHERAL, PWM_PIO_PIN);
		pwm_channel_enable(PWM, PWM_CHANNEL_0);
		pwm_set_duty_cycle(0);		
	}
}

/* Argument should be a value between 0-999 */
void pwm_set_duty_cycle(uint32_t dutycycle)
{
	pwm_channel_update_duty(PWM, &pwm_instance, dutycycle);
}