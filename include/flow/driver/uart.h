/* The MIT License (MIT)
 *
 * Copyright (c) 2019 Cynara Krewe
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

#ifndef FLOW_DRIVER_UART_H_
#define FLOW_DRIVER_UART_H_

#include "flow/flow.h"

#include "flow/driver/isr.h"

namespace Flow {
namespace Driver {

/**
 * \brief Name space for all UART related classes.
 *
 * This contains target agnostic implementations and interfaces
 * to which target specific peripherals should adhere.
 */
namespace UART
{

class Buffer
{
public:
	Buffer(uint8_t* buffer, uint8_t length) : buffer(buffer), length(length){}
	uint8_t* buffer;
	uint8_t length;
};

/**
 * \brief Interface to be implemented by a target specific UART peripheral.
 */
class Peripheral: public Flow::Component, public WithISR
{
public:
	Flow::InPort<char> in{this};
	Flow::OutPort<char> out;

	virtual ~Peripheral()
	{
	}

	/**
	 * \brief Configure and enable the peripheral.
	 */
	virtual void start() = 0;

	/**
	 * \brief Disable the peripheral.
	 */
	virtual void stop() = 0;
};

} // namespace UART
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_UART_H_ */
