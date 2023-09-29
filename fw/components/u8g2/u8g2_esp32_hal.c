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

static TaskHandle_t xHandle_task1 = NULL;
static void vTask1(void * pvParameters);
static u8g2_t m_u8g2;

extern PowerOpt_t *powerCtrl;


void u8g2Init()
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &m_u8g2,
        U8G2_R0,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb);

    u8x8_SetI2CAddress(&m_u8g2.u8x8, 0x78);

    u8g2_InitDisplay(&m_u8g2);

    u8g2_SetPowerSave(&m_u8g2, 0);  // wake up display

    u8g2_ClearBuffer(&m_u8g2);

    u8g2_DrawBox(&m_u8g2, 0, 26, 80, 6);
    u8g2_DrawFrame(&m_u8g2, 0, 26, 100, 6);

    ESP_LOGI(TAG, "u8g2_SetFont");
    u8g2_SetFont(&m_u8g2, u8g2_font_9x18_tn);
    ESP_LOGI(TAG, "u8g2_DrawStr");
    u8g2_DrawStr(&m_u8g2, 2, 17, "Hi nkolban!");
    ESP_LOGI(TAG, "u8g2_SendBuffer");

    u8g2_SetFont(&m_u8g2, u8g2_font_unifont_t_symbols);
    u8g2_DrawGlyph(&m_u8g2, 5, 60, 0x2603);

    u8g2_SetFont(&m_u8g2, u8g2_font_emoticons21_tr);
    u8g2_DrawGlyph(&m_u8g2, 22, 60, 0x29);

    u8g2_SendBuffer(&m_u8g2);

    xTaskCreatePinnedToCore(vTask1,             //任务函数
                            "task1",            //任务名称
                            2048,               //堆栈大小
                            NULL,               //传递参数
                            2,                  //任务优先级
                            &xHandle_task1,     //任务句柄
                            0);    //无关联，不绑定在任何一个核上
}

void vTask1(void * pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int updateIntervalMs = 33;
    const TickType_t ticks = pdMS_TO_TICKS(updateIntervalMs);

    u8g2_ClearBuffer(&m_u8g2);

    Point_t point_s,point_e;
    int count = 0;
    point_s.x = 0;

    int64_t timeuse,timeuse2;
    timeuse = 0;
    timeuse2 = 0;
    uint32_t time3;
    u8g2_SetFont(&m_u8g2, u8g2_font_6x13_mr);
    char var_buff[100];
    u8g2_SetFontMode(&m_u8g2,0);
    u8g2_SetDrawColor(&m_u8g2,1);
    uint32_t voltage = 0;
    while (1)
    {
        xTaskDelayUntil( &xLastWakeTime, ticks);

        point_s.x = (count/17)*127;
        point_s.y = 0;

        point_e.x = 8*(count%17);
        point_e.y = 64;

        count++;

        u8g2_DrawLine(&m_u8g2,point_s.x,point_s.y,point_e.x,point_e.y);
        time3 = (timeuse2-timeuse);

        voltage = powerCtrl->getPowerVoltagemV();
//        ESP_LOGI(TAG, "voltage : %4dmV",voltage);

        sprintf(var_buff, "%6dus %dmV", time3,voltage);

        u8g2_DrawStr(&m_u8g2, 2, 15, var_buff);
        //u8g2_SetDrawColor(&m_u8g2,0);
        //u8g2_DrawBox(&m_u8g2, 0, 0, 127, 17);
        //u8g2_SetDrawColor(&m_u8g2,1);
        //u8g2_DrawButtonUTF8(&m_u8g2, 64, 15,U8G2_BTN_BW1|U8G2_BTN_HCENTER,60,1,1,var_buff);
        if (count > 32)
        {
            count = 0;
            u8g2_ClearBuffer(&m_u8g2);
        }

        //printf(":start\n");
        timeuse = esp_timer_get_time();
        u8g2_SendBuffer(&m_u8g2);
        timeuse2 = esp_timer_get_time();

        //printf(": %llu\n",timeuse2-timeuse);
    }
}
