/*
 * utilities.h
 *
 * Created: 3/22/2016 13:37:14
 *  Author: jonathan
 */ 


#ifndef UTILITIES_H_
#define UTILITIES_H_

void regulate_init(void);
int signal_filter(int);
int calculate_distance(int);
int pid_controller(int);
void matlab_recieve_values(void);

#define BL 4

#endif /* UTILITIES_H_ */