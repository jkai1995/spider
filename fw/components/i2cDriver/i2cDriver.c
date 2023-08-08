#include "i2cDriver.h"

//static const char *TAG = "i2cDriver";

#define I2C_MASTER_SCL_IO           16                         /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           17                         /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
//#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

esp_err_t I2C1_Init(uint32_t clk_speed)
{
    int ret;
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,//GPIO_PULLUP_DISABLE
        .scl_pullup_en = GPIO_PULLUP_DISABLE,//GPIO_PULLUP_DISABLE
        .master.clk_speed = clk_speed,
    };

    i2c_param_config(i2c_master_port, &conf);

    ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    return ret;

}

//slaveAddr 8-bit
esp_err_t IIC_ReadBytes(uint8_t slaveAddr,uint8_t *data, size_t len)
{
    return i2c_master_read_from_device(I2C_MASTER_NUM,(slaveAddr >> 1),data,len,I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

//slaveAddr 8-bit
esp_err_t IIC_WriteBytes(uint8_t slaveAddr,uint8_t *data, size_t len)
{
    return i2c_master_write_to_device(I2C_MASTER_NUM,(slaveAddr >> 1),data,len,I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

//slaveAddr 8-bit
esp_err_t IIC_ReadReg(uint8_t slaveAddr,uint8_t reg_addr,uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, (slaveAddr >> 1), &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

