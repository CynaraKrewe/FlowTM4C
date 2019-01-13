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

#ifndef FLOW_DRIVER_SSI_H_
#define FLOW_DRIVER_SSI_H_

#include "flow/flow.h"

#include "flow/driver/digitalio.h"
#include "flow/driver/isr.h"

namespace Flow {
namespace Driver {

/**
 * \brief Name space for all SSI related classes.
 *
 * This contains target agnostic implementations and interfaces
 * to which target specific peripherals should adhere.
 */
namespace SSI {
namespace Master {

/**
 * \brief Interface to be implemented by a target specific SSI peripheral.
 *
 * The SSI::Bus interacts through this interface with
 * a target specific SSI peripheral.
 */
class Peripheral:
		virtual public Flow::Driver::WithISR
{
public:
	/**
	 * \brief The state of the peripheral.
	 */
	enum class State
	{
		INIT, BUSY, IDLE
	};

	virtual ~Peripheral()
	{
	}

	/**
	 * \brief Configure and enable the peripheral.
	 */
	virtual void start() = 0;

	/**
	 * \brief Disable the peripheral.
	 */
	virtual void stop() = 0;

	/**
	 * \brief Perform a full duplex SSI operation.
	 *
	 * \param transmit The buffer to be transmitted.
	 * \param transmitLength The lenght of the buffer to be transmitted.
	 * \param receive The buffer to store received data in.
	 * \param receiveLength The length of the buffer to store received data in.
	 *
	 * \return Operation request was successful.
	 */
	virtual bool transceive(const uint8_t* const transmit,
	        uint16_t transmitLength, uint8_t* const receive,
	        uint16_t receiveLength) = 0;

	/**
	 * \brief Get the peripheral status.
	 *
	 * \return Peripheral status.
	 */
	virtual State status() const
	{
		return state;
	}

protected:
	State state = State::INIT;
};

/**
 * \brief An SSI operation.
 *
 * This implementation is target agnostic.
 */
class Operation
{
public:
	/**
	 * \brief Create a SSI operation.
	 *
	 * \param slaveSelect The digital output to be asserted during this operation.
	 * If the FSS signal from the SSI peripheral is used slaveSelect can be omitted.
	 */
	Operation(Flow::Driver::Digital::Output* slaveSelect = nullptr) :
			slaveSelect(slaveSelect)
	{
	}

	virtual ~Operation()
	{
	}

	/**
	 * \brief Operation status.
	 */
	enum class Status
	{
		TBD, FAIL, SUCCESS
	} status = Status::TBD;

	uint8_t* transmit = nullptr;
	uint16_t transmitLength = 0;
	uint8_t* receive = nullptr;
	uint16_t receiveLength = 0;

	uint32_t tag[4];

