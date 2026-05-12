#ifndef __ADXL345_H__
#define __ADXL345_H__

#include "stm32f4xx_hal.h"

#define ADXL_ADDR 				(0x53<<1)
#define ADXL_REG_ID				0x00
#define ADXL_REG_POWER_CTL		0x2D
#define ADXL_REG_DATA_FORMAT	0x31
#define ADXL_REG_DATAX0			0x32

void ADXL345_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef ADXL345_Read_Raw(I2C_HandleTypeDef *hi2c, int16_t *x, int16_t *y, int16_t *z);

#endif
