/*
 * task_regulate.h
 *
 * Created: 2/19/2016 15:58:42
 *  Author: Jonathan
 */ 


#ifndef TASK_REGULATE_H_
#define TASK_REGULATE_H_

#define OFFSET		1500
#define K_PROP		0.2
#define K_INT		0.4

void task_regulate(void *pvParameters);
float calc_speed(int new_value);

#endif /* TASK_REGULATE_H_ */