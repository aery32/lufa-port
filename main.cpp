#include <aery32/all.h>
#include "board.h"

#include <LUFA/Common/Common.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include <LUFA/Version.h>
#include "LUFA/Descriptors.h"

#define LED AVR32_PIN_PC00

using namespace aery;

/*
 * LUFA CDC Class driver interface configuration and state information.
 * This structure is passed to all CDC Class driver functions, so that
 * multiple instances of the same class within a device can be
 * differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface = {
	{ /* Config */
		0, /* ControlInterfaceNumber */
		{ /* DataINEndpoint */
			CDC_TX_EPADDR, /* Address */
			CPU_TO_LE16(CDC_TXRX_EPSIZE), /* Size */
			EP_TYPE_CONTROL, /* Endpoint type */
			1, /* Banks */
		},
		{ /* DataOUTEndpoint */
			CDC_RX_EPADDR, /* Address */
			CPU_TO_LE16(CDC_TXRX_EPSIZE), /* Size */
			EP_TYPE_CONTROL, /* Endpoint type */
			1, /* Banks */
		},
		{ /* NotificationEndpoint */
			CDC_NOTIFICATION_EPADDR, /* Address */
			CPU_TO_LE16(CDC_NOTIFICATION_EPSIZE), /* Size */
			EP_TYPE_CONTROL, /* Endpoint type */
			1, /* Banks */
		}
	}
};

/* Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/* Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/* Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/* Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);	
}

int main(void)
{
	/*
	 * The default board initializer defines all pins as input and
	 * sets the CPU clock speed to 66 MHz.
	 */
	board::init();
	gpio_init_pin(LED, GPIO_OUTPUT);

	/* Set up the USB generic clock to 48MHz, f_pll1 / 2 == 48MHz. */
	pm_init_gclk(GCLK_USBB, GCLK_SOURCE_PLL1, 1);
	pm_enable_gclk(GCLK_USBB);

	/* Register USB isr handler, from LUFA library */
	INTC_Init();
	INTC_RegisterGroupHandler(INTC_IRQ_GROUP(AVR32_USBB_IRQ), 1, &USB_GEN_vect);
	GlobalInterruptEnable();

	/* Initiliaze the USB, LUFA magic */
	USB_Init();

	/* All init done, turn the LED on */
	gpio_set_pin_high(LED);

	for(;;) {
		/* Put your application code here */

		CDC_Device_SendString(&VirtualSerial_CDC_Interface,
			"Hello! You're running LUFA version "
			LUFA_VERSION_STRING " on Aery32 :)\r\n"
		);
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}

	return 0;
}