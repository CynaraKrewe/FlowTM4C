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

#ifndef FLOW_DRIVER_CLOCK_H_
#define FLOW_DRIVER_CLOCK_H_

#include "frequency.h"

namespace Flow {
namespace Driver {

template<typename TargetSpecificClock>
class Clock
{
public:
    virtual ~Clock(){}

	virtual void configure(Frequency frequency) = 0;

	Frequency getFrequency()
	{
	    return frequency;
	}

	static TargetSpecificClock& instance()
	{
	    static TargetSpecificClock _instance;

	    return _instance;
	}

protected:
	Frequency frequency;

	Clock(){}
};

} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_CLOCK_H_ */
