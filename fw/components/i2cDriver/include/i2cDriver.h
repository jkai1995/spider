#ifndef I2CDRIVER_H
#define I2CDRIVER_H

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

esp_err_t I2C1_Init(uint32_t clk_speed);

esp_err_t IIC_ReadBytes(uint8_t slaveAddr,uint8_t *data, size_t len);

//slaveAddr 7-bit
esp_err_t IIC_WriteBytes(uint8_t slaveAddr,uint8_t *data, size_t len);

//slaveAddr 7-bit
esp_err_t IIC_ReadReg(uint8_t slaveAddr,uint8_t reg_addr,uint8_t *data, size_t len);


//slaveAddr 8-bit
void IIC_start(uint8_t i2c_address);

//slaveAddr 8-bit
void IIC_write_byte(uint8_t* data_ptr,uint8_t len);

//slaveAddr 8-bit
void IIC_cmd_begin();
#endif