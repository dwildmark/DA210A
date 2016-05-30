/*
 * test.c
 *	Functions for testing some of the functionality in the program.
 * 
 *  Author: Dennis Wildmark
 */ 
#include <asf.h>
#include "utils.h"

void test_adc_val(void)
{
	uint16_t adc_val[100] = {0};
	uint16_t min = 1024;
	uint16_t max = 0;
	int sum = 0;
	char str[20] = {0};
	for(int i = 0; i < 100; i++){
		adc_start(ADC);
		while((adc_get_status(ADC) & 0x1<<24)==0);
		adc_val[i] = adc_get_latest_value(ADC);
		if(adc_val[i] > max) {
			max = adc_val[i];
		}
		if( adc_val[i] < min) {
			min = adc_val[i];
		}
		sum += adc_val[i];
	}
	int avg = sum / 100;
	itoa(min, str, 10);
	printf("MIN: ");
	printf(str);
	printf("\n");
	printf("MAX: ");
	itoa(max, str, 10);
	printf(str);
	printf("\n");
	itoa(avg, str, 10);
	printf("AVG: ");
	printf(str);
	printf("\n");
	printf("-----------------------\n");
}
