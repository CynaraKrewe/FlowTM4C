/*
 The MIT License (MIT)

 Copyright (c) 2017 Cynara Krewe

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software, hardware and associated documentation files (the "Solution"), to deal
 in the Solution without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Solution, and to permit persons to whom the Solution is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Solution.

 THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
 SOLUTION.
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driverlib/debug.h"

#include "pinmux/pinout.h"

#include "flow/components.h"
#include "flow/utility.h"

#include "flow/tm4c/usb_cdc.h"
#include "flow/tm4c/clock.h"
#include "flow/tm4c/pwm.h"
#include "flow/tm4c/uart.h"
#include "flow/tm4c/gpio.h"

static Flow::Component** _sysTickComponents = nullptr;
static unsigned int _sysTickComponentsCount = 0;

int main(void)
{
	// Set up the clock circuit.
	Clock::instance()->configure(120 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	Timer* timer = new Timer(200 /*ms*/);
	Split<Tick, 3>* tickSplit = new Split<Tick, 3>();

	DigitalOutput* led0 = new DigitalOutput(Gpio::Name{ Gpio::Port::N, 1 });
	DigitalOutput* led1 = new DigitalOutput(Gpio::Name{ Gpio::Port::N, 0 });
	DigitalOutput* led2 = new DigitalOutput(Gpio::Name{ Gpio::Port::F, 4 });

	Toggle* toggleLed = new Toggle[3]();

	UsbCdc* cdc = new UsbCdc();

	Timer* timerP = new Timer(20 /*ms*/);
	UpDownCounter<Tick>* counterP = new UpDownCounter<Tick>(0, 50, 0);
	Convert<unsigned int, float>* convertP = new Convert<unsigned int, float>();
	Frequency pwmFrequency[4] =	{ 1 kHz, 0, 0, 0 };

	Pwm* pwmP = new Pwm(Pwm::Divider::_64, pwmFrequency);

	Uart* debug = new Uart(Uart::Number::_0, Uart::Baudrate::_115200);

	// Connect the components of the application.
	Flow::Connection* connections[] =
	{
		Flow::connect(cdc->out, cdc->in, 40),

		Flow::connect(timer->outTick, tickSplit->in),
		Flow::connect(tickSplit->out[0], toggleLed[0].tick),
		Flow::connect(toggleLed[0].out, led0->inValue),
		Flow::connect(tickSplit->out[1], toggleLed[1].tick),
		Flow::connect(toggleLed[1].out, led1->inValue),
		Flow::connect(tickSplit->out[2], toggleLed[2].tick),
		Flow::connect(toggleLed[2].out, led2->inValue),

		Flow::connect(timerP->outTick, counterP->in),
		Flow::connect(counterP->out, convertP->inFrom),
		Flow::connect(convertP->outTo, pwmP->inDutyCycleOutput[0]),

		Flow::connect(debug->out, debug->in, 40)
	};

	// Define the deployment of the components.
	Flow::Component* mainComponents[] =
	{
		cdc,

		tickSplit,
		&toggleLed[0],
		&toggleLed[1],
		&toggleLed[2],
		led0,
		led1,
		led2,

		convertP,
		counterP,
		pwmP,

		debug
	};

	Flow::Component* sysTickComponents[] =
	{
		timer,
		timerP
	};

	_sysTickComponents = sysTickComponents;
	_sysTickComponentsCount = ArraySizeOf(sysTickComponents);

	// Run the application.
	while(true)
	{
		for(unsigned int c = 0; c < ArraySizeOf(mainComponents); c++)
		{
			mainComponents[c]->run();
		}
	}

	// Disconnect the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(connections); i++)
	{
		Flow::disconnect(connections[i]);
	}

	// Destruct the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(mainComponents); i++)
	{
		delete mainComponents[i];
	}

	_sysTickComponentsCount = 0;
	_sysTickComponents = nullptr;

	for(unsigned int i = 0; i < ArraySizeOf(sysTickComponents); i++)
	{
		delete sysTickComponents[i];
	}
}

// SysTick related stuff.
extern "C" void SysTickIntHandler(void)
{
	for(unsigned int c = 0; c < _sysTickComponentsCount; c++)
	{
		_sysTickComponents[c]->run();
	}
}

// An assert will end up here.
extern "C" void __error__(const char *pcFilename, uint32_t ui32Line)
{
	printf("File: %s\r\nLine: %lu\r\n", pcFilename, ui32Line);
	__asm__ __volatile__("bkpt");
	while(true);
}
