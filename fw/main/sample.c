#include "stdlib.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "inputKey.h"
#include <esp_log.h>


#define SAMPLEINTERVAL_MS (20)

static const char *TAG = "sample";
static TaskHandle_t s_xHandle_task1 = NULL;
static InputKeyOpt_t *s_pInputKey = NULL;


static void vTask1(void * pvParameters);

void sampleTaskInit()
{
    inputKeyInit(SAMPLEINTERVAL_MS,&s_pInputKey);

    xTaskCreatePinnedToCore(vTask1,             //任务函数
                            "task1",            //任务名称
                            2048,               //堆栈大小
                            NULL,               //传递参数
                            2,                  //任务优先级
                            &s_xHandle_task1,     //任务句柄
                            0);    //无关联，不绑定在任何一个核上
}

void vTask1(void * pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t ticks = pdMS_TO_TICKS(SAMPLEINTERVAL_MS);

    while (1)
    {
        s_pInputKey->updateKeyValue();

        if (1)
        {
            inputKey_State_t upst = s_pInputKey->getKeyState(Up);
            inputKey_State_t midst = s_pInputKey->getKeyState(Middle);
            inputKey_State_t dst = s_pInputKey->getKeyState(Down);

            if (upst != state_None || midst != state_None || dst != state_None)
            {
                ESP_LOGI(TAG, "key : %d %d %d",upst,midst,dst);
            }
        }

        xTaskDelayUntil( &xLastWakeTime, ticks);
    }
}


