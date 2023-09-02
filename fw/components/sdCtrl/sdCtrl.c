#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdCtrl.h"
#include "gpioDriver.h"

static const char *TAG = "sdCtrl";


typedef enum
{
    CH442E_Strapping = 0,//SEL_SD level == 0
    CH442E_SDCard = 1,//SEL_SD level == 1
}USBSEL_Level;//CH442E

static gpioOutInfo_t m_gpioSdCardCtrl[] = 
{
    {"SEL_SD",    26,  CH442E_Strapping},
};

//                              ----> Strapping
//                            |
//       GPIO2,GPIO12      ---
//                            |
//                              ----> SD Card



void SDCardCtrlInit()
{
    uint64_t pinMask = 0;
    pinMask |= (1 << m_gpioSdCardCtrl[0].pinx);

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = pinMask;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLDOWN_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    gpio_set_level(m_gpioSdCardCtrl[0].pinx,m_gpioSdCardCtrl[0].dftLevel);
}

void sdCtrlSelectStrapping()
{
    gpio_set_level(m_gpioSdCardCtrl[0].pinx,CH442E_Strapping);
}

void sdCtrlSelectSDCard()
{
    gpio_set_level(m_gpioSdCardCtrl[0].pinx,CH442E_SDCard);
}


