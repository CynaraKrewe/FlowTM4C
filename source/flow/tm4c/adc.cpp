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

#include "flow/tm4c/adc.h"
#include "flow/tm4c/clock.h"

#include "inc/hw_ints.h"

#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"

namespace Flow {
namespace TM4C {

const uint32_t ADC::_peripheral[] =
{
    SYSCTL_PERIPH_ADC0,
    SYSCTL_PERIPH_ADC1
};

const uint32_t ADC::_base[] =
{
    ADC0_BASE,
    ADC1_BASE
};

ADC::ADC(ADC::Number number)
:   number(number)
{
    assert(number < Number::COUNT);

    while(!SysCtlPeripheralReady(peripheral()))
    {
        SysCtlPeripheralEnable(peripheral());
    }
}

void ADC::start()
{
    SysCtlPeripheralReset(peripheral());
}

void ADC::stop()
{
    SysCtlPeripheralReset(peripheral());
}

uint32_t ADC::peripheral() const
{
    return _peripheral[(uint8_t)number];
}

uint32_t ADC::base() const
{
    return _base[(uint8_t)number];
}

ADC1::ADC1() : ADC(Number::_1)
{
}

ADC1::~ADC1()
{
}

void ADC1::start()
{
    ADC::start();

    ADCSequenceConfigure(base(), SEQ3, ADC_TRIGGER_PROCESSOR, (uint8_t)Prio::Low );

    ADCSequenceStepConfigure(base(), SEQ3, STEP0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_TS | ADC_CTL_SHOLD_256);

    ADCSequenceEnable(base(), SEQ3);

    ADCIntEnable(base(), SEQ3);

    IntEnable(INT_ADC1SS3);
}

void ADC1::stop()
{
    IntDisable(INT_ADC1SS3);

    ADC::stop();
}

void ADC1::run()
{
    ADCProcessorTrigger(base(), SEQ3);
}

void ADC1::trigger()
{
    IntTrigger(INT_ADC1SS3);
}

void ADC1::isr()
{
    ADCIntClear(base(), SEQ3);

    if(ADCSequenceDataGet(base(), SEQ3, &sample[0]) == 1)
    {
        outTemperature.send(sample[0]);
    }
}

} // namespace TM4C
} // namespace Flow
