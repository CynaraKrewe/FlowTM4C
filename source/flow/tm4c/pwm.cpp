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

#include <assert.h>

#include "flow/tm4c/pwm.h"
#include "flow/tm4c/clock.h"

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"

namespace Flow {
namespace TM4C {

Pwm::Pwm(Divider divider, Frequency generatorFrequency[GENERATOR_COUNT])
:   divider(divider)
{
    for(unsigned int g = 0; g < GENERATOR_COUNT; g++)
    {
        this->generatorFrequency[g] = generatorFrequency[g];
    }

    for(unsigned int o = 0; o < OUTPUT_COUNT; o++)
    {
        inDutyCycleOutput[o] = new Flow::InPort<DutyCycle>{this};
    }

	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
	{
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	}
}

Pwm::~Pwm()
{
    SysCtlPeripheralDisable(SYSCTL_PERIPH_PWM0);
}

void Pwm::start()
{
    PWMClockSet(PWM0_BASE, (uint32_t)divider);
    inputClockFrequency = Clock::instance().getFrequency() / decodeDivider(divider);

    for(unsigned int g = 0; g < GENERATOR_COUNT; g++)
    {
        PWMGenConfigure(PWM0_BASE, (uint32_t)GENERATOR[g],
                PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC
                | PWM_GEN_MODE_DBG_STOP | PWM_GEN_MODE_GEN_NO_SYNC
                | PWM_GEN_MODE_DB_NO_SYNC | PWM_GEN_MODE_FAULT_UNLATCHED
                | PWM_GEN_MODE_FAULT_NO_MINPER | PWM_GEN_MODE_FAULT_LEGACY);

        PWMGenEnable(PWM0_BASE, (uint32_t)GENERATOR[g]);

        uint16_t value = (uint16_t)(inputClockFrequency / generatorFrequency[g]);
        PWMGenPeriodSet(PWM0_BASE, (uint32_t)GENERATOR[g], value);
    }
}

void Pwm::stop()
{
    for(unsigned int g = 0; g < GENERATOR_COUNT; g++)
    {
        PWMGenDisable(PWM0_BASE, (uint32_t)GENERATOR[g]);
    }
}

void Pwm::run()
{
	for(unsigned int g = 0; g < GENERATOR_COUNT; g++)
	{
		unsigned int outputOffset = g * OUTPUT_FOR_GENERATOR;

		for(unsigned int o = 0; o < OUTPUT_FOR_GENERATOR; o++)
		{
			unsigned int output = outputOffset + o;

			DutyCycle dutyCycle;
			if(inDutyCycleOutput[output]->receive(dutyCycle))
			{
				assert((-0.001f < dutyCycle) && (dutyCycle < 100.001f));
				uint32_t period = PWMGenPeriodGet(PWM0_BASE, (uint32_t)GENERATOR[g]);
				uint32_t threshold = (uint32_t)(((period - 1) * dutyCycle) / 100.0f);
				PWMPulseWidthSet(PWM0_BASE, (uint32_t)OUTPUT[output], threshold);
				PWMOutputState(PWM0_BASE, (1 << output), (dutyCycle > 0.0f));
//				PWMSyncUpdate(PWM0_BASE, PWM_GEN_0_BIT | PWM_GEN_1_BIT | PWM_GEN_2_BIT | PWM_GEN_3_BIT);
			}
		}
	}
}

unsigned int Pwm::decodeDivider(Divider divider)
{
	unsigned int decoded = 1;

	switch(divider)
	{
	case Divider::_1:
		decoded = 1;
		break;
	case Divider::_2:
		decoded = 2;
		break;
	case Divider::_4:
		decoded = 4;
		break;
	case Divider::_8:
		decoded = 8;
		break;
	case Divider::_16:
		decoded = 16;
		break;
	case Divider::_32:
		decoded = 32;
		break;
	case Divider::_64:
		decoded = 64;
		break;
	}

	return decoded;
}

const Pwm::Generator Pwm::GENERATOR[GENERATOR_COUNT] =
{
	Pwm::Generator::_0,
	Pwm::Generator::_1,
	Pwm::Generator::_2,
	Pwm::Generator::_3
};

const Pwm::Output Pwm::OUTPUT[OUTPUT_COUNT] =
{
	Pwm::Output::_0,
	Pwm::Output::_1,
	Pwm::Output::_2,
	Pwm::Output::_3,
	Pwm::Output::_4,
	Pwm::Output::_5,
	Pwm::Output::_6,
	Pwm::Output::_7
};

} // namespace TM4C
} // namespace Flow
