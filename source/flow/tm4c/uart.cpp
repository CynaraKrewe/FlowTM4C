/* The MIT License (MIT)
 *
 * Copyright (c) 2018 Cynara Krewe
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software, hardware and associated documentation files (the "Solution"), to deal
 * in the Solution without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Solution, and to permit persons to whom the Solution is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Solution.
 *
 * THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
 * SOLUTION.
 */

#include <assert.h>

#include "flow/tm4c/clock.h"
#include "flow/tm4c/uart.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

namespace Flow {
namespace TM4C {

UART::UART(UART::Number uartNumber, UART::Baudrate baudRate)
:	uartNumber(uartNumber),
	baudRate(baudRate)
{
	assert(uartNumber < UART::Number::COUNT);
	assert(baudRate < UART::Baudrate::COUNT);

	while(!SysCtlPeripheralReady(peripheral()))
	{
	    SysCtlPeripheralEnable(peripheral());
	}
}

UART::~UART()
{
    SysCtlPeripheralDisable(peripheral());
}

void UART::start()
{
    Frequency coreFrequency = Clock::instance().getFrequency();
    UARTConfigSetExpClk(base(), coreFrequency, uartBaudrate[(uint8_t)baudRate],
                                (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                 UART_CONFIG_PAR_NONE));
    UARTFIFODisable(base());

    UARTIntEnable(base(), (UART_INT_RX | UART_INT_TX));

    IntEnable(vector());
}

void UART::stop()
{
    IntDisable(vector());

    UARTIntDisable(base(), (UART_INT_RX | UART_INT_TX));
    UARTDisable(base());
}

void UART::run()
{
    trigger();
}

void UART::trigger()
{
    IntTrigger(vector());
}

void UART::isr()
{
    UARTIntClear(base(), UART_INT_RX);

	// Loop while there are characters in the receive FIFO.
	while(UARTCharsAvail(base()))
	{
		// Read the next character from the UART.
		out.send((char)UARTCharGet(base()));
	}

    UARTIntClear(base(), UART_INT_TX);

	char toTransmit;

	// Transmit FIFO not full?
	while(UARTSpaceAvail(base()) && in.receive(toTransmit))
	{
		UARTCharPut(base(), toTransmit);
	}
}

const uint32_t UART::_peripheral[] =
{
    SYSCTL_PERIPH_UART0,
    SYSCTL_PERIPH_UART1,
    SYSCTL_PERIPH_UART2,
    SYSCTL_PERIPH_UART3,
    SYSCTL_PERIPH_UART4,
    SYSCTL_PERIPH_UART5,
    SYSCTL_PERIPH_UART6,
    SYSCTL_PERIPH_UART7
};

uint32_t UART::peripheral() const
{
    return _peripheral[(uint8_t)uartNumber];
}

const uint32_t UART::_base[] =
{
    UART0_BASE,
    UART1_BASE,
    UART2_BASE,
    UART3_BASE,
    UART4_BASE,
    UART5_BASE,
    UART6_BASE,
    UART7_BASE
};

uint32_t UART::base() const
{
    return _base[(uint8_t)uartNumber];
}

const uint8_t UART::_vector[] =
{
    INT_UART0,
    INT_UART1,
    INT_UART2,
    INT_UART3,
    INT_UART4,
    INT_UART5,
    INT_UART6,
    INT_UART7
};

uint8_t UART::vector() const
{
    return _vector[(uint8_t)uartNumber];
}

const uint32_t UART::uartBaudrate[] =
{
    9600,
    19200,
    38400,
    76800,
    115200
};

} // namespace TM4C
} // namespace Flow
