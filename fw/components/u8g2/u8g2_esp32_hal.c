#include "stdlib.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "u8g2.h"
#include "i2cDriver.h"
#include "u8g2_esp32_hal.h"
#include "powerCtrl.h"

static const char* TAG = "u8g2_hal";

/*
 * HAL callback function as prescribed by the U8G2 library.  This callback is
 * invoked to handle I2C communications.
 */
/*uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t* u8x8,
                               uint8_t msg,
                               uint8_t arg_int,
                               void* arg_ptr) {
  ESP_LOGD(TAG, "i2c_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p", msg,
           arg_int, arg_ptr);

  switch (msg) {
    case U8X8_MSG_BYTE_SEND: {
      uint8_t* data_ptr = (uint8_t*)arg_ptr;
      ESP_LOG_BUFFER_HEXDUMP(TAG, data_ptr, arg_int, ESP_LOG_VERBOSE);

      IIC_write_byte(data_ptr,arg_int);
      break;
    }

    case U8X8_MSG_BYTE_START_TRANSFER: {
      uint8_t i2c_address = u8x8_GetI2CAddress(u8x8);
      IIC_start(i2c_address);
      break;
    }

    case U8X8_MSG_BYTE_END_TRANSFER: {
      IIC_cmd_begin();
      break;
    }
  }
  return 0;
}  // u8g2_esp32_i2c_byte_cb
*/
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t* u8x8,
                               uint8_t msg,
                               uint8_t arg_int,
                               void* arg_ptr) {


  static uint8_t buffer[192];
  static uint8_t buf_idx;
  uint8_t *data;

  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
      data = (uint8_t *)arg_ptr;  
      while( arg_int > 0 )
      {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
      }  
      break;
    case U8X8_MSG_BYTE_INIT:
      /* add your custom code to init i2c subsystem */
      break;
    case U8X8_MSG_BYTE_SET_DC:
      /* ignored for i2c */
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      buf_idx = 0;
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
     IIC_WriteBytes(u8x8_GetI2CAddress(u8x8),buffer, buf_idx);
      break;
    default:
      return 0;
  }
  return 1;
}  // u8g2_esp32_i2c_byte_cb


/*
 * HAL callback function as prescribed by the U8G2 library.  This callback is
 * invoked to handle callbacks for GPIO and delay functions.
 */
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t* u8x8,
                                     uint8_t msg,
                                     uint8_t arg_int,
                                     void* arg_ptr) {
  ESP_LOGD(TAG,
           "gpio_and_delay_cb: Received a msg: %d, arg_int: %d, arg_ptr: %p",
           msg, arg_int, arg_ptr);

  switch (msg) {
    case U8X8_MSG_DELAY_MILLI:
      //vTaskDelay(arg_int / portTICK_PERIOD_MS);
      break;
  }
  return 0;
}  // u8g2_esp32_gpio_and_delay_cb

void u8g2Init(u8g2_t *pU8g2)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        pU8g2,
        U8G2_R0,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb);

    u8x8_SetI2CAddress(&pU8g2->u8x8, 0x78);

    u8g2_InitDisplay(pU8g2);

    u8g2_SetPowerSave(pU8g2, 0);  // wake up display

    u8g2_ClearBuffer(pU8g2);

    u8g2_DrawBox(pU8g2, 0, 26, 80, 6);
    u8g2_DrawFrame(pU8g2, 0, 26, 100, 6);

    ESP_LOGI(TAG, "u8g2_SetFont");
    u8g2_SetFont(pU8g2, u8g2_font_9x18_tn);
    ESP_LOGI(TAG, "u8g2_DrawStr");
    u8g2_DrawStr(pU8g2, 2, 17, "Hi nkolban!");
    ESP_LOGI(TAG, "u8g2_SendBuffer");

    u8g2_SetFont(pU8g2, u8g2_font_unifont_t_symbols);
    u8g2_DrawGlyph(pU8g2, 5, 60, 0x2603);

    u8g2_SetFont(pU8g2, u8g2_font_emoticons21_tr);
    u8g2_DrawGlyph(pU8g2, 22, 60, 0x29);

    u8g2_SendBuffer(pU8g2);
}


