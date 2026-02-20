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



#include "stm32f1xx_hal.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c1;
#define ANT21_I2C &hi2c1

#define ANT21_ADDRESS 0x70 // = (0x38 << 1)

uint32_t i2c_RETRY_TIME = 100;

HAL_StatusTypeDef AHT21_init(void) {
  HAL_StatusTypeDef ret;
  uint8_t buff[8];

  ret = HAL_I2C_IsDeviceReady(ANT21_I2C, ANT21_ADDRESS, 2,
      i2c_RETRY_TIME); // is device avaialble ?
  if (ret != HAL_OK) {
    return HAL_ERROR;
  }
  buff[0] = 0x71;
  ret = HAL_I2C_Master_Transmit(ANT21_I2C, ANT21_ADDRESS, buff, 1,
      i2c_RETRY_TIME);
  if (ret != HAL_OK) {
    return HAL_ERROR;
  }
  ret = HAL_I2C_Master_Receive(ANT21_I2C, ANT21_ADDRESS, buff, 1,
      i2c_RETRY_TIME);
  buff[0] = buff[0] & 0x18;
  if (buff[0] != 0x18) {
    return HAL_ERROR;
  }
  return ret;
}

uint32_t AHT21_Read_Humidity(void) {
  HAL_StatusTypeDef ret;
  uint8_t buff[8];
  uint32_t humidity;

  buff[0] = 0xAC;
  buff[1] = 0X33;
  buff[2] = 0x00;
  ret = HAL_I2C_Master_Transmit(ANT21_I2C, ANT21_ADDRESS, buff, 3,
      i2c_RETRY_TIME);
  if (ret != HAL_OK) {
    return HAL_ERROR;
  }
  HAL_Delay(100);
  ret = HAL_I2C_Master_Receive(ANT21_I2C, ANT21_ADDRESS, buff, 6,
      i2c_RETRY_TIME);
  //CALCULATING HUMIDITY
  humidity = (buff[1] << 12) | (buff[2] << 4) | (buff[3] >> 4);
  humidity = (humidity * 100);
  humidity = humidity / 0x100000;
  return humidity;
}

int32_t AHT21_Read_Temperature(void) {
  HAL_StatusTypeDef ret;
  uint8_t buff[8];
  uint32_t temperature;

  buff[0] = 0xAC;
  buff[1] = 0X33;
  buff[2] = 0x00;
  ret = HAL_I2C_Master_Transmit(ANT21_I2C, ANT21_ADDRESS, buff, 3,
      i2c_RETRY_TIME);
  if (ret != HAL_OK) {
    return HAL_ERROR;
  }
  HAL_Delay(100);
  ret = HAL_I2C_Master_Receive(ANT21_I2C, ANT21_ADDRESS, buff, 6,
      i2c_RETRY_TIME);
  //CALCULATING TEMPERATURE
  temperature = ((buff[3] & 0xF) << 16) | (buff[4] << 8) | (buff[5]);
  temperature = (temperature * 200);
  temperature = temperature / 0x100000;
  temperature = temperature - 50;
  return temperature;
}
