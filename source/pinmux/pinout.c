//*****************************************************************************
//
// Configure the device pins for different signals
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// This file was automatically generated on 25-4-2017 at 19:55:11
// by TI PinMux version 4.0.1483 
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "pinout.h"

//*****************************************************************************
//
//! \addtogroup pinout_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Configures the device pins for the customer specific usage.
//!
//! \return None.
//
//*****************************************************************************
void
PinoutSet(void)
{
    //
    // Enable Peripheral Clocks 
    //
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    //
    // Configure the GPIO Pin Mux for PF0
	// for M0PWM0
    //
	MAP_GPIOPinConfigure(GPIO_PF0_M0PWM0);
	MAP_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PJ0
	// for GPIO_PJ0
    //
	MAP_GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
	MAP_GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //
    // Configure the GPIO Pin Mux for PJ1
	// for GPIO_PJ1
    //
	MAP_GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_1);
	MAP_GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //
    // Configure the GPIO Pin Mux for PN1
	// for GPIO_PN1
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    //
    // Configure the GPIO Pin Mux for PN0
	// for GPIO_PN0
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PF4
	// for GPIO_PF4
    //
	MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
    //
    // Configure the GPIO Pin Mux for PA0
	// for U0RX
    //
	MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PA1
	// for U0TX
    //
	MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
	MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);
    //
    // Configure the GPIO Pin Mux for PL4
	// for EPI0S26
    //
	MAP_GPIOPinConfigure(GPIO_PL4_EPI0S26);
	GPIOPinTypeEPI(GPIO_PORTL_BASE, GPIO_PIN_4);
    //
    // Configure the GPIO Pin Mux for PK6
	// for EPI0S25
    //
	MAP_GPIOPinConfigure(GPIO_PK6_EPI0S25);
	GPIOPinTypeEPI(GPIO_PORTK_BASE, GPIO_PIN_6);
    //
    // Configure the GPIO Pin Mux for PL0
	// for EPI0S16
    //
	MAP_GPIOPinConfigure(GPIO_PL0_EPI0S16);
	GPIOPinTypeEPI(GPIO_PORTL_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PG0
	// for EPI0S11
    //
	MAP_GPIOPinConfigure(GPIO_PG0_EPI0S11);
	GPIOPinTypeEPI(GPIO_PORTG_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PA6
	// for EPI0S8
    //
	MAP_GPIOPinConfigure(GPIO_PA6_EPI0S8);
	GPIOPinTypeEPI(GPIO_PORTA_BASE, GPIO_PIN_6);
    //
    // Configure the GPIO Pin Mux for PK1
	// for EPI0S1
    //
	MAP_GPIOPinConfigure(GPIO_PK1_EPI0S1);
	GPIOPinTypeEPI(GPIO_PORTK_BASE, GPIO_PIN_1);
    //
    // Configure the GPIO Pin Mux for PP3
	// for EPI0S30
    //
	MAP_GPIOPinConfigure(GPIO_PP3_EPI0S30);
	GPIOPinTypeEPI(GPIO_PORTP_BASE, GPIO_PIN_3);
    //
    // Configure the GPIO Pin Mux for PB2
	// for EPI0S27
    //
	MAP_GPIOPinConfigure(GPIO_PB2_EPI0S27);
	GPIOPinTypeEPI(GPIO_PORTB_BASE, GPIO_PIN_2);
    //
    // Configure the GPIO Pin Mux for PQ1
	// for EPI0S21
    //
	MAP_GPIOPinConfigure(GPIO_PQ1_EPI0S21);
	GPIOPinTypeEPI(GPIO_PORTQ_BASE, GPIO_PIN_1);
    //
    // Configure the GPIO Pin Mux for PL3
	// for EPI0S19
    //
	MAP_GPIOPinConfigure(GPIO_PL3_EPI0S19);
	GPIOPinTypeEPI(GPIO_PORTL_BASE, GPIO_PIN_3);
    //
    // Configure the GPIO Pin Mux for PM0
	// for EPI0S15
    //
	MAP_GPIOPinConfigure(GPIO_PM0_EPI0S15);
	GPIOPinTypeEPI(GPIO_PORTM_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PM2
	// for EPI0S13
    //
	MAP_GPIOPinConfigure(GPIO_PM2_EPI0S13);
	GPIOPinTypeEPI(GPIO_PORTM_BASE, GPIO_PIN_2);
    //
    // Configure the GPIO Pin Mux for PB3
	// for EPI0S28
    //
	MAP_GPIOPinConfigure(GPIO_PB3_EPI0S28);
	GPIOPinTypeEPI(GPIO_PORTB_BASE, GPIO_PIN_3);
    //
    // Configure the GPIO Pin Mux for PQ3
	// for EPI0S23
    //
	MAP_GPIOPinConfigure(GPIO_PQ3_EPI0S23);
	GPIOPinTypeEPI(GPIO_PORTQ_BASE, GPIO_PIN_3);
    //
    // Configure the GPIO Pin Mux for PG1
	// for EPI0S10
    //
	MAP_GPIOPinConfigure(GPIO_PG1_EPI0S10);
	GPIOPinTypeEPI(GPIO_PORTG_BASE, GPIO_PIN_1);
    //
    // Configure the GPIO Pin Mux for PK5
	// for EPI0S31
    //
	MAP_GPIOPinConfigure(GPIO_PK5_EPI0S31);
	GPIOPinTypeEPI(GPIO_PORTK_BASE, GPIO_PIN_5);
    //
    // Configure the GPIO Pin Mux for PP2
	// for EPI0S29
    //
	MAP_GPIOPinConfigure(GPIO_PP2_EPI0S29);
	GPIOPinTypeEPI(GPIO_PORTP_BASE, GPIO_PIN_2);
    //
    // Configure the GPIO Pin Mux for PK3
	// for EPI0S3
    //
	MAP_GPIOPinConfigure(GPIO_PK3_EPI0S3);
	GPIOPinTypeEPI(GPIO_PORTK_BASE, GPIO_PIN_3);
    //
    // Configure the GPIO Pin Mux for PC5
	// for EPI0S6
    //
	MAP_GPIOPinConfigure(GPIO_PC5_EPI0S6);
	GPIOPinTypeEPI(GPIO_PORTC_BASE, GPIO_PIN_5);
    //
    // Configure the GPIO Pin Mux for PA7
	// for EPI0S9
    //
	MAP_GPIOPinConfigure(GPIO_PA7_EPI0S9);
	GPIOPinTypeEPI(GPIO_PORTA_BASE, GPIO_PIN_7);
    //
    // Configure the GPIO Pin Mux for PK0
	// for EPI0S0
    //
	MAP_GPIOPinConfigure(GPIO_PK0_EPI0S0);
	GPIOPinTypeEPI(GPIO_PORTK_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PC4
	// for EPI0S7
    //
	MAP_GPIOPinConfigure(GPIO_PC4_EPI0S7);
	GPIOPinTypeEPI(GPIO_PORTC_BASE, GPIO_PIN_4);
    //
    // Configure the GPIO Pin Mux for PL1
	// for EPI0S17
    //
	MAP_GPIOPinConfigure(GPIO_PL1_EPI0S17);
	GPIOPinTypeEPI(GPIO_PORTL_BASE, GPIO_PIN_1);
    //
    // Configure the GPIO Pin Mux for PM1
	// for EPI0S14
    //
	MAP_GPIOPinConfigure(GPIO_PM1_EPI0S14);
	GPIOPinTypeEPI(GPIO_PORTM_BASE, GPIO_PIN_1);
    //
    // Configure the GPIO Pin Mux for PK7
	// for EPI0S24
    //
	MAP_GPIOPinConfigure(GPIO_PK7_EPI0S24);
	GPIOPinTypeEPI(GPIO_PORTK_BASE, GPIO_PIN_7);
    //
    // Configure the GPIO Pin Mux for PC7
	// for EPI0S4
    //
	MAP_GPIOPinConfigure(GPIO_PC7_EPI0S4);
	GPIOPinTypeEPI(GPIO_PORTC_BASE, GPIO_PIN_7);
    //
    // Configure the GPIO Pin Mux for PL2
	// for EPI0S18
    //
	MAP_GPIOPinConfigure(GPIO_PL2_EPI0S18);
	GPIOPinTypeEPI(GPIO_PORTL_BASE, GPIO_PIN_2);
    //
    // Configure the GPIO Pin Mux for PM3
	// for EPI0S12
    //
	MAP_GPIOPinConfigure(GPIO_PM3_EPI0S12);
	GPIOPinTypeEPI(GPIO_PORTM_BASE, GPIO_PIN_3);
    //
    // Configure the GPIO Pin Mux for PQ0
	// for EPI0S20
    //
	MAP_GPIOPinConfigure(GPIO_PQ0_EPI0S20);
	GPIOPinTypeEPI(GPIO_PORTQ_BASE, GPIO_PIN_0);
    //
    // Configure the GPIO Pin Mux for PQ2
	// for EPI0S22
    //
	MAP_GPIOPinConfigure(GPIO_PQ2_EPI0S22);
	GPIOPinTypeEPI(GPIO_PORTQ_BASE, GPIO_PIN_2);
    //
    // Configure the GPIO Pin Mux for PC6
	// for EPI0S5
    //
	MAP_GPIOPinConfigure(GPIO_PC6_EPI0S5);
	GPIOPinTypeEPI(GPIO_PORTC_BASE, GPIO_PIN_6);
    //
    // Configure the GPIO Pin Mux for PK2
	// for EPI0S2
    //
	MAP_GPIOPinConfigure(GPIO_PK2_EPI0S2);
	GPIOPinTypeEPI(GPIO_PORTK_BASE, GPIO_PIN_2);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

