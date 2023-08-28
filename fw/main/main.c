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
#include "oled.h"
#include "bmp.h"
#include <esp_log.h>
#include "inputKey.h"
#include "powerCtrl.h"

static const char *TAG = "wjk";


void showDemo(void)
{
	static uint8_t t=' ';
			OLED_ShowPicture(0,0,128,64,BMP2,1);
		OLED_Refresh();
		/*vTaskDelay(pdMS_TO_TICKS(500));
		OLED_Clear();
		OLED_ShowChinese(0,0,0,16,1);
		OLED_ShowChinese(18,0,1,16,1);
		OLED_ShowChinese(36,0,2,16,1);
		OLED_ShowChinese(54,0,3,16,1);
		OLED_ShowChinese(72,0,4,16,1);
		OLED_ShowChinese(90,0,5,16,1);
		OLED_ShowChinese(108,0,6,16,1);
		OLED_ShowString(8,16,"ZHONGJINGYUAN",16,1);
		OLED_ShowString(20,32,"2014/05/01",16,1);
		OLED_ShowString(0,48,"ASCII:",16,1);
		OLED_ShowString(63,48,"CODE:",16,1);
		OLED_ShowChar(48,48,t,16,1);
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,48,t,3,16,1);
		OLED_Refresh();
		vTaskDelay(pdMS_TO_TICKS(500));
		OLED_Clear();
		OLED_ShowChinese(0,0,0,16,1);
	  OLED_ShowChinese(16,0,0,24,1);
		OLED_ShowChinese(24,20,0,32,1);
	  OLED_ShowChinese(64,0,0,64,1);
		OLED_Refresh();
	  vTaskDelay(pdMS_TO_TICKS(500));
	OLED_Clear();
		OLED_ShowString(0,0,"ABC",8,1);
		OLED_ShowString(0,8,"ABC",12,1);
	  OLED_ShowString(0,20,"ABC",16,1);
		OLED_ShowString(0,36,"ABC",24,1);
	  OLED_Refresh();
		vTaskDelay(pdMS_TO_TICKS(500));
		OLED_ScrollDisplay(11,4,1);*/

}


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

    int ret = I2C1_Init(100000);
    ESP_LOGI(TAG, "I2C1_Init ret = %d",ret);
    OLED_Init();
    PowerCtrlInit();
    showDemo();
    //webserver_main();

    inputKeyInit(updateIntervalMs);
    int keyValue1= 0;
    int keyValue2= 0;
    int keyValue3= 0;
    uint32_t voltage = 0;
    uint32_t lastv = 0;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t ticks = pdMS_TO_TICKS(updateIntervalMs);
    while (1)
    {
        xTaskDelayUntil( &xLastWakeTime, ticks);
        updateKeyValue();
        updateAdcValue();

        voltage = getPowerVoltage();
        ESP_LOGI(TAG, "voltage %d",voltage);
        if (state_None != getKeyState(Up))
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
            ESP_LOGI(TAG, "keyCount50Hz = %d,%d,%d",getKeyState(Up),getKeyState(Middle),getKeyState(Down));
        }



        if (lastv != voltage)
        {
            OLED_ShowNum(0,0,voltage,10,16,1);
            OLED_Refresh();
        }


        lastv = voltage;
    }
}
