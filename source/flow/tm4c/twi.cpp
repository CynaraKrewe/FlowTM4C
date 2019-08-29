/**
 * \file
 * \copyright
 * Copyright (c) 2019 Onera B.V. - All Rights Reserved.
 * See the included documentation/02_License.md for details.
 */

#include <assert.h>

#include "flow/tm4c/clock.h"
#include "flow/tm4c/dma.h"
#include "flow/tm4c/twi.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"

#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"


Flow::TM4C::TWI::TWI(Number twiNumber) :
		_number(twiNumber)
{
	assert(_number < Number::COUNT);

	while(!SysCtlPeripheralReady(peripheral()))
	{
		SysCtlPeripheralEnable(peripheral());
	}
}

Flow::TM4C::TWI::~TWI()
{
	stop();
}

void Flow::TM4C::TWI::start()
{
	Frequency coreFrequency = Clock::instance().getFrequency();

	I2CMasterInitExpClk(base(), coreFrequency, false);

	I2CRxFIFOFlush(base());
	I2CTxFIFOFlush(base());

	I2CRxFIFOConfigSet(base(), I2C_FIFO_CFG_RX_MASTER_DMA | I2C_FIFO_CFG_RX_TRIG_1);
	I2CTxFIFOConfigSet(base(), I2C_FIFO_CFG_TX_MASTER_DMA | I2C_FIFO_CFG_TX_TRIG_1);

	DMA::peripheral().assign(dmaReceiveChannelAssign());
	DMA::peripheral().assign(dmaTransmitChannelAssign());

	I2CMasterIntEnableEx(base(), (I2C_MASTER_INT_STOP | I2C_MASTER_INT_NACK));
	IntEnable(vector());

	state = State::IDLE;
}

void Flow::TM4C::TWI::stop()
{
	state = State::INIT;
}

void Flow::TM4C::TWI::trigger()
{
	IntTrigger(vector());
}

void Flow::TM4C::TWI::isr()
{
	uint32_t status = I2CMasterIntStatusEx(base(), true);
	I2CMasterIntClearEx(base(), status);


	if(status & I2C_MASTER_INT_NACK)
	{
		state = State::NACK;
	}
	else if(status & I2C_MASTER_INT_STOP)
	{
		state = State::IDLE;
	}
}

bool Flow::TM4C::TWI::transceive(uint8_t address,
        Flow::Driver::TWI::Direction direction, uint8_t length, uint8_t* data)
{
	assert(state == State::IDLE);

    uint32_t controlDMA = 0;
    uint32_t channel = DMA::NONE;

    switch(direction)
    {
    	case Flow::Driver::TWI::Direction::TRANSMIT:
    	{
    		controlDMA = (UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4);
    		channel = dmaTransmitChannel();
    	} break;
    	case Flow::Driver::TWI::Direction::RECEIVE:
    	{
    		controlDMA = (UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_4);
    		channel = dmaReceiveChannel();
    	} break;
    }

	I2CMasterSlaveAddrSet(base(), address, (direction == Flow::Driver::TWI::Direction::RECEIVE));
	I2CMasterBurstLengthSet(base(), length);

    uDMAChannelControlSet(
			(channel | UDMA_PRI_SELECT), controlDMA);
	uDMAChannelTransferSet(
			(channel | UDMA_PRI_SELECT), UDMA_MODE_BASIC,
			(void*)data, (void*)(base() + I2C_O_FIFODATA), length);
	uDMAChannelEnable(channel);

	I2CMasterControl(base(), I2C_MASTER_CMD_FIFO_SINGLE_SEND);

	state = State::BUSY;

	return true;
}

Flow::Driver::TWI::Peripheral::State Flow::TM4C::TWI::status() const
{
	return state;
}

void Flow::TM4C::TWI::unblock()
{
}

Flow::TM4C::TWI::Number Flow::TM4C::TWI::number() const
{
	return _number;
}

uint8_t Flow::TM4C::TWI::vector() const
{
	return _vector[(uint8_t)number()];
}

uint32_t Flow::TM4C::TWI::peripheral() const
{
    return _peripheral[(uint8_t)number()];
}

uint32_t Flow::TM4C::TWI::base() const
{
    return _base[(uint8_t)number()];
}
uint32_t Flow::TM4C::TWI::dmaTransmitChannel() const
{
    return _dmaTransmitChannel[(uint8_t)number()];
}

uint32_t Flow::TM4C::TWI::dmaReceiveChannel() const
{
    return _dmaReceiveChannel[(uint8_t)number()];
}

uint32_t Flow::TM4C::TWI::dmaTransmitChannelAssign() const
{
    return _dmaTransmitChannelAssign[(uint8_t)number()];
}

uint32_t Flow::TM4C::TWI::dmaReceiveChannelAssign() const
{
    return _dmaReceiveChannelAssign[(uint8_t)number()];
}

const uint8_t Flow::TM4C::TWI::_vector[] =
{
	INT_I2C0,
	INT_I2C1,
	INT_I2C2,
	INT_I2C3,
	INT_I2C4,
	INT_I2C5
};

const uint32_t Flow::TM4C::TWI::_peripheral[] =
{
    SYSCTL_PERIPH_I2C0,
    SYSCTL_PERIPH_I2C1,
    SYSCTL_PERIPH_I2C2,
    SYSCTL_PERIPH_I2C3,
    SYSCTL_PERIPH_I2C4,
    SYSCTL_PERIPH_I2C5
};

const uint32_t Flow::TM4C::TWI::_base[] =
{
	I2C0_BASE,
    I2C1_BASE,
    I2C2_BASE,
    I2C3_BASE,
    I2C4_BASE,
    I2C5_BASE
};

const uint32_t Flow::TM4C::TWI::_dmaTransmitChannel[] =
{
    1,
    3,
    5,
    19,
    21,
    23
};

const uint32_t Flow::TM4C::TWI::_dmaReceiveChannel[] =
{
    0,
    2,
    4,
    18,
    20,
    22
};

const uint32_t Flow::TM4C::TWI::_dmaTransmitChannelAssign[] =
{
    UDMA_CH1_I2C0TX,
    UDMA_CH3_I2C1TX,
    UDMA_CH5_I2C2TX,
    UDMA_CH19_I2C3TX,
    UDMA_CH21_I2C4TX,
    UDMA_CH23_I2C5TX
};

const uint32_t Flow::TM4C::TWI::_dmaReceiveChannelAssign[] =
{
    UDMA_CH0_I2C0RX,
    UDMA_CH2_I2C1RX,
    UDMA_CH4_I2C2RX,
    UDMA_CH18_I2C3RX,
    UDMA_CH20_I2C4RX,
    UDMA_CH22_I2C5RX
};
