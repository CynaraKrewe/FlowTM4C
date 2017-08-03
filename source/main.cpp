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
#include <string>
#include <string.h>

#include "driverlib/debug.h"

#include "pinmux/pinout.h"

#include "flow/components.h"
#include "flow/utility.h"

#include "flow/tm4c/usb_cdc.h"
#include "flow/tm4c/clock.h"
#include "flow/tm4c/pwm.h"
#include "flow/tm4c/uart.h"
#include "flow/tm4c/adc.h"
#include "flow/tm4c/gpio.h"

static Flow::Component** _sysTickComponents = nullptr;

static unsigned int _sysTickComponentsCount = 0;

class ConvertTemperature: public Flow::Component
{
public:
	Flow::InPort<uint32_t> in;
	Flow::OutPort<float> out;

	void run()
	{
		uint32_t temperatureCounts;

		if (in.receive(temperatureCounts))
		{

			float temperatureCelcius = (147.5f - ( ( (75*3.3f) * temperatureCounts) / 4096 ));
			out.send(temperatureCelcius);

		}
	}
};


class ConvertVoltage: public Flow::Component
{
public:
	Flow::InPort<uint32_t> in;
	Flow::OutPort<float> out;

	void run()
	{
		uint32_t voltageCounts;

		if (in.receive(voltageCounts))
		{

			float adcVoltage = ( voltageCounts / 4096.0f ) * 3.3f;
			out.send(adcVoltage);

		}
	}
};



class DrawFrame: public Flow::Component
{
public:
	Flow::InPort<Tick> tick;

	Flow::InPort<float> inADC9;
	Flow::InPort<float> inADC0;
	Flow::InPort<float> inADC1;
	Flow::InPort<float> inADC2;
	Flow::InPort<float> inADC3;
	Flow::InPort<float> inCpuTemp;

	Flow::OutPort<char> out;

	void run()
	{

		char TerminalBuffer[400];
		Tick t;


		float varADC9;
		float varADC0;
		float varADC1;
		float varADC2;
		float varADC3;
		float varCpuTemp;


		if(tick.receive(t))
		{
		sprintf(&TerminalBuffer[0], "\f#### Sample nummer: %8u\n\r", samplenr);

		uint8_t length = strlen(TerminalBuffer);

		if (inADC9.receive(varADC9))
		{
			sprintf(&TerminalBuffer[length], "#  V AIN9: %0.3f V\n\r", varADC9);
		}

		length = strlen(TerminalBuffer);

		if (inADC0.receive(varADC0))
		{
			sprintf(&TerminalBuffer[length], "#  V AIN0: %0.3f V\n\r", varADC0);
		}

		length = strlen(TerminalBuffer);

		if (inADC1.receive(varADC1))
		{
			sprintf(&TerminalBuffer[length], "#  V AIN1: %0.3f V\n\r", varADC1);
		}

		length = strlen(TerminalBuffer);

		if (inADC2.receive(varADC2))
		{
			sprintf(&TerminalBuffer[length], "#  V AIN2: %0.3f V\n\r", varADC2);
		}

		length = strlen(TerminalBuffer);

		if (inADC3.receive(varADC3))
		{
			sprintf(&TerminalBuffer[length], "#  V AIN3: %0.3f V\n\r", varADC3);
		}

		length = strlen(TerminalBuffer);

		if (inCpuTemp.receive(varCpuTemp))
		{
			sprintf(&TerminalBuffer[length], "#  T CPU_CORE: %0.3f *C\n\r", varCpuTemp);
		}
		samplenr++;

		for (int i = 0; i < strlen(TerminalBuffer); i++)
		{
			out.send(TerminalBuffer[i]);
		}
		}


	}
private:
	uint32_t samplenr = 0;
};



