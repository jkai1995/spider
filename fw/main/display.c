#include "stdlib.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "u8g2.h"
#include "u8g2_esp32_hal.h"

#include "powerCtrl.h"
#include "inputKey.h"
#include "rotary_encoder.h"
#include "webserver.h"
#include "protocol_examples_common.h"

static const char* TAG = "display";



static TaskHandle_t xHandle_task1 = NULL;
static void vTask1(void * pvParameters);
static u8g2_t m_u8g2;

extern PowerOpt_t*          powerCtrl;
extern InputKeyOpt_t*       s_pInputKey;
extern rotary_encoder_t*    encoder;


typedef struct
{
    int16_t x;
    int16_t y;
}Point_t;

typedef struct
{
    Point_t point_s;
    Point_t point_e;
}Line_t;


typedef enum
{
    FeatureFps,
    FeatureParameter,
    FeatureTypeMax,
}FeatureType;

typedef struct
{
    FeatureType selectedFeature;
}display_t;

Line_t s_line;
void featureFpsMain(int count)
{
    s_line.point_s.x = (count/17)*127;
    s_line.point_s.y = 0;

    s_line.point_e.x = 8*(count%17);
    s_line.point_e.y = 64;

    if (0 == count)
    {
        u8g2_ClearBuffer(&m_u8g2);
    }

    u8g2_DrawLine(&m_u8g2,s_line.point_s.x,s_line.point_s.y,s_line.point_e.x,s_line.point_e.y);
}

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

display_t m_display;

void printMessage()
{
    inputKey_State_t upst = s_pInputKey->getKeyState(Up);
    inputKey_State_t midst = s_pInputKey->getKeyState(Middle);
    inputKey_State_t dst = s_pInputKey->getKeyState(Down);

    if (upst != state_None || midst != state_None || dst != state_None)
    {
        ESP_LOGI(TAG, "key : %d %d %d ",upst,midst,dst);
    }

    int encoderCnt;
    float encoderRPS;
    encoderCnt = encoder->get_counter_value(encoder,&encoderRPS);
    if (encoderCnt != 0)
    {
        ESP_LOGI(TAG, "encoder : cnt=%4d rspeed=%f r/s ",encoderCnt,encoderRPS);
    }

    uint32_t voltage = 0;

    voltage = powerCtrl->getPowerVoltagemV();
    ESP_LOGI(TAG, "voltage : %4dmV",voltage);

}



#define SIZEOFBUFF 100
void vTask1(void * pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int updateIntervalMs = 33;
    const TickType_t ticks = pdMS_TO_TICKS(updateIntervalMs);

    u8g2_ClearBuffer(&m_u8g2);

    int count = 0;

    int64_t timeuse,timeuse2;
    timeuse = 0;
    timeuse2 = 0;
    uint32_t time3 = 0;
    u8g2_SetFont(&m_u8g2, u8g2_font_6x13_mr);
    char var_buff[SIZEOFBUFF];
    u8g2_SetFontMode(&m_u8g2,0);
    u8g2_SetDrawColor(&m_u8g2,1);

    while (1)
    {
        xTaskDelayUntil( &xLastWakeTime, ticks);

        uint32_t voltage = powerCtrl->getPowerVoltagemV();
        PowerCtrl_type_t pc_t = powerCtrl->getPowerCtrlType();

        m_display.selectedFeature = FeatureParameter;
        if (FeatureFps == m_display.selectedFeature)
        {
            featureFpsMain(count);
        }
        else if (FeatureParameter == m_display.selectedFeature)
        {
            sprintf(var_buff, "%6dus %dmV", time3,voltage);
            u8g2_DrawStr(&m_u8g2, 2, 15, var_buff);
            sprintf(var_buff, "select : %s", (pc_t == PC_UART)?"UART":"PD");
            u8g2_DrawStr(&m_u8g2, 2, 30, var_buff);
            esp_ip4_addr_t ipaddr = get_IPv4();
            snprintf(var_buff, SIZEOFBUFF, "IPv4:"  IPSTR, IP2STR(&ipaddr));
            u8g2_DrawStr(&m_u8g2, 2, 45, var_buff);
        }
        else
        {
            //u8g2_SetDrawColor(&m_u8g2,0);
            //u8g2_DrawBox(&m_u8g2, 0, 0, 127, 17);
            //u8g2_SetDrawColor(&m_u8g2,1);
            //u8g2_DrawButtonUTF8(&m_u8g2, 64, 15,U8G2_BTN_BW1|U8G2_BTN_HCENTER,60,1,1,var_buff);
        }



        count++;
        if (count > 32)
        {
            count = 0;
        }

        timeuse = esp_timer_get_time();
        u8g2_SendBuffer(&m_u8g2);
        timeuse2 = esp_timer_get_time();
        time3 = (timeuse2-timeuse);
    }
}



