#include "stdlib.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "u8g2.h"
#include "u8g2_esp32_hal.h"

#include "powerCtrl.h"
#include "inputKey.h"
#include "rotary_encoder.h"
#include "powerCtrl.h"

static const char* TAG = "display";



static TaskHandle_t xHandle_task1 = NULL;
static void vTask1(void * pvParameters);
static u8g2_t m_u8g2;

extern PowerOpt_t *powerCtrl;


void displayInit()
{
    u8g2Init(&m_u8g2);

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

        PowerCtrl_type_t pc_t = powerCtrl->getPowerCtrlType();
        sprintf(var_buff, "select : %s", (pc_t == PC_UART)?"UART":"PD");
        u8g2_DrawStr(&m_u8g2, 2, 30, var_buff);
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



