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

#ifndef FLOW_TM4C_TWI_H_
#define FLOW_TM4C_TWI_H_

#include "flow/driver/twi.h"

namespace Flow {
namespace TM4C {

class TWI :
        public Flow::Driver::TWI::Peripheral
{
public:
	enum class Number :
			unsigned int
	{
		_0 = 0,
		_1 = 1,
		_2 = 2,
		_3 = 3,
		_4 = 4,
		_5 = 5,
		COUNT
	};

	TWI(Number twiNumber);
	~TWI();

	void start() final override;
	void stop() final override;

	void trigger() final override;
	void isr() final override;

	bool transceive(uint8_t address, Flow::Driver::TWI::Direction direction, uint8_t length,
	        uint8_t* data) final override;

	State status() const final override;

	void unblock();

private:
	Number number() const;
	uint8_t vector() const;
	uint32_t peripheral() const;
	uint32_t base() const;
	uint32_t dmaTransmitChannel() const;
	uint32_t dmaReceiveChannel() const;
	uint32_t dmaTransmitChannelAssign() const;
	uint32_t dmaReceiveChannelAssign() const;

	const Number _number;
    static const uint8_t _vector[(unsigned int)Number::COUNT];
    static const uint32_t _peripheral[(unsigned int)Number::COUNT];
    static const uint32_t _base[(unsigned int)Number::COUNT];
    static const uint32_t _dmaTransmitChannel[(unsigned int)Number::COUNT];
    static const uint32_t _dmaReceiveChannel[(unsigned int)Number::COUNT];
    static const uint32_t _dmaTransmitChannelAssign[(unsigned int)Number::COUNT];
    static const uint32_t _dmaReceiveChannelAssign[(unsigned int)Number::COUNT];
};

} // namespace TM4C
} // namespace Flow

#endif /* FLOW_TM4C_TWI_H_ */
