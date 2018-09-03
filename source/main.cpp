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

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driverlib/debug.h"

#include "pinmux/pinout.h"

#include "flow/components.h"
#include "flow/pool.h"
#include "flow/reactor.h"
#include "flow/utility.h"

#include "flow/tm4c/adc.h"
#include "flow/tm4c/clock.h"
#include "flow/tm4c/digitalio.h"
#include "flow/tm4c/pwm.h"
#include "flow/tm4c/ssi.h"
#include "flow/tm4c/uart.h"
#include "flow/tm4c/usb_cdc.h"

using namespace Flow::TM4C;

class ConvertTemperature : public Flow::Component
{
public:
	Flow::InPort<uint32_t> in{this};
	Flow::OutPort<float> out;

    void start() final override{}
    void stop() final override{}

	void run()
	{
		uint32_t temperatureCounts;

		if(in.receive(temperatureCounts))
		{
			float temperatureCelcius = (147.5f - (((75.0f * 3.3f) * (float)temperatureCounts) / 4096.0f));
			out.send(temperatureCelcius);
		}
	}
};

class DrawFrame : public Flow::Component
{
public:
	Flow::InPort<float> inTemperature{this};

	Flow::OutPort<char> out;

    void start() final override{}
    void stop() final override{}

	void run()
	{
		char buffer[50] = { 0 };

        sprintf(&buffer[0], "\fSample # %u\r\n", sample++);

        uint8_t length = strlen(buffer);

        float temperature;
        if(inTemperature.receive(temperature))
        {
            sprintf(&buffer[length], "\tT_core = %0.3f *C\r\n", temperature);
        }

        for(unsigned int i = 0; i < strlen(buffer); i++)
        {
            out.send(buffer[i]);
        }
	}

private:
	unsigned int sample = 0;
};

Digital::Interrupt switch0{Pin::Port::J, 0, Digital::Interrupt::Trigger::Change, Digital::Polarity::Inverted};
Digital::Output led0{Pin::Port::N, 1, Digital::Polarity::Normal};

Digital::Interrupt switch1{Pin::Port::J, 1, Digital::Interrupt::Trigger::Change, Digital::Polarity::Inverted};
Digital::Output led1{Pin::Port::N, 0, Digital::Polarity::Normal};

Timer timerLedBlink{100 /*ms*/};
Toggle toggle;
Digital::Output led2{Pin::Port::F, 4, Digital::Polarity::Normal};

Timer timerP{20 /*ms*/};
UpDownCounter<Tick> counterP{0, 30, 0};
Convert<uint32_t, float> convertP;
Frequency pwmFrequency[4] = { 1 kHz, 0, 0, 0 };
Pwm pwmP{Pwm::Divider::_32, pwmFrequency};

ConvertTemperature convertTemperature;
ADC1 adc1;
Timer timerTemperature{1000 /*ms*/};
DrawFrame drawFrame;
UART uart0{UART::Number::_0, UART::Baudrate::_115200};

UsbCdc cdc;

int main(void)
{
	// Set up the clock circuit.
    Clock::instance().configure(120 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	// Connect the components of the application.
	Flow::connect(cdc.out, cdc.in, 40);

    Flow::connect(timerLedBlink.outTick, toggle.tick);
    Flow::connect(toggle.out, led2.inState);

    Flow::connect(switch0.out, led0.inState);
    Flow::connect(switch1.out, led1.inState);

    Flow::connect(timerP.outTick, counterP.in);
    Flow::connect(counterP.out, convertP.inFrom);
    Flow::connect(convertP.outTo, pwmP.inDutyCycleOutput[0]);

    Flow::connect(timerTemperature.outTick, adc1.inTrigger);
    Flow::connect(adc1.outTemperature, convertTemperature.in);
    Flow::connect(convertTemperature.out, drawFrame.inTemperature);
    Flow::connect(drawFrame.out, uart0.in, 50);

    Flow::Reactor::start();

	// Run the application.
	while(true)
	{
	    Flow::Reactor::run();
	}
}

// SysTick related stuff.
extern "C" void isrSysTick(void)
{
    timerLedBlink.isr();
    timerP.isr();
    timerTemperature.isr();
}

extern "C" void isrADC1Seq3(void)
{
    adc1.isr();
}

extern "C" void isrUART0(void)
{
    uart0.isr();
}

extern "C" void isrUSB0(void)
{
    cdc.isr();
}

extern "C" void isrGPIOJ(void)
{
    switch0.isr();
    switch1.isr();
}

// An assert will end up here.
extern "C" void __assert_func(const char* file, int line, const char* function, const char* expression)
{
    __asm__ __volatile__("bkpt");
    while(true);
}
