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

#include "flow/tm4c/clock.h"

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

namespace Flow {
namespace TM4C {

template<>
void Clock::configure<Device::TM4C129>(Frequency frequency)
{
	FPULazyStackingEnable();

	this->frequency = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ
			| SYSCTL_OSC_MAIN
			| SYSCTL_USE_PLL
			| SYSCTL_CFG_VCO_480),
			frequency);

	SysTickPeriodSet(this->frequency / 1000); // 1ms
	SysTickIntEnable();
	SysTickEnable();
}

template<>
void Clock::configure<Device::TM4C123>(Frequency frequency)
{
	FPULazyStackingEnable();

	uint8_t divisor = 200 MHz / frequency;

	assert(2 < divisor && divisor <= 16);

	this->frequency = 200 MHz / divisor;

	SysCtlClockSet((SYSCTL_OSC_INT
			| SYSCTL_USE_PLL
			| ((divisor - 1) << 23)));

	SysTickPeriodSet(this->frequency / 1000); // 1ms
	SysTickIntEnable();
	SysTickEnable();
}

uint32_t Clock::now() const
{
	return _now;
}

void Clock::trigger()
{
}

void Clock::isr()
{
	_now++;
}

} // namespace TM4C
} // namespace Flow
