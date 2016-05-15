/*
 * console_func.c
 *
 * Created: 3/22/2016 12:56:42
 *  Author: jonathan
 */ 
#include <asf.h>

void configure_console(void)
/* Enables serial communication via the USB interface. */
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}
