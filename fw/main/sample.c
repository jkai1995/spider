#include "stdlib.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "inputKey.h"
#include "rotary_encoder.h"
#include "powerCtrl.h"

#include <esp_log.h>


#define SAMPLEINTERVAL_MS (20)
#define ENCODERSAMPLE_DIV (10)
#define SAMPLINGTICKMAX (1000)


static const char *TAG = "sample";
static TaskHandle_t s_xHandle_task1 = NULL;
static InputKeyOpt_t    *s_pInputKey = NULL;
rotary_encoder_t        *encoder = NULL;
PowerOpt_t              *powerCtrl = NULL;


void encoderInit(void)
{
    // Rotary encoder underlying device is represented by a PCNT unit in this example
    uint32_t pcnt_unit = 0;

    // Create rotary encoder instance
    rotary_encoder_config_t config = ROTARY_ENCODER_DEFAULT_CONFIG((rotary_encoder_dev_t)pcnt_unit, 25, 33);

    config.pulseCntPR = 110;
    config.samplingPeriod_ms = ENCODERSAMPLE_DIV*SAMPLEINTERVAL_MS;

    ESP_ERROR_CHECK(rotary_encoder_new_ec11(&config, &encoder));

    // Filter out glitch (1us)
    ESP_ERROR_CHECK(encoder->set_glitch_filter(encoder, 1));

    // Start encoder
    ESP_ERROR_CHECK(encoder->start(encoder));

}

static void vTask1(void * pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t ticks = pdMS_TO_TICKS(SAMPLEINTERVAL_MS);

    uint32_t samplingTicks = 0;
    while (1)
    {
        s_pInputKey->updateKeyValue();
        if (0 == samplingTicks%ENCODERSAMPLE_DIV)
        {
            encoder->sample_encoder(encoder);
        }
        powerCtrl->updateAdcValue();

        samplingTicks++;
        if (samplingTicks >= SAMPLINGTICKMAX)
        {
            samplingTicks = 0;
        }
        xTaskDelayUntil( &xLastWakeTime, ticks);
    }
}

void sampleTaskInit()
{
    inputKeyInit(SAMPLEINTERVAL_MS,&s_pInputKey);
    encoderInit();
    PowerCtrlInit(&powerCtrl);
    xTaskCreatePinnedToCore(vTask1,             //任务函数
                            "task1",            //任务名称
                            2048,               //堆栈大小
                            NULL,               //传递参数
                            2,                  //任务优先级
                            &s_xHandle_task1,     //任务句柄
                            0);    //无关联，不绑定在任何一个核上
}


