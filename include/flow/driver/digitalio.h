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

#ifndef FLOW_DRIVER_DIGITALIO_H_
#define FLOW_DRIVER_DIGITALIO_H_

#include <assert.h>
#include <stdint.h>

#include "flow/components.h"
#include "flow/driver/isr.h"

namespace Flow {
namespace Driver {

/**
 * \brief Digital i/o related classes.
 *
 * Contains target agnostic implementations and interfaces
 * to which target specific peripherals should adhere.
 */
namespace Digital {

/**
 * \brief Digital i/o polarity.
 */
enum class Polarity
{
    Normal, /**< Active means high and inactive means low.  */
    Inverted /**< Active means low and inactive means high.  */
};

/**
 * \brief Abstract representation of a digital i/o with configurable polarity.
 */
class GPIO
{
public:

	/**
	 * Create a digital i/o.
	 *
	 * \param polarity The polarity configuration.
	 */
	GPIO(Polarity polarity) :
			polarity(polarity)
	{
	}

	virtual ~GPIO()
	{
	}

protected:
	Polarity polarity;

	/**
	 * \brief Apply the polarity to a value.
	 *
	 * \param value The original value.
	 * \return The result from applying the polarity to the original value.
	 */
	bool applyPolarity(bool value)
	{
		bool active = false;

		switch(polarity)
		{
			case Polarity::Normal:
				active = value;
			break;
			case Polarity::Inverted:
				active = !value;
			break;
		}

		return active;
	}
};

/**
 * \brief Interface to be implemented by a target specific digital input.
 */
class Input: public GPIO
{
public:
	/**
	 * \brief Create a digital input.
	 *
	 * \param polarity The polarity configuration.
	 */
	Input(Polarity polarity) :
			GPIO(polarity)
	{
	}

	virtual ~Input()
	{
	}

	/**
	 * \brief Read the state of the digital input.
	 *
	 * Polarity is applied.
	 *
	 * \return The state of the digital input.
	 */
	virtual bool get() = 0;
};

/**
 * \brief Interface to be implemented by a target specific digital output.
 */
class Output: public Flow::Component, public GPIO
{
public:
	Flow::InPort<bool> inState{this};

	/**
	 * \brief Create a digital output.
	 *
	 * \param polarity The polarity configuration.
	 */
	Output(Polarity polarity) :
			GPIO(polarity)
	{
	}

    void start() final override
    {
    }

    void stop() final override
    {
    }

	void run() final override
	{
	    bool state;
        if(inState.receive(state))
        {
            set(state);
        }
	}

	/**
	 * \brief Set the state of the digital output.
	 *
	 * Polarity is applied.
	 *
	 * \param state The state of the digital output.
	 */
	virtual void set(bool state) = 0;
};

/**
 * \brief Interface to be implemented by a target specific
 * digital input with interrupt capability.
 */
class Interrupt: public Input, public Flow::Component, public Flow::Driver::WithISR
{
public:
	/**
	 * \brief Interrupt trigger configuration.
	 */
	enum class Trigger
		: uint8_t
		{
			Change,
			FallingEdge,
			RisingEdge,
			COUNT /**< DO NOT USE */
	};

	Flow::OutPort<bool> out;

	/**
	 * \brief Create a digital input with interrupt capability.
	 *
	 * \param trigger The interrupt trigger configuration.
	 */
	Interrupt(Trigger trigger, Polarity polarity) :
		Input(polarity),
		_trigger(trigger)
	{
		assert(_trigger < Trigger::COUNT);
	}

	virtual ~Interrupt()
	{
	}

protected:
	Trigger _trigger;
};

} // namespace Digital
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_DIGITALIO_H_ */
