/*
 * task_regulate.h
 *
 * Created: 2/19/2016 15:58:42
 *  Author: Jonathan
 */ 


#ifndef TASK_REGULATE_H_
#define TASK_REGULATE_H_

#define OFFSET			1500
#define K_PROP			(2.2)
#define K_INT			(0.4)
#define BUFF_LENGTH		6
#define taskREG_PERIOD	100
#define INTEG_LIM		100000

void task_regulate(void *pvParameters);
float calc_speed_a(int new_value);
float calc_speed_b(int new_value);
void regulate_PID(float cha_setpoint, float chb_setpoint);
void motor_controller(uint16_t target_value_A, uint16_t target_value_B);
void beta_controller(float setpoint_A, float setpoint_B);

#endif /* TASK_REGULATE_H_ */