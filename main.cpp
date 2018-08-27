/*
 * File name  : main.cpp
 * Description: CJMCU-75 LM75A Temperature Sensor Breakout Board  
 * No : 03
 * Note: using thread, ticker, event loop, and splitting code into two parts
 *      1) portion in interrupt context
 *      2) portion in user context (slow portion, e.g. printf, i2c operation)
 *
 */
#include "mbed.h"
#include "mbed_events.h"

#include "cjmcu75.h"

#define CJMCU75_I2C_ADDR	(0x48<<1)	//-- mbed take 8-bit address

float		tempC;
DigitalOut	led(LED1);
Ticker		tick;
Thread		t;

EventQueue	queue(32 * EVENTS_EVENT_SIZE);

CJMCU75		sensor(I2C_SDA, I2C_SCL, (int)CJMCU75_I2C_ADDR);

void get_temp_interrupt_context(void);
void get_temp_user_context(void);

//-- main() ----------------------------------------------------------
int main()
{
	printf("\r\nStarting temperature measurement every 3 seconds\r\n");
	printf("Initializing CJMCU-75 temperature sensor\r\n");
	sensor.init();

	char id = sensor.get_id();
	printf("device id = %x\r\n", id);
	
	//-- start event queue thread
	t.start(callback(&queue, &EventQueue::dispatch_forever));

	//-- create event every 3 seconds
	tick.attach(get_temp_interrupt_context, 3.0);
}

/* in interrupt context */
void get_temp_interrupt_context(void)
{
	led = !led;
	queue.call(get_temp_user_context);
}

/* in user context */
void get_temp_user_context(void)
{
	tempC = sensor.get_temp();
	printf("%.2f\r\n", tempC);
}
//-- end of program --------------------------------------------------
