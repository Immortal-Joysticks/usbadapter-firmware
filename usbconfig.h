/*
	V-USB configuration for Immortal Joysticks USB adapter
*/

#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

/* ---------------------------- Hardware Config ---------------------------- */

#if defined(TYPE_ATTINYx4)
	// PCB using ATtiny24A/44A/84A

	#define USB_CFG_IOPORTNAME		A
	#define USB_CFG_DMINUS_BIT		0
	#define USB_CFG_DPLUS_BIT		1


	#define USB_INTR_CFG            PCMSK0
	#define USB_INTR_CFG_SET        (1<<USB_CFG_DPLUS_BIT)
	#define USB_INTR_ENABLE_BIT     PCIE0
	#define USB_INTR_PENDING        GIFR
	#define USB_INTR_PENDING_BIT    PCIF0
	#define USB_INTR_VECTOR         PCINT0_vect

#elif defined(TYPE_ATMEGA48)
	// PCB using ATmega48PA, breadboard using ATmega328P

	#define USB_CFG_IOPORTNAME		D
	#define USB_CFG_DMINUS_BIT		2
	#define USB_CFG_DPLUS_BIT		3

	#define USB_INTR_CFG_SET		((1 << ISC10) | (1 << ISC11))
	#define USB_INTR_ENABLE_BIT		INT1
	#define USB_INTR_PENDING_BIT	INTF1
	#define USB_INTR_VECTOR			INT1_vect
#else
	#error Unknown AVR type !
#endif

#define USB_CFG_CLOCK_KHZ		(F_CPU/1000)
#define USB_CFG_CHECK_CRC		0

/* --------------------------- Functional Range ---------------------------- */

#define USB_CFG_HAVE_INTRIN_ENDPOINT		1
#define USB_CFG_HAVE_INTRIN_ENDPOINT3		0
#define USB_CFG_EP3_NUMBER					3
#define USB_CFG_IMPLEMENT_HALT				0
#define USB_CFG_SUPPRESS_INTR_CODE			0
#define USB_CFG_INTR_POLL_INTERVAL			10
#define USB_CFG_IS_SELF_POWERED				0
#define USB_CFG_MAX_BUS_POWER				100
#define USB_CFG_IMPLEMENT_FN_WRITE			0
#define USB_CFG_IMPLEMENT_FN_READ			0
#define USB_CFG_IMPLEMENT_FN_WRITEOUT		0
#define USB_CFG_HAVE_FLOWCONTROL			0
#define USB_CFG_DRIVER_FLASH_PAGE			0
#define USB_CFG_LONG_TRANSFERS				0
#define USB_COUNT_SOF						0
#define USB_CFG_CHECK_DATA_TOGGLING			0
#define USB_CFG_HAVE_MEASURE_FRAME_LENGTH	0
#define USB_USE_FAST_CRC					0

/* -------------------------- Device Description --------------------------- */

#define  USB_CFG_VENDOR_ID					0xc0, 0x16			// VID = voti.nl
#define  USB_CFG_DEVICE_ID					0xdc, 0x27			// PID = HID gamepad

#define USB_CFG_VENDOR_NAME					'I', 'm', 'm', 'o', 'r', 't', 'a', 'l', ' ', 'J', 'o', 'y', 's', 't', 'i', 'c', 'k', 's'
#define USB_CFG_VENDOR_NAME_LEN				18

#define USB_CFG_DEVICE_NAME					'I', 'm', 'm', 'o', 'r', 't', 'a', 'l', ' ', 'J', 'o', 'y', 's', 't', 'i', 'c', 'k', 's', ' ', 'U', 'S', 'B', ' ', 'A', 'd', 'a', 'p', 't', 'e', 'r'
#define USB_CFG_DEVICE_NAME_LEN				30

#define USB_CFG_SERIAL_NUMBER				'I', 'M', 'J', 'O', 'Y', '0', '0', '0', '1'
#define USB_CFG_SERIAL_NUMBER_LEN			9

#define USB_CFG_DEVICE_VERSION				0x00, 0x01			/* Version number of the device: Minor number first, then major number */

#define USB_CFG_DEVICE_CLASS				0
#define USB_CFG_DEVICE_SUBCLASS				0
#define USB_CFG_INTERFACE_CLASS				3				// HID device
#define USB_CFG_INTERFACE_SUBCLASS			0
#define USB_CFG_INTERFACE_PROTOCOL			0

#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH	42				// should match usbHidReportDescriptor in main.c

#define USB_CFG_DESCR_PROPS_DEVICE                  0
#define USB_CFG_DESCR_PROPS_CONFIGURATION           0
#define USB_CFG_DESCR_PROPS_STRINGS                 0
#define USB_CFG_DESCR_PROPS_STRING_0                0
#define USB_CFG_DESCR_PROPS_STRING_VENDOR           0
#define USB_CFG_DESCR_PROPS_STRING_PRODUCT          0
#define USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER    0
#define USB_CFG_DESCR_PROPS_HID                     0
#define USB_CFG_DESCR_PROPS_HID_REPORT              0
#define USB_CFG_DESCR_PROPS_UNKNOWN                 0


#define usbMsgPtr_t unsigned short

#endif /* __usbconfig_h_included__ */
