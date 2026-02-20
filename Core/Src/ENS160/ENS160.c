/**
  ******************************************************************************

  ENS160 LIBRARY for STM32 using I2C
  Author:   Roman Sukhanov
  Updated:  20/02/2026

  ******************************************************************************
  Copyright (C) 2026 roma2010pk@gmail.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  Source code: https://github.com/rRaufToprak/ENS160-Driver

  ******************************************************************************
*/



#include "ENS160.h"

extern I2C_HandleTypeDef hi2c1;
#define ENS160_I2C &hi2c1

#define ENS160_ADDRESS 0xA6  // = (0x53 << 1)

bool ENS160_Control_ID()
{
  bool id_status = false;

  uint8_t buffer[2];
  uint16_t sensorID = 0;

  HAL_I2C_Mem_Read(ENS160_I2C, ENS160_ADDRESS, ENS160_PART_ID, I2C_MEMADD_SIZE_8BIT, buffer, 2, 1000);
  sensorID = ((uint16_t)buffer[1]<<8)| buffer[0];
  
  id_status = (sensorID == 0x160) ? true : false;
  
  return id_status;
}

bool ENS160_Write_Register(uint8_t register_address, uint8_t data)
{
  HAL_StatusTypeDef status;

  status = HAL_I2C_Mem_Write(ENS160_I2C, ENS160_ADDRESS, register_address, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);

  if(status == HAL_OK)
    return true;
  else
    return false;
}

uint8_t ENS160_Read_Single_Register(uint8_t register_address)
{
  HAL_StatusTypeDef status;
  uint8_t value;
  
  status = HAL_I2C_Mem_Read(ENS160_I2C, ENS160_ADDRESS, register_address, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);
  
  if(status == HAL_OK)
    return value;
  else
    return 0;
}

uint16_t ENS160_Read_Register(uint8_t register_address)
{
  uint8_t data[2];
  
  HAL_I2C_Mem_Read(ENS160_I2C, ENS160_ADDRESS, register_address, I2C_MEMADD_SIZE_8BIT, data, 2, 1000);
  
  uint16_t raw_value = ((data[1]<<8)| data[0]);

  return raw_value;
}

bool ENS160_SetOpMode(uint8_t command)
{
  HAL_StatusTypeDef status;
  
  status = HAL_I2C_Mem_Write(ENS160_I2C, ENS160_ADDRESS, ENS160_OPMODE, I2C_MEMADD_SIZE_8BIT, &command, 1, 1000);

  if(status == HAL_OK)
    return true;
  else
    return false;
}

void ENS160_Read_Datas(SensorData *data)
{
  data->AQI_data  = ENS160_Read_Single_Register(ENS160_DATA_AQI);
  data->ECO2_data = ENS160_Read_Register(ENS160_DATA_ECO2); 
  data->TVOC_data = ENS160_Read_Register(ENS160_DATA_TVOC);
}

void ENS160_Update_Comp_Val(uint16_t temp, uint16_t hum)
{
  uint8_t data_buffer[2];
  temp = (temp + 273.15f) * 64;
  hum = hum * 512;
  
  data_buffer[0] = (uint8_t)temp & 0xFF;
  data_buffer[1] = ((uint16_t)temp >> 8 & 0xFF);
  HAL_I2C_Mem_Write(ENS160_I2C, ENS160_ADDRESS, ENS160_TEMP_IN, I2C_MEMADD_SIZE_8BIT, data_buffer, 2, 1000);
  
  data_buffer[0] = (uint8_t)hum & 0xFF;
  data_buffer[1] = ((uint16_t)hum >> 8 & 0xFF);

  HAL_I2C_Mem_Write(ENS160_I2C, ENS160_ADDRESS, ENS160_RH_IN, I2C_MEMADD_SIZE_8BIT, data_buffer, 2, 1000);
}
