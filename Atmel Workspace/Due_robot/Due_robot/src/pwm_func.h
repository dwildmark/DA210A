/*
 * pwm.h
 *
 * 
 *  Author: Dennis Wildmark
 */ 


#ifndef PWM_H_
#define PWM_H_

#define PWM_RESOLUTION		19999
#define PWM_FREQUENCY		50
#define PWM_PIO				PIOB
#define PWM_PIO_PERIPHERAL	PIO_PERIPH_B
#define PWM_PIN_A			PIO_PB17B_PWML1
#define PWM_PIN_B			PIO_PB16B_PWML0
#define PWM_MIN_VAL			1000
#define PWM_MAX_VAL			2000

int pwm_config(void);
void pwm_set_value_A(uint32_t dutycycle);
void pwm_set_value_B(uint32_t dutycycle);

#endif /* PWM_H_ */