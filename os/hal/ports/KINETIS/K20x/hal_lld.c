/*
    ChibiOS/RT - Copyright (C) 2006-2014 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    templates/hal_lld.c
 * @brief   HAL Driver subsystem low level driver source template.
 *
 * @addtogroup HAL
 * @{
 */

#include "ch.h"
#include "hal.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level HAL driver initialization.
 * @todo    Use a macro to define the system clock frequency.
 *
 * @notapi
 */
void hal_lld_init(void) {

}

/**
 * @brief   MK20D5 clock initialization.
 * @note    All the involved constants come from the file @p board.h.
 * @note    This function is meant to be invoked early during the system
 *          initialization, it is usually invoked from the file
 *          @p board.c.
 * @todo    This function needs to be more generic.
 *
 * @special
 */
void mk20d50_clock_init(void) {

  /* Disable the watchdog */
  WDOG->UNLOCK = 0xC520;
  WDOG->UNLOCK = 0xD928;
  WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN;

  SIM->SCGC5 |= SIM_SCGC5_PORTA |
                SIM_SCGC5_PORTB |
                SIM_SCGC5_PORTC |
                SIM_SCGC5_PORTD |
                SIM_SCGC5_PORTE;

  /* EXTAL0 and XTAL0 */
  PORTA->PCR[18] = 0;
  PORTA->PCR[19] = 0;

  /*
   * Start in FEI mode
   */

  /* Disable capacitors for crystal */
  OSC->CR = 0;

  /* Enable OSC, 8-32 MHz range, low power mode */
  MCG->C2 = MCG_C2_RANGE0(1) | MCG_C2_LOCRE0 | MCG_C2_EREFS0;

  /* Switch to crystal as clock source, FLL input of 8 MHz / 256 = 31.25 KHz */
  MCG->C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);

  /* Wait for crystal oscillator to begin */
  while (!(MCG->S & MCG_S_OSCINIT0));

  /* Wait for the FLL to use the oscillator */
  while (MCG->S & MCG_S_IREFST);

  /* Wait for the MCGOUTCLK to use the oscillator */
  while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2));

  /*
   * Now in FBE mode
   */

  /* Config PLL input for 2 MHz (8 MHz crystal / 4) */
  MCG->C5 = MCG_C5_PRDIV0(3);

  /* Config PLL for 96 MHz output */
  MCG->C6 = MCG_C6_PLLS | MCG_C6_VDIV0(0);

  /* Wait for PLL to start using crystal as its input */
  while (!(MCG->S & MCG_S_PLLST));

  /* Wait for PLL to lock */
  while (!(MCG->S & MCG_S_LOCK0));

  /*
   * Now in PBE mode
   */

  /* Switch to PLL as clock source */
  MCG->C1 = MCG_C1_CLKS(0);

  /* Wait for PLL clock to be used */
  while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL);

  /*
   * Now in PEE mode
   */
}

/** @} */
