/*
The MIT License (MIT)

Copyright (c) 2016 Cynara Krewe

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

#include <assert.h>

#include "flow/tm4c/usb_cdc.h"
#include "flow/driver/frequency.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"

extern "C" void USB0DeviceIntHandler(void);

namespace Flow {
namespace TM4C {

UsbCdc::UsbCdc()
{
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_USB0))
    {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    }
}

UsbCdc::~UsbCdc()
{
    SysCtlPeripheralDisable(SYSCTL_PERIPH_USB0);
}

void UsbCdc::start()
{
    USBStackModeSet(0, eUSBModeDevice, nullptr);

    uint32_t sysClock = 120 MHz;
    USBDCDFeatureSet(0, USBLIB_FEATURE_CPUCLK, &sysClock);
    uint32_t pll;
    SysCtlVCOGet(SYSCTL_XTAL_25MHZ, &pll);
    USBDCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &pll);

    void* device = USBDCDCInit(0, &cdcDevice);
    assert(device != nullptr);
    (void)device;
}

void UsbCdc::stop()
{
    USBDCDCTerm(&cdcDevice);
}

void UsbCdc::run()
{
    trigger();
}

void UsbCdc::trigger()
{
    IntTrigger(INT_USB0);
}

void UsbCdc::isr()
{
    transmitHandler();

    USB0DeviceIntHandler();
}

uint32_t UsbCdc::controlHandler(uint32_t event)
{
    switch(event)
    {
    case USB_EVENT_CONNECTED:
    {
        _connected = true;
    } break;
    case USB_EVENT_DISCONNECTED:
    {
        _connected = false;
    } break;
    default: break;
    }

    return 0;
}

uint32_t UsbCdc::receiveHandler(uint32_t event)
{
    switch(event)
    {
    case USB_EVENT_RX_AVAILABLE:
    case USB_EVENT_DATA_REMAINING:
    {
        while(USBDCDCRxPacketAvailable(&cdcDevice) > 0 && !out.full())
        {
            char received;
            uint32_t count = USBDCDCPacketRead(&cdcDevice, (uint8_t*)&received, 1, true);
            if(count)
            {
                out.send(received);
            }
        }
    } break;
    default: break;
    }

    return USBDCDCRxPacketAvailable(&cdcDevice);
}

uint32_t UsbCdc::transmitHandler()
{
    char toTransmit;
    while(USBDCDCTxPacketAvailable(&cdcDevice) > 0 && in.receive(toTransmit))
    {
        USBDCDCPacketWrite(&cdcDevice, (uint8_t*)&toTransmit, 1, true);
    }
    USBDCDCPacketWrite(&cdcDevice, nullptr, 0, false);

    return 0;
}

uint32_t UsbCdc::controlHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData)
{
    UsbCdc* cdc = (UsbCdc*)callbackData;

    return cdc->controlHandler(event);
}

uint32_t UsbCdc::receiveHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData)
{
    UsbCdc* cdc = (UsbCdc*)callbackData;

    return cdc->receiveHandler(event);
}

uint32_t UsbCdc::transmitHandler(void *callbackData, uint32_t event, uint32_t messageValue, void *messageData)
{
    UsbCdc* cdc = (UsbCdc*)callbackData;

    return cdc->transmitHandler();
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
    'C', 0, 'y', 0, 'n', 0, 'a', 0, 'r', 0, 'a', 0, 'K', 0, 'r', 0, 'e', 0, 'w', 0, 'e', 0
};

const uint8_t UsbCdc::productString[] =
{
    2 + (8 * 2),
    USB_DTYPE_STRING,
    'F', 0, 'l', 0, 'o', 0, 'w', 0, 'T', 0, 'M', 0, '4', 0, 'C', 0
};

const uint8_t UsbCdc::serialNumberString[] =
{
    2 + (1 * 2),
    USB_DTYPE_STRING,
    '0', 0
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

} // namespace TM4C
} // namespace Flow
