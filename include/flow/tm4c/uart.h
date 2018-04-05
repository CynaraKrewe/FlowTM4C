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

#ifndef TM4C_UART_H_
#define TM4C_UART_H_

#include <stdint.h>

#include "flow/flow.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

class Uart
:	public Flow::Component
{
public:
	enum class Number : uint8_t
	{
		_0 = 0,
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		COUNT
	};

	enum class Baudrate : uint8_t
	{
		_9600 = 0,
		_19200,
		_38400,
		_76800,
		_115200,
		COUNT
	};

	Flow::OutPort<char> out;
	Flow::InPort<char> in;

	Uart(Number uartNumber, Baudrate baudRate = Baudrate::_9600);

	void run() final override;

protected:
	const Number uartNumber;
	const Baudrate baudRate;

	static const uint32_t sysCtlPeripheral[(uint8_t)Number::COUNT];
	static const uint32_t uartBase[(uint8_t)Number::COUNT];
	static const uint32_t uartBaudrate[(uint8_t)Baudrate::COUNT];
};

#endif // TM4C_UART_H_
