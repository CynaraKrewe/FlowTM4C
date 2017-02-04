/*
The MIT License (MIT)

Copyright (c) 2017 Cynara Krewe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software, hardware and associated documentation files (the "Solution"), to deal
in the Solution without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Solution, and to permit persons to whom the Solution is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Solution.

THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
SOLUTION.
 */

#include "flow/tm4c/frequency.h"
#include "flow/tm4c/usb_cdc.h"

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

void UsbCdc::run()
{
	char toTransmit;
	while(USBDCDCTxPacketAvailable(device) > 0 && in.receive(toTransmit))
	{
		USBDCDCPacketWrite(device, (uint8_t*)&toTransmit, 1, true);
	}
	USBDCDCPacketWrite(device, nullptr, 0, false);
}

UsbCdc::UsbCdc()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0xff;
	GPIOPinConfigure(GPIO_PD6_USB0EPEN);
	GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUSBDigital(GPIO_PORTD_BASE, GPIO_PIN_6);
	GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinTypeGPIOInput(GPIO_PORTQ_BASE, GPIO_PIN_4);

	uint32_t sysClock = 120 MHz;
	USBDCDFeatureSet(0, USBLIB_FEATURE_CPUCLK, &sysClock);
	uint32_t pll = SYSCTL_CFG_VCO_480;
	USBDCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &pll);

	device = USBDCDCInit(0, &cdcDevice);
	ASSERT(device != nullptr);
}

UsbCdc::~UsbCdc()
{
	USBDCDCTerm(device);
}

uint32_t UsbCdc::ReceiveHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData)
{
	UsbCdc* cdc = (UsbCdc*)callbackData;

	switch(event)
	{
	case USB_EVENT_RX_AVAILABLE:
	case USB_EVENT_DATA_REMAINING:

		while(USBDCDCRxPacketAvailable(cdc->device) > 0 && !cdc->out.full())
		{
			char received;
			USBDCDCPacketRead(cdc->device, (uint8_t*)&received, 1, true);
			cdc->out.send(received);
		}

		break;
	default:
		break;
	}

	return USBDCDCRxPacketAvailable(cdc->device);

}

uint32_t UsbCdc::TransmitHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData)
{
	switch(event)
	{
	case USB_EVENT_TX_COMPLETE:
		break;
	default:
		break;
	}

	return 0; // Ignored by the USB library.
}

uint32_t UsbCdc::ControlHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData)
{
	return 0;
}

const uint8_t UsbCdc::languageDescriptor[] =
{
	4,
	USB_DTYPE_STRING,
	USBShort(USB_LANG_EN_US)
};

const uint8_t UsbCdc::manufacturerString[] =
{
	2 + (11 * 2),
	USB_DTYPE_STRING,
	'C', 0, 'y', 0, 'n', 0, 'a', 0, 'r', 0, 'a', 0,
	'K', 0, 'r', 0, 'e', 0, 'w', 0, 'e', 0
};

const uint8_t UsbCdc::productString[] =
{
	2 + (12 * 2),
	USB_DTYPE_STRING,
	'C', 0, 'y', 0, 'n', 0, 'a', 0, 'r', 0, 'a', 0,
	'V', 0, 'i', 0, 'r', 0, 'i', 0, 'd', 0, 'e', 0
};

const uint8_t UsbCdc::serialNumberString[] =
{
	2 + (4 * 2),
	USB_DTYPE_STRING,
	'1', 0, '3', 0, '3', 0, '7', 0
};

const uint8_t UsbCdc::controlInterfaceString[] =
{
	2 + (21 * 2),
	USB_DTYPE_STRING,
	'A', 0, 'C', 0, 'M', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 't', 0,
	'r', 0, 'o', 0, 'l', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0,
	'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

const uint8_t UsbCdc::configString[] =
{
	2 + (26 * 2),
	USB_DTYPE_STRING,
	'S', 0, 'e', 0, 'l', 0, 'f', 0, ' ', 0, 'P', 0, 'o', 0, 'w', 0,
	'e', 0, 'r', 0, 'e', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0,
	'f', 0, 'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0,
	'o', 0, 'n', 0
};

const uint8_t* const UsbCdc::stringDescriptors[] =
{
	languageDescriptor,
	manufacturerString,
	productString,
	serialNumberString,
	controlInterfaceString,
	configString
};
