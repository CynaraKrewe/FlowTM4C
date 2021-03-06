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

#include <algorithm>
#include <assert.h>

#include "flow/utility.h"
#include "flow/tm4c/clock.h"
#include "flow/tm4c/dma.h"
#include "flow/tm4c/ssi.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"

#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"

namespace Flow {
namespace TM4C {
namespace SSI {

Base::Base(Number number):
				_number(number)
{
	assert(_number < Number::COUNT);
}

Base::~Base()
{
}

void Base::trigger()
{
    IntTrigger(vector());
}

Base::Number Base::number() const
{
	return _number;
}

uint8_t Base::vector() const
{
    return _vector[(uint8_t)number()];
}

uint32_t Base::peripheral() const
{
    return _peripheral[(uint8_t)number()];
}

uint32_t Base::base() const
{
    return _base[(uint8_t)number()];
}
uint32_t Base::dmaTransmitChannel() const
{
    return _dmaTransmitChannel[(uint8_t)number()];
}

uint32_t Base::dmaReceiveChannel() const
{
    return _dmaReceiveChannel[(uint8_t)number()];
}

uint32_t Base::dmaTransmitChannelAssign() const
{
    return _dmaTransmitChannelAssign[(uint8_t)number()];
}

uint32_t Base::dmaReceiveChannelAssign() const
{
    return _dmaReceiveChannelAssign[(uint8_t)number()];
}


const uint8_t Base::_vector[] =
{
    INT_SSI0,
    INT_SSI1,
    INT_SSI2,
    INT_SSI3
};

const uint32_t Base::_peripheral[] =
{
    SYSCTL_PERIPH_SSI0,
    SYSCTL_PERIPH_SSI1,
    SYSCTL_PERIPH_SSI2,
    SYSCTL_PERIPH_SSI3
};

const uint32_t Base::_base[] =
{
    SSI0_BASE,
    SSI1_BASE,
    SSI2_BASE,
    SSI3_BASE
};

const uint32_t Base::_dmaTransmitChannel[] =
{
    11,
    25,
    13,
    15
};

const uint32_t Base::_dmaReceiveChannel[] =
{
    10,
    24,
    12,
    14
};

const uint32_t Base::_dmaTransmitChannelAssign[] =
{
    UDMA_CH11_SSI0TX,
    UDMA_CH25_SSI1TX,
    UDMA_CH13_SSI2TX,
    UDMA_CH15_SSI3TX
};

const uint32_t Base::_dmaReceiveChannelAssign[] =
{
    UDMA_CH10_SSI0RX,
    UDMA_CH24_SSI1RX,
    UDMA_CH12_SSI2RX,
    UDMA_CH14_SSI3RX
};

Master::Master(Number ssiNumber):
		Base(ssiNumber)
{
    while(!SysCtlPeripheralReady(peripheral()))
    {
        SysCtlPeripheralEnable(peripheral());
    }
}

Master::~Master()
{
    SysCtlPeripheralDisable(peripheral());
}

void Master::start()
{
    Frequency coreFrequency = Clock::instance().getFrequency();
    SSIConfigSetExpClk(base(), coreFrequency, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 2000000, 8);

    SSIEnable(base());
//    SSIAdvFrameHoldEnable(base());
    SSIDMAEnable(base(), (SSI_DMA_TX | SSI_DMA_RX));

    DMA::peripheral().assign(dmaReceiveChannelAssign());
    DMA::peripheral().assign(dmaTransmitChannelAssign());

    uDMAChannelControlSet(
            (dmaTransmitChannel() | UDMA_PRI_SELECT),
            (UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4));

    SSIIntEnable(base(), (SSI_DMATX | SSI_TXEOT | SSI_DMARX));

    IntEnable(vector());

    state = State::IDLE;
}

void Master::stop()
{

}

bool Master::transceive(const uint8_t* const transmit, uint16_t transmitLength,
		uint8_t* const receive, uint16_t receiveLength)
{
    assert(transmitLength > 0 || receiveLength > 0);
    assert(((transmitLength > 0) ? (transmit != nullptr) : true));
    assert(((receiveLength > 0) ? (receive != nullptr) : true));

    state = State::BUSY;

    uint32_t dmaEnable = 0;

    uint16_t transceiveLength = std::max(transmitLength, receiveLength);

    uint32_t control = (UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_4);
    const void* receiveBuffer = nullptr;

    if(receiveLength == 0)
    {
    	receiveBuffer = &dummy;
    	control |= UDMA_DST_INC_NONE;
    }
    else
    {
    	receiveBuffer = receive;
    }

	uDMAChannelControlSet((dmaReceiveChannel() | UDMA_PRI_SELECT), control);
    uDMAChannelTransferSet(
            (dmaReceiveChannel() | UDMA_PRI_SELECT), UDMA_MODE_BASIC,
            (void*)(base() + SSI_O_DR), (void*)receiveBuffer, transceiveLength);

    uDMAChannelEnable(dmaReceiveChannel());
    dmaEnable |= SSI_DMA_RX;

    control = (UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4);
    const void* transmitBuffer = nullptr;

	if(transmitLength == 0)
    {
		transmitBuffer = &dummy;
		control |= UDMA_SRC_INC_NONE;
    }
	else
	{
		transmitBuffer = transmit;
	}

	uDMAChannelControlSet(
			(dmaTransmitChannel() | UDMA_PRI_SELECT), control);
	uDMAChannelTransferSet(
			(dmaTransmitChannel() | UDMA_PRI_SELECT), UDMA_MODE_BASIC,
			(void*)transmitBuffer, (void*)(base() + SSI_O_DR), transceiveLength);

	uDMAChannelEnable(dmaTransmitChannel());
	dmaEnable |= SSI_DMA_TX;

    SSIDMAEnable(base(), dmaEnable);

    return true;
}

void Master::isr()
{
    uint32_t interruptStatus = SSIIntStatus(base(), true);

    if(interruptStatus & SSI_DMATX)
    {
        uDMAChannelDisable(dmaTransmitChannel());
        SSIDMADisable(base(), SSI_DMA_TX);

        SSIIntClear(base(), SSI_DMATX);
    }

    if(interruptStatus & SSI_DMARX)
    {
        uDMAChannelDisable(dmaReceiveChannel());
        SSIDMADisable(base(), SSI_DMA_RX);

        SSIIntClear(base(), SSI_DMARX);
    }

    if(interruptStatus & SSI_TXEOT)
	{
        SSIIntClear(base(), SSI_TXEOT);

    	if(!uDMAChannelIsEnabled(dmaReceiveChannel())
    			&& !uDMAChannelIsEnabled(dmaTransmitChannel()))
		{
			state = State::IDLE;
		}
	}
}

} // namespace SSI
} // namespace TM4C
} // namespace Flow
