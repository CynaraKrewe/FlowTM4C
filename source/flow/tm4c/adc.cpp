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

#include "flow/tm4c/adc.h"
#include "flow/tm4c/clock.h"

#include "driverlib/debug.h"
#include "driverlib/adc.h"

Adc::Adc()
{
	//
	// Enable the ADC modules.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

	//
	// Wait for the ADC modules to become ready.
	//
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1));


	//
	// Initializing/filling the sequence FIFO.
	// Using processor trigger
	//
	ADCSequenceConfigure(ADC0_BASE, SEQ0, ADC_TRIGGER_PROCESSOR, (uint8_t)Prio::Highest);

	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP0, ADC_CTL_CH9);

	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP1, ADC_CTL_CH0);

	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP2, ADC_CTL_CH1);

	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP3, ADC_CTL_CH2);

	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP4, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH3);

//	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP5, ADC_CTL_CH9);

//	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP6, ADC_CTL_CH9);

//	ADCSequenceStepConfigure(ADC0_BASE, SEQ0, STEP7, ADC_CTL_END | ADC_CTL_CH9);

	ADCSequenceEnable(ADC0_BASE, SEQ0);


	//
	// Initializing/filling the sequence FIFO.
	// Using continuous trigger
	//


	ADCSequenceConfigure(ADC1_BASE, SEQ3, ADC_TRIGGER_ALWAYS, (uint8_t)Prio::Low );

	ADCSequenceStepConfigure(ADC1_BASE, SEQ3, STEP0, ADC_CTL_IE |ADC_CTL_END | ADC_CTL_TS);

	ADCSequenceEnable(ADC1_BASE, SEQ3);

}

const uint32_t Adc::sysCtlPeripheral[] =
{
SYSCTL_PERIPH_ADC0,
SYSCTL_PERIPH_ADC1 };

const uint32_t Adc::AdcBase[] =
{
ADC0_BASE,
ADC1_BASE };

void Adc::run()
{
	//
	// Trigger the sample sequence 0.
	//
	ADCProcessorTrigger(ADC0_BASE, SEQ0);

	//Wait for sequencer to become ready.
	while(!ADCIntStatus(ADC0_BASE, 0, false))
	{
	}

	if (ADCSequenceDataGet(ADC0_BASE, SEQ0, &AdcBuffer[0]) == 5) // Check if 5 samples have been made
	{
		sampleAin9.send(AdcBuffer[0]);
		sampleAin0.send(AdcBuffer[1]);
		sampleAin1.send(AdcBuffer[2]);
		sampleAin2.send(AdcBuffer[3]);
		sampleAin3.send(AdcBuffer[4]);
	}


	//
	// No trigger for sample sequence 3 due to continuous mode.
	//


	if (ADCSequenceDataGet(ADC1_BASE, SEQ3, &AdcTempBuffer[0]) == 1)
	{
		sampleCpuTemperature.send(AdcTempBuffer[0]);
	}
}

