/*
 * Copyright (C)2019 Kai Ludwig, DG4KLU
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#define BSS_LOWER_BOTTOM  __attribute__((section(".bss.$RAM2")))
#define BSS_LOWER_TOP     __attribute__((section(".bss.$RAM4")))
#define DATA_LOWER_TOP    __attribute__((section(".data.$RAM4")))

#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "virtual_com.h"
#include "usb_com.h"

#include "buttons.h"
#include "LEDs.h"
#include "keyboard.h"
#include "rotary_switch.h"
#include "display.h"
#include "vox.h"

#include "UC1701.h"

#include "i2c.h"
#include "hr-c6000_spi.h"
#include "i2s.h"
#include "AT1846S.h"
#include "HR-C6000.h"
#include "wdog.h"
#include "adc.h"
#include "dac.h"
#include "pit.h"

#include "sound.h"
#include "trx.h"
#include "SPI_Flash.h"
#include "EEPROM.h"


void mainTaskInit(void);
void powerOffFinalStage(void);

#endif /* _MAIN_H_ */
