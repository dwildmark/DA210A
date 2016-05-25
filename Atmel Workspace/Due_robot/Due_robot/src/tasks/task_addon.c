/*
 * task_addon.c
 *
 * Created: 2016-05-24 13:30:11
 *  Author: DennisW
 */ 

#include <asf.h>
#include "sync.h"
#include "task_addon.h"

void task_addon(void *pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xTimeIncrement = taskADDON_PERIOD;
	xLastWakeTime = xTaskGetTickCount();
	bool switch_top;
	bool switch_btm;
	while(1){
		switch_top = ioport_get_pin_level(ADDON_SWITCH_TOP);
		switch_btm = ioport_get_pin_level(ADDON_SWITCH_BTM);
		if(addon_up && (!switch_top)) {
			ioport_set_pin_level(ADDON_DIR_PIN_CW, ADDON_DIR_HIGH);
			ioport_set_pin_level(ADDON_DIR_PIN_CCW, ADDON_DIR_LOW);
			ioport_set_pin_level(ADDON_DRIVE_PIN, true);
		} else if(addon_down && (!switch_btm)) {
			ioport_set_pin_level(ADDON_DIR_PIN_CW, ADDON_DIR_LOW);
			ioport_set_pin_level(ADDON_DIR_PIN_CCW, ADDON_DIR_HIGH);
			ioport_set_pin_level(ADDON_DRIVE_PIN, true);
		} else {
			ioport_set_pin_level(ADDON_DRIVE_PIN, false);
			ioport_set_pin_level(ADDON_DIR_PIN_CW, ADDON_DIR_LOW);
			ioport_set_pin_level(ADDON_DIR_PIN_CCW, ADDON_DIR_LOW);
			addon_up = false;
			addon_down = false;
		}
		vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
	}
}