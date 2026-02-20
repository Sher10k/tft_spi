/**
  ******************************************************************************

  ANT21 LIBRARY for STM32 using I2C
  Author:   Roman Sukhanov
  Updated:  20/02/2026

  ******************************************************************************
  Copyright (C) 2026 roma2010pk@gmail.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ******************************************************************************
*/

#ifndef _ANT21_H_
#define _ANT21_H_


#include "stm32f1xx_hal.h"

HAL_StatusTypeDef AHT21_init(void);

uint32_t AHT21_Read_Humidity(void);

int32_t AHT21_Read_Temperature(void);

#endif /* INC_ANT21_H_ */
