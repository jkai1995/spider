/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "webserver.h"

#include "i2cDriver.h"
#include <esp_log.h>


//#include "sdCtrl.h"
#include "fileExplorer.h"
#include "sample.h"
#include "display.h"

static const char *TAG = "wjk";


void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    //webserver_main();

    int updateIntervalMs = 20;

    int ret = I2C1_Init(600000);
    ESP_LOGI(TAG, "I2C1_Init ret = %d",ret);
    //OLED_Init();
    //PowerCtrlInit();
    FileExplorer_t *pFileExplorer;
    FileExplorerInit(&pFileExplorer);
    //SDCardCtrlInit();
    //sdCtrlSelectSDCard();
    //encoderInit();
    //showDemo();
    sampleTaskInit();

    displayInit();
    //webserver_main();
    //SDIOInit();
    //inputKeyInit(updateIntervalMs);
    //int keyValue1= 0;
    //int keyValue2= 0;
    //int keyValue3= 0;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t ticks = pdMS_TO_TICKS(updateIntervalMs);
    char *selectedName;
    int  isDir = 0;
    FileExplorer_getSelectedEntry(&selectedName,&isDir);
    while (1)
    {
        xTaskDelayUntil( &xLastWakeTime, ticks);
        //updateKeyValue();
        //updateAdcValue();

        //voltage = getPowerVoltage();
        //ESP_LOGI(TAG, "voltage %d",voltage);
/*        if (state_None != getKeyState(Up))
        {
            keyValue1++;
        }
        else
        {
            keyValue1 = 0;
        }

        if (state_None != getKeyState(Middle))
        {
            keyValue2++;
        }
        else
        {
            keyValue2 = 0;
        }

        if (state_None != getKeyState(Down))
        {
            keyValue3++;
        }
        else
        {
            keyValue3 = 0;
        }

        if (keyValue1 > 0 || keyValue2 > 0 || keyValue3 >0)
        {
            inputKey_State_t upst = getKeyState(Up);
            inputKey_State_t midst = getKeyState(Middle);
            inputKey_State_t dst = getKeyState(Down);
            if (upst == state_click)
            {
                FileExplorer_preEntry();
            }
            else if (midst == state_click)
            {
                if (isDir)
                {
                    FileExplorer_cd(selectedName);
                }
                else
                {
                    FileExplorer_cat(selectedName);
                }
            }
            else if (dst == state_click)
            {
                FileExplorer_nextEntry();
            }

            FileExplorer_getSelectedEntry(&selectedName,&isDir);
            if (isDir)
            {
                ESP_LOGI(TAG, "<%s>",selectedName);
            }
            else
            {
                ESP_LOGI(TAG, " %s ",selectedName);
            }
        }
*/
        //int counter = encoder->get_counter_value(encoder);
        //ESP_LOGI(TAG, "Encoder value: %d", counter);

    }
}
