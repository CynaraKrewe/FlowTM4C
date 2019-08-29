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

#ifndef FLOW_TM4C_ADC_H_
#define FLOW_TM4C_ADC_H_

#include <stdint.h>

#include "flow/driver/isr.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

namespace Flow {
namespace TM4C {

class ADC: public Flow::Component, public Flow::Driver::WithISR
{
public:
	enum class Base : uint8_t
	{
		_0 = 0,
		_1,
		COUNT
	};

	enum Sequence
	{
		SEQ0 = 0,
		SEQ1,
		SEQ2,
		SEQ3,
		SEQ_COUNT
	};


	enum Step
	{
		STEP0= 0,
		STEP1,
		STEP2,
		STEP3,
		STEP4,
		STEP5,
		STEP6,
		STEP7,
		STEP_COUNT
	};

 	enum class Number : uint8_t
	{
		_0 = 0,
		_1,
		COUNT
	};

 	enum class Prio : uint8_t
	{
		Highest = 0,
		High,
		Medium,
		Low
	};

 	ADC(Number number);
 	virtual ~ADC(){}

protected:
    void start() override;
    void stop() override;

    uint32_t peripheral() const;
    uint32_t base() const;

private:
    Number number;

	static const uint32_t _peripheral[(uint8_t)Number::COUNT];
	static const uint32_t _base[(uint8_t)Number::COUNT];
};

class ADC1 : public ADC
{
public:
    Flow::InPort<Tick> inTrigger{this};
    Flow::OutPort<uint32_t> outTemperature;

    ADC1();
    ~ADC1();

    void start() final override;
    void stop() final override;

    void run() final override;

    void isr() final override;

private:
    void trigger() final override;

    uint32_t sample[1];
};

} // namespace TM4C
} // namespace Flow

#endif // FLOW_TM4C_ADC_H_