int main(void)
{
	// Set up the clock circuit.
	Clock::instance()->configure(120 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	ConvertTemperature* convertTemperature = new ConvertTemperature();
	ConvertVoltage* convertVoltage0 = new ConvertVoltage();
	ConvertVoltage* convertVoltage1 = new ConvertVoltage();
	ConvertVoltage* convertVoltage2 = new ConvertVoltage();
	ConvertVoltage* convertVoltage3 = new ConvertVoltage();
	ConvertVoltage* convertVoltage4 = new ConvertVoltage();

	DrawFrame* drawFrame = new DrawFrame();

	Adc* adc1 = new Adc();

	Toggle* toggleLed = new Toggle[3]();

	UsbCdc* cdc = new UsbCdc();

	Timer* timerP = new Timer(20 /*ms*/);
	UpDownCounter<Tick>* counterP = new UpDownCounter<Tick>(0, 50, 0);
	Convert<unsigned int, float>* convertP = new Convert<unsigned int, float>();
	Frequency pwmFrequency[4] =	{ 1 kHz, 0, 0, 0 };

	Pwm* pwmP = new Pwm(Pwm::Divider::_64, pwmFrequency);

	Uart* uart0 = new Uart(Uart::Number::_0, Uart::Baudrate::_115200);

	Split<Tick, 4>* splitTick = new Split<Tick, 4>();

	DigitalOutput* led0 = new DigitalOutput(Gpio::Name{ Gpio::Port::N, 1 });
	DigitalOutput* led1 = new DigitalOutput(Gpio::Name{ Gpio::Port::N, 0 });
	DigitalOutput* led2 = new DigitalOutput(Gpio::Name{ Gpio::Port::F, 4 });
	DigitalOutput* led3 = new DigitalOutput(Gpio::Name{ Gpio::Port::F, 0 });

	Timer* timer1 = new Timer(100 /*ms*/);

// Connect the components of the application.
	Flow::Connection* connections[] =

	{
		Flow::connect(cdc->out, cdc->in, 40),

		Flow::connect(timer1->outTick,splitTick->in),
		Flow::connect(splitTick->out[0], toggleLed[0].tick),
		Flow::connect(toggleLed[0].out, led0->inValue),
		Flow::connect(splitTick->out[1], toggleLed[1].tick),
		Flow::connect(toggleLed[1].out, led1->inValue),
		Flow::connect(splitTick->out[2], toggleLed[2].tick),
		Flow::connect(toggleLed[2].out, led2->inValue),

		Flow::connect(timerP->outTick, counterP->in),
		Flow::connect(counterP->out, convertP->inFrom),
		Flow::connect(convertP->outTo, pwmP->inDutyCycleOutput[0]),

		Flow::connect(adc1->sampleCpuTemperature, convertTemperature->in),
		Flow::connect(adc1->sampleAin9, convertVoltage0->in),
		Flow::connect(adc1->sampleAin0, convertVoltage1->in),
		Flow::connect(adc1->sampleAin1, convertVoltage2->in),
		Flow::connect(adc1->sampleAin2, convertVoltage3->in),
		Flow::connect(adc1->sampleAin3, convertVoltage4->in),

		Flow::connect(convertTemperature->out,drawFrame->inCpuTemp),
		Flow::connect(convertVoltage0->out,drawFrame->inADC9),
		Flow::connect(convertVoltage1->out,drawFrame->inADC0),
		Flow::connect(convertVoltage2->out,drawFrame->inADC1),
		Flow::connect(convertVoltage3->out,drawFrame->inADC2),
		Flow::connect(convertVoltage4->out,drawFrame->inADC3),

		Flow::connect(splitTick->out[3],drawFrame->tick),
		Flow::connect(drawFrame->out, uart0->in, 400),

	};



	// Define the deployment of the components.
	Flow::Component* mainComponents[] =

	{
		splitTick,
		led0,
		led1,
		led2,
		led3,
		&toggleLed[0],
		&toggleLed[1],
		&toggleLed[2],
		uart0,
		cdc,
		convertP,
		counterP,
		pwmP,
		adc1,
		convertTemperature,
		convertVoltage0,
		convertVoltage1,
		convertVoltage2,
		convertVoltage3,
		convertVoltage4,
		drawFrame
	};

	Flow::Component* sysTickComponents[] =
	{ timer1, timerP };


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


//////
// The following code can be used an example
//////

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
	__asm__ __volatile__("bkpt");
	while(true);
}