	Flow::Driver::Digital::Output* slaveSelect = nullptr;
};

/**
 * \brief SSI bus multiplexer/demultiplexer.
 *
 * This implementation is target agnostic.
 * It uses round robin scheduling if multiple slaves are connected.
 */
template<uint_fast8_t ENDPOINT_COUNT>
class Bus:
    public Flow::Component,
    public Flow::Driver::WithISR
{
public:
	/**
	 * \brief Create a SSI bus.
	 *
	 * \param peripheral The peripheral this bus is connected to.
	 */
	explicit Bus(Peripheral& peripheral) :
			peripheral(peripheral)
	{
		for(uint_fast8_t i = 0; i < ENDPOINT_COUNT; i++)
		{
			endPoint[i] = new Flow::InOutPort<Operation*>(this);
		}
	}

	/**
	 * \brief The bus end points.
	 *
	 * flow::connect() one or more SSI::Slave to these.
	 */
	Flow::InOutPort<Operation*>* endPoint[ENDPOINT_COUNT];

	/**
	 * \brief Start the SSI bus.
	 *
	 * The SSI peripheral handled by this bus will also be started.
	 */
	void start() final override
	{
		peripheral.start();
	}

	/**
	 * \brief Stop the SSI bus.
	 *
	 * The SSI peripheral handled by this bus will also be halted.
	 */
	void stop() final override
	{
		peripheral.stop();
	}

	/**
	 * \brief Let the bus perform its duty.
	 *
	 * The main behavior of the SSI bus & peripheral are running in interrupt context.
	 * If the SSI bus & peripheral are idle (all previous operations are completed,
	 * thus no interrupts will happen) then this will trigger the interrupt.
	 */
	void run() final override
	{
		if(currentOperation == nullptr)
		{
			trigger();
		}
	}

	/**
	 * \brief The interrupt service routine associated with the SSI bus.
	 *
	 * This is the main behavior of the SSI bus & peripheral and should run in interrupt context.
	 * It should be called in the associated interrupt handler.
	 */
	void isr() final override
	{
		peripheral.isr();

		switch(peripheral.status())
		{
			case Peripheral::State::IDLE:
			{
				if(currentOperation != nullptr)
				{
					if(currentOperation->status == Operation::Status::TBD)
					{
						currentOperation->status = Operation::Status::SUCCESS;
					}

					if(currentOperation->slaveSelect != nullptr)
					{
						currentOperation->slaveSelect->set(false);
					}

					endPoint[currentEndPoint]->send(currentOperation);
					currentOperation = nullptr;
				}

				if(++currentEndPoint >= ENDPOINT_COUNT)
				{
					currentEndPoint = 0;
				}

				// Search for end point with a request.
				while(currentEndPoint < ENDPOINT_COUNT)
				{
					if(endPoint[currentEndPoint]->receive(currentOperation))
					{
						if(currentOperation->slaveSelect != nullptr)
						{
							currentOperation->slaveSelect->set(true);
						}

						if(!peripheral.transceive(currentOperation->transmit,
						        currentOperation->transmitLength,
						        currentOperation->receive,
						        currentOperation->receiveLength))
						{
							currentOperation->status = Operation::Status::FAIL;
						}

						break;
					}
					else
					{
						currentEndPoint++;
					}
				}
			}
			break;
			case Peripheral::State::INIT:
			case Peripheral::State::BUSY:
			break;
		}
	}

private:
	Peripheral& peripheral;

	uint_fast8_t currentEndPoint = ENDPOINT_COUNT;
	Operation* currentOperation = nullptr;

	/**
	 * \brief Trigger the interrupt so the interrupt service routine will execute.
	 */
	void trigger() final override
	{
		peripheral.trigger();
	}
};

/**
 * \brief The interface drivers for SSI devices should adhere to
 * (on the master side implementation).
 */
class Slave:
    public Flow::Component
{
public:
	/**
	 * \brief Create a SSI slave.
	 *
	 * \param slaveSelect The slave select to be asserted during operations.
	 */
	explicit Slave(Flow::Driver::Digital::Output* slaveSelect = nullptr) :
			slaveSelect(slaveSelect)
	{
	}

	/**
	 * \brief The slave end point.
	 *
	 * flow::connect() this to one of the SSI::Bus end points.
	 */
	Flow::InOutPort<Operation*> endPoint
	{ this };

protected:
	Flow::Driver::Digital::Output* slaveSelect = nullptr;
};

} // namespace Master

namespace Slave
{

/**
 * \brief An SSI operation.
 *
 * This implementation is target agnostic.
 */
class Operation
{
public:
	Operation(const uint8_t* const transmit, uint8_t* const receive) :
			transmit(transmit),
			receive(receive)
	{}

	virtual ~Operation()
	{
	}

	/**
	 * \brief Operation status.
	 */
	enum class Status
	{
		TBD, FAIL, SUCCESS
	} status = Status::TBD;

	const uint8_t* const transmit;
	uint8_t transmitLength = 0;
	uint8_t* const receive;
	uint8_t receiveLength = 0;
};

class Peripheral:
    public Flow::Component,
    virtual public Flow::Driver::WithISR
{
public:
	Flow::InOutPort<Operation*> inOutOperation{this};

	virtual ~Peripheral()
	{
	}
};

} // namespace Slave
} // namespace SSI
} // namespace Driver
} // namespace Flow

#endif /* FLOW_DRIVER_SSI_H_ */
