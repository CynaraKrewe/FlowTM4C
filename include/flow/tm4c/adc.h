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

#ifndef TM4C_ADC_H_
#define TM4C_ADC_H_

#include <stdint.h>

#include "flow/flow.h"

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

class Adc: public Flow::Component
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


	Adc();

	void run();

	Flow::OutPort<uint32_t> sampleAin9;
	Flow::OutPort<uint32_t> sampleAin0;
	Flow::OutPort<uint32_t> sampleAin1;
	Flow::OutPort<uint32_t> sampleAin2;
	Flow::OutPort<uint32_t> sampleAin3;
	Flow::OutPort<uint32_t> sampleCpuTemperature;





private:
	uint32_t AdcBuffer[8]; // 8 for Seq0, 1 for Seq3
	uint32_t AdcTempBuffer[1]; // 8 for Seq0, 1 for Seq3

	static const uint32_t sysCtlPeripheral[(uint8_t) Number::COUNT];
	static const uint32_t AdcBase[(uint8_t) Number::COUNT];
};


#endif // TM4C_ADC_H_
