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

#include <algorithm>
#include <numeric>

#include "flow/tm4c/clock.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"

#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "flow/tm4c/timer.h"

namespace Flow {
namespace TM4C {
namespace Timer {

Base::Base(Number number) :
		_number(number)
{
	assert(_number < Number::COUNT);
}

Number Base::number() const
{
	return _number;
}

uint8_t Base::vector(Channel channel) const
{
	return _vector[(unsigned int)number()][(unsigned int)channel];
}

uint32_t Base::peripheral() const
{
	return _peripheral[(unsigned int)number()];
}

uint32_t Base::base() const
{
	return _base[(unsigned int)number()];
}

SingleShot::SingleShot(Number number, uint16_t periodMs) :
		Base(number),
		periodMs(periodMs)
{
	while(!SysCtlPeripheralReady(peripheral()))
	{
		SysCtlPeripheralEnable(peripheral());
	}
}

SingleShot::~SingleShot()
{
	SysCtlPeripheralDisable(peripheral());
}

void SingleShot::start()
{
	TimerClockSourceSet(base(), TIMER_CLOCK_SYSTEM);
	TimerConfigure(base(), (TIMER_CFG_ONE_SHOT));

	IntEnable(vector(Channel::A));
}

void SingleShot::stop()
{
	IntDisable(vector(Channel::A));

	TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
	TimerDisable(base(), TIMER_A);
}

void SingleShot::run()
{
	bool start;
	if(inStart.receive(start))
	{
		if(start)
		{
			TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
			TimerDisable(base(), TIMER_A);
			TimerLoadSet(base(), TIMER_A, Clock::instance().getFrequency() / 1000 * periodMs);
			TimerIntEnable(base(), TIMER_TIMA_TIMEOUT);
			TimerEnable(base(), TIMER_A);
		}
		else
		{
			TimerIntDisable(base(), TIMER_TIMA_TIMEOUT);
			TimerDisable(base(), TIMER_A);
		}
	}
}

void SingleShot::isr()
{
	uint32_t status = TimerIntStatus(base(), true);
	TimerIntClear(base(), status);

	if(status & TIMER_TIMA_TIMEOUT)
	{
		outTick.send(TICK);
	}
}

void SingleShot::trigger()
{
}

const uint8_t Base::_vector[(unsigned int)Number::COUNT][(unsigned int)Channel::COUNT] =
{
	{ INT_TIMER0A, INT_TIMER0B },
	{ INT_TIMER1A, INT_TIMER1B },
	{ INT_TIMER2A, INT_TIMER2B },
	{ INT_TIMER3A, INT_TIMER3B },
	{ INT_TIMER4A, INT_TIMER4B },
	{ INT_TIMER5A, INT_TIMER5B },
	{ INT_TIMER6A, INT_TIMER6B },
	{ INT_TIMER7A, INT_TIMER7B }
};

const uint32_t Base::_peripheral[] =
{
    SYSCTL_PERIPH_TIMER0,
    SYSCTL_PERIPH_TIMER1,
    SYSCTL_PERIPH_TIMER2,
    SYSCTL_PERIPH_TIMER3,
    SYSCTL_PERIPH_TIMER4,
    SYSCTL_PERIPH_TIMER5,
    SYSCTL_PERIPH_TIMER6,
    SYSCTL_PERIPH_TIMER7
};

const uint32_t Base::_base[] =
{
	TIMER0_BASE,
    TIMER1_BASE,
    TIMER2_BASE,
    TIMER3_BASE,
    TIMER4_BASE,
    TIMER5_BASE,
    TIMER6_BASE,
    TIMER7_BASE
};

} // namespace Timer
} // namespace TM4C
} // namespace Flow
