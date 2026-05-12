/*
 * adxl345.c
 *
 *  Created on: Apr 9, 2026
 *      Author: Leslie
 */

#include "adxl345.h"

void ADXL345_Init(I2C_HandleTypeDef *hi2c)
{
	uint8_t power_cmd = (1<<3);
	HAL_I2C_Mem_Write(hi2c, ADXL_ADDR, ADXL_REG_POWER_CTL, 1, &power_cmd, 1, 100);
}

HAL_StatusTypeDef ADXL345_Read_Raw(I2C_HandleTypeDef *hi2c, int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t raw_data[6];
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, ADXL_ADDR, ADXL_REG_DATAX0, 1, raw_data, 6, 100);

	if(status == HAL_OK)
	{
		*x = (int16_t)((raw_data[1]<<8) | raw_data[0]);
		*y = (int16_t)((raw_data[3]<<8) | raw_data[2]);
		*z = (int16_t)((raw_data[5]<<8) | raw_data[4]);
	}
	return status;
}
