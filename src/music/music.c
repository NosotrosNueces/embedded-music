//*****************************************************************************
//
// timers.c - Timers example.
//
// Copyright (c) 2012-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "wave.h"
#include "dac.h"
#include "songs.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Timer (timers)</h1>
//!
//! This example application demonstrates the use of the timers to generate
//! periodic interrupts.  One timer is set up to interrupt once per second and
//! the other to interrupt twice per second; each interrupt handler will toggle
//! its own indicator on the display.
//!
//! UART0, connected to the Virtual Serial Port and running at 115,200, 8-N-1,
//! is used to display messages from this application.
//
//*****************************************************************************

//*****************************************************************************
//
// Flags that contain the current value of the interrupt indicator as displayed
// on the UART.
//
//*****************************************************************************
//
// 5 timers
// 3 for music
// 1 to update tick
// 1 to write to DAC

uint32_t tick;

uint16_t *line0;
uint16_t *line1;
uint16_t *line2;
uint16_t *line3;
uint16_t *line4;
uint16_t *line5;
uint16_t *line6;
uint16_t *line7;

uint32_t len;

uint32_t voices;
uint16_t val[8];

uint32_t index0;
uint32_t index1;
uint32_t index2;
uint32_t index3;
uint32_t index4;
uint32_t index5;
uint32_t index6;
uint32_t index7;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

void
Timer0HandlerA(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet() / line0[tick]);
    val[0] = (wave[index0] + wave[(index0 + PERIOD / 4) % PERIOD] + 
        wave[(index0 + PERIOD / 2) % PERIOD] + 
        wave[(index0 + 3 * PERIOD / 4) % PERIOD]) / 4;
    index0 = (index0 + 1) % PERIOD;
}

void
Timer0HandlerB(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
}

void
Timer1HandlerA(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}

void
Timer1HandlerB(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMB_TIMEOUT);
}

void
Timer2HandlerA(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
}

void
Timer2HandlerB(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMB_TIMEOUT);
}

void
Timer3HandlerA(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
}

void
Timer3HandlerB(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER3_BASE, TIMER_TIMB_TIMEOUT);
}

void
Timer4HandlerA(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
}

void
Timer4HandlerB(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER4_BASE, TIMER_TIMB_TIMEOUT);
}


// outputs to the DAC
void
Timer5HandlerA(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
    uint16_t superpos = 0;
    int i;
    for (i = 0; i < voices; i++) {
        superpos += val[i];
    }
    superpos /= voices;
    dac_out(superpos);
}

// Updates the beat of the song
void
Timer5HandlerB(void)
{
    //
    // Clear the timer interrupt.
    //
    ROM_TimerIntClear(TIMER5_BASE, TIMER_TIMB_TIMEOUT);
    tick = (tick + 1) % len;
}

//*****************************************************************************
//
// This example application demonstrates the use of the timers to generate
// periodic interrupts.
//
//*****************************************************************************
int
main(void)
{
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable the GPIO pins for the LED (PF1 & PF2).
    //
    //ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_1);


    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);

    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();

    //
    // Configure the two 32-bit periodic timers.
    //
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    //ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
    //ROM_TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
    //ROM_TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);


    //
    // Setup the interrupts for the timer timeouts.
    //
    ROM_IntEnable(INT_TIMER0A);
    //ROM_IntEnable(INT_TIMER0B);
    //ROM_IntEnable(INT_TIMER1A);
    //ROM_IntEnable(INT_TIMER1B);
    //ROM_IntEnable(INT_TIMER2A);
    //ROM_IntEnable(INT_TIMER2B);
    //ROM_IntEnable(INT_TIMER3A);
    //ROM_IntEnable(INT_TIMER3B);
    //ROM_IntEnable(INT_TIMER4A);
    //ROM_IntEnable(INT_TIMER4B);
    ROM_IntEnable(INT_TIMER5A);
    ROM_IntEnable(INT_TIMER5B);

    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    //ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    //ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMB_TIMEOUT);
    //ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    //ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMB_TIMEOUT);
    //ROM_TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    //ROM_TimerIntEnable(TIMER3_BASE, TIMER_TIMB_TIMEOUT);
    //ROM_TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
    //ROM_TimerIntEnable(TIMER4_BASE, TIMER_TIMB_TIMEOUT);
    ROM_TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerIntEnable(TIMER5_BASE, TIMER_TIMB_TIMEOUT);

    //
    // Enable the timers.
    //
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    //ROM_TimerEnable(TIMER0_BASE, TIMER_B);
    //ROM_TimerEnable(TIMER1_BASE, TIMER_A);
    //ROM_TimerEnable(TIMER1_BASE, TIMER_B);
    //ROM_TimerEnable(TIMER2_BASE, TIMER_A);
    //ROM_TimerEnable(TIMER2_BASE, TIMER_B);
    //ROM_TimerEnable(TIMER3_BASE, TIMER_A);
    //ROM_TimerEnable(TIMER3_BASE, TIMER_B);
    //ROM_TimerEnable(TIMER4_BASE, TIMER_A);
    //ROM_TimerEnable(TIMER4_BASE, TIMER_B);
    ROM_TimerEnable(TIMER5_BASE, TIMER_A);
    ROM_TimerEnable(TIMER5_BASE, TIMER_B);

    // Timer7B sets the tempo
    //
    // Sets the interval to be the 16th notes:
    // cycles/sec * (60 s/m) * (1 / 76 m / b) / 4
    ROM_TimerLoadSet(TIMER5_BASE, TIMER_B, ROM_SysCtlClockGet() * 60 / 76 / 4);
    ROM_TimerLoadSet(TIMER5_BASE, TIMER_B, ROM_SysCtlClockGet() / 8000);

    line0 = concert_a;
    len = concert_a_len;

    //
    // Loop forever while the timers run.
    //
    while (1) {
    }
}
