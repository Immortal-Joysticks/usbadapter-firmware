/*
	Immortal Joysticks USB adapter

	Written by Peter Mulholland and Andrew Hutchings
	Licensed under GPL v2 or later - see LICENSE
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/wdt.h>

#include "usbdrv/usbdrv.h"

FUSES = {
#if defined(TYPE_ATTINYx4)
    .low = 0xDF,
    .high = 0xDF,
#elif defined (TYPE_ATMEGA48)
    .low = 0xDF,
    .high = 0xDF,
#else
	#error Unknown AVR type !
#endif
    .extended = EFUSE_DEFAULT
};

// X/Y joystick w/ 8-bit readings (-127 to +127), 8 digital buttons
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] =
{
	0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x04,		// USAGE (Joystick)
	0xa1, 0x01,		// COLLECTION (Application)
		0xa1, 0x02,		//   COLLECTION (Physical)
			0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
			0x09, 0x30,		//     USAGE (X)
			0x09, 0x31,		//     USAGE (Y)
			0x15, 0x00,		//   LOGICAL_MINIMUM (0)
			0x26, 0xff,		//   LOGICAL_MAXIMUM (255)
			0x00,
			0x75, 0x08,		//   REPORT_SIZE (8)
			0x95, 0x02,		//   REPORT_COUNT (2)
			0x81, 0x02,		//   INPUT (Data,Var,Abs)

			0x05, 0x09,		// USAGE_PAGE (Button)
			0x19, 0x01,		//   USAGE_MINIMUM (Button 1)
			0x29, 0x08,		//   USAGE_MAXIMUM (Button 8)
			0x15, 0x00,		//   LOGICAL_MINIMUM (0)
			0x25, 0x01,		//   LOGICAL_MAXIMUM (1)
			0x75, 0x01,		// REPORT_SIZE (1)
			0x95, 0x08,		// REPORT_COUNT (8)
			0x81, 0x02,		// INPUT (Data,Var,Abs)
		0xc0,			// END_COLLECTION
	0xc0			// END_COLLECTION
};

// Our hid status report (x and y position, and 8 buttons)
struct hidReport
{
	uint8_t xpos;
	uint8_t ypos;
	uint8_t buttons;
} rept;

uint8_t usbFunctionSetup(uint8_t data[8])
{
	usbRequest_t *rq = (usbRequest_t *) data;

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_CLASS)
		return 0;

	switch (rq->bRequest)
	{
		case USBRQ_HID_GET_REPORT:
			usbMsgPtr = (usbMsgPtr_t) &rept;
			return sizeof(rept);

		// TODO: should we do these?
		//case USBRQ_HID_GET_IDLE:
		//case USBRQ_HID_SET_IDLE:

		default:
			return 0;
	}
}

void initPort()
{
#if defined(TYPE_ATTINYx4)
	// Inputs on PA2-7 and PB2

	DDRA &= ~((1<<DDA2)   | (1<<DDA3)   | (1<<DDA4) | (1<<DDA5) | (1<<DDA6) | (1<<DDA7));
	PORTA =  ((1<<PORTA2) | (1<<PORTA3) | (1<<PORTA4) | (1<<PORTA5 | (1<<PORTA6) | (1<<PORTA7)));

	DDRB &= ~(1<<DDB2);
	PORTB =  (1<<PORTB2);

	_NOP();
#elif defined(TYPE_ATMEGA48)
	// Inputs on PD4-6 and PB0-2

	DDRD &= ~((1<<DDD4)   | (1<<DDD5)   | (1<<DDD6) | (1<<DDD7));
	PORTD =  ((1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7));

	DDRB &= ~((1<<DDB0)   | (1<<DDB1)   | (1<<DDB2));
	PORTB =  ((1<<PORTB0) | (1<<PORTB1) | (1<<PORTB2));
	_NOP();
#else
	#error Unknown AVR type !
#endif
}

static int loop_timer= 0;
static int hold_start= 0;
static bool b_pressed = false;

void readPort(struct hidReport *r)
{
#if defined(TYPE_ATTINYx4)
	uint8_t pins = PINA;

	if      (!(pins & (1<<PINA4)))	r->xpos = 0x00;				// left
	else if (!(pins & (1<<PINA3)))	r->xpos = 0xff;			// right
	else	r->xpos = 0x7f;

	if      (!(pins & (1<<PINA6)))	r->ypos = 0x00;				// up
	else if (!(pins & (1<<PINA5)))	r->ypos = 0xff;			// down
	else	r->ypos = 0x7f;

	r->buttons = 0;
	if (!(pins & (1<<PINA7)))	r->buttons |= (1<<1);			// FIRE1
	if (!(pins & (1<<PINA2)))	r->buttons |= (1<<0);			// FIRE2

	if (!(PINB & (1<<PINB2))) {
		if (loop_timer >= 15000) {
			r->buttons |= (1<<0);
			b_pressed = true;
			return;
		}
		loop_timer++;
	} else if (loop_timer) {
		if (b_pressed) {
			b_pressed = false;
			loop_timer= 0;
		} else if (hold_start < 1500) {
			r->buttons |= (1<<7);			// START
			hold_start++;
		} else {
			loop_timer = 0;
			hold_start = 0;
		}
	}

#elif defined(TYPE_ATMEGA48)
	uint8_t pb = PINB;
	uint8_t pd = PIND;

	if      (!(pb & (1<<PINB0)))	r->xpos = 0x00;				// left
	else if (!(pb & (1<<PINB1)))	r->xpos = 0xff;				// right
	else	r->xpos = 0x7f;

	if      (!(pd & (1<<PIND5)))	r->ypos = 0x00;				// up
	else if (!(pd & (1<<PIND7)))	r->ypos = 0xff;				// down
	else	r->ypos = 0x7f;

	r->buttons = 0;
	if (!(pd & (1<<PIND6)))	r->buttons |= (1<<1);			// FIRE1
	if (!(pb & (1<<PINB2)))	r->buttons |= (1<<0);			// FIRE2
	if (!(pd & (1<<PIND4))) r->buttons |= (1<<7);			// START
#else
	#error Unknown AVR type !
#endif
}

int main(void)
{
	int i = 0;
	// Don't use the watchdog timer
	wdt_disable();

	// Initialise V-USB
	usbInit();

	// Set up input ports
	initPort();

	// Start with stick centred and no buttons pressed
	rept.xpos = rept.ypos = 0;
	rept.buttons = 0;

	// Enable interrupts (the fun starts now)
	sei();

	// Continuous loop.. read inputs, send it out to USB if it changed...
	while (1)
	{
		usbPoll();
		if (!usbInterruptIsReady())
			continue;

		// Read joystick
		struct hidReport newRept;
		readPort(&newRept);

		// Every so often send the current position anyway, this helps with the
		// fact that we are 0, 0 on startup.
		i++;

		// Send the new values out if they've changed
		if ((rept.xpos != newRept.xpos) || (rept.ypos != newRept.ypos) || (rept.buttons != newRept.buttons) || i >= 255)
		{
			rept = newRept;
			i = 0;
			usbSetInterrupt((unsigned char *) &rept, sizeof(struct hidReport));
		}
	}
}
