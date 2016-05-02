/*
 * pwm.h
 *
 * Created: 2016-02-19 14:14:16
 *  Author: Jonathan
 */ 


#ifndef PWM_H_
#define PWM_H_

#define PWM_RESOLUTION		1000
#define PWM_FREQUENCY		500
#define PWM_PIO				PIOB
#define PWM_PIO_PERIPHERAL	PIO_PERIPH_B
#define PWM_PIO_PIN			PIO_PB16B_PWML0

void pwm_config(void);
void pwm_set_duty_cycle(uint32_t dutycycle);

#endif /* PWM_H_ */