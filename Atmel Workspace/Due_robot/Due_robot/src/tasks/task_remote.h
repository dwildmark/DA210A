/*
 * task_remote.h
 *
 * Created: 2016-05-04 10:28:45
 *  Author: Dennis Wildmark
 */ 


#ifndef TASK_REMOTE_H_
#define TASK_REMOTE_H_

#define BTN_ON		7
#define BTN_OFF		11
#define BTN_UP		13
#define BTN_DOWN	14
void task_remote(void *pvParameters);
uint8_t read_remote(void);



#endif /* TASK_REMOTE_H_ */