/*
 * This file holds functions for initializing and using the PWM device.
 *
 *  Author: Dennis Wildmark, Olle Casperson, Jonathan Böcker
 */ 
#include <asf.h>
#include "pwm_func.h"

static pwm_channel_t pwm_instance_A;
static pwm_channel_t pwm_instance_B;

/************************************************************************/
/* Function initializes the PWM with the defined settings               */
/************************************************************************/
int pwm_config(void)
{
	if(pmc_enable_periph_clk(ID_PWM) == 0) {
		/* Initialize channel A PWM */
		pwm_channel_disable(PWM, PWM_CHANNEL_0);
		pwm_clock_t pwmclock = {
			.ul_clka = PWM_FREQUENCY * PWM_RESOLUTION,
			.ul_clkb = 0,
			.ul_mck = sysclk_get_cpu_hz()
		};
		pwm_init(PWM, &pwmclock);
		pwm_instance_A.alignment = PWM_ALIGN_LEFT;
		pwm_instance_A.polarity = PWM_LOW;
		pwm_instance_A.ul_prescaler = PWM_CMR_CPRE_CLKA;
		pwm_instance_A.ul_period = PWM_RESOLUTION;
		pwm_instance_A.ul_duty = 0;
		pwm_instance_A.channel = PWM_CHANNEL_0;
		pwm_channel_init(PWM, &pwm_instance_A);
		pio_set_peripheral(PWM_PIO, PWM_PIO_PERIPHERAL, PWM_PIN_A);
		pwm_channel_enable(PWM, PWM_CHANNEL_0);
		
		/* Initialize channel B PWM */
		pwm_channel_disable(PWM, PWM_CHANNEL_1);
		pwm_instance_B.alignment = PWM_ALIGN_LEFT;
		pwm_instance_B.polarity = PWM_LOW;
		pwm_instance_B.ul_prescaler = PWM_CMR_CPRE_CLKA;
		pwm_instance_B.ul_period = PWM_RESOLUTION;
		pwm_instance_B.ul_duty = 0;
		pwm_instance_B.channel = PWM_CHANNEL_1;
		pwm_channel_init(PWM, &pwm_instance_B);
		pio_set_peripheral(PWM_PIO, PWM_PIO_PERIPHERAL, PWM_PIN_B);
		pwm_channel_enable(PWM, PWM_CHANNEL_1);
		return 0;
	}
	return 1;
}

/************************************************************************/
/* Function sets PWM dutycycle to the provided value.                   */
/* It also limits it to between 0 and PWM_RESOLUTION.                   */
/************************************************************************/
void pwm_set_value_A(uint32_t dutycycle)
{
 	dutycycle = min(max(dutycycle, PWM_MIN_VAL), PWM_MAX_VAL);
	pwm_channel_update_duty(PWM, &pwm_instance_A, dutycycle);
}

void pwm_set_value_B(uint32_t dutycycle)
{
	dutycycle = min(max(dutycycle, PWM_MIN_VAL), PWM_MAX_VAL);
	pwm_channel_update_duty(PWM, &pwm_instance_B, dutycycle);
}