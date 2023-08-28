#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"
#include "powerCtrl.h"

static const char *TAG = "PowerCtrl";


typedef struct
{
    char*           pinName;
    uint32_t        pinx;
    int             dftLevel;
}gpioOutInfo_t;

typedef enum
{
    CH442E_UART = 0,//SEL_USB level == 0
    CH442E_PowerDelivery = 1,//SEL_USB level == 1
}USBSEL_Level;//CH442E

static gpioOutInfo_t m_gpioPowerCtrl[] = 
{
    {"SEL_USB",    27,  CH442E_UART},
};

//                              ----> UART IC CH340
//                            |
//  USB Type C Port DM,DP ---
//                            |
//                              ----> PD IC  CH224K


//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

typedef struct
{
    char*               pinName;
    adc1_channel_t      channel;
    adc_bits_width_t    width_bit;
    adc_atten_t         atten;
    esp_adc_cal_value_t calSource;
}AdcCfg_t;

#define NUM_OF_SAMPLES     10

typedef struct
{
    esp_adc_cal_characteristics_t   adc1_chars;
    bool                            cali_enable;
    int                             adc_raw[NUM_OF_SAMPLES];
    int                             adcrawAve;
    int                             adcrawTotal;
    int                             curIdx;
    int                             datacount;
    AdcCfg_t                        *pAdcCfg;
    uint32_t                        voltage;
}AdcInfo_t;


static AdcCfg_t m_ADCCfg = 
{
    "PowerDetect",
    ADC1_CHANNEL_7,
    ADC_WIDTH_10Bit,
    ADC_ATTEN_DB_6,
    ADC_EXAMPLE_CALI_SCHEME,
};

static AdcInfo_t m_AdcInfo;

static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(m_ADCCfg.calSource);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, m_ADCCfg.atten, m_ADCCfg.width_bit, 0, &(m_AdcInfo.adc1_chars));
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

void updateAdcValue()
{
    int adcraw = adc1_get_raw(m_AdcInfo.pAdcCfg->channel);

    m_AdcInfo.datacount++;
    if (m_AdcInfo.datacount > NUM_OF_SAMPLES)
    {
        m_AdcInfo.adcrawTotal -= m_AdcInfo.adc_raw[m_AdcInfo.curIdx];
    }
    m_AdcInfo.adc_raw[m_AdcInfo.curIdx] = adcraw;
    m_AdcInfo.adcrawTotal += adcraw;
    m_AdcInfo.datacount = (m_AdcInfo.datacount >= NUM_OF_SAMPLES)?(NUM_OF_SAMPLES):(m_AdcInfo.datacount);

    ESP_LOGI(TAG, "adc_raw[%d] = %d adcrawTota=%d datacount=%d", m_AdcInfo.curIdx, adcraw,m_AdcInfo.adcrawTotal,m_AdcInfo.datacount);
    if (m_AdcInfo.cali_enable)
    {
        if (m_AdcInfo.datacount == NUM_OF_SAMPLES)
        {
            m_AdcInfo.adcrawAve = m_AdcInfo.adcrawTotal/NUM_OF_SAMPLES;
            m_AdcInfo.voltage = esp_adc_cal_raw_to_voltage(m_AdcInfo.adcrawAve, &(m_AdcInfo.adc1_chars));
        }

        ESP_LOGI(TAG, "cali data: %d mV", m_AdcInfo.voltage);
    }

    m_AdcInfo.curIdx++;
    m_AdcInfo.curIdx = (m_AdcInfo.curIdx >= NUM_OF_SAMPLES)?(0):(m_AdcInfo.curIdx);

}

uint32_t getPowerVoltage()
{
    return m_AdcInfo.voltage;//mV
}


static void adcInit()
{
    m_AdcInfo.pAdcCfg = &m_ADCCfg;
    m_AdcInfo.cali_enable = adc_calibration_init();

    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(m_ADCCfg.width_bit));
    ESP_ERROR_CHECK(adc1_config_channel_atten(m_ADCCfg.channel, m_ADCCfg.atten));

    m_AdcInfo.curIdx = 0;
    m_AdcInfo.datacount = 0;
    m_AdcInfo.adcrawTotal = 0;
    m_AdcInfo.voltage = 0;
    m_AdcInfo.adcrawAve = 0;
}

void PowerCtrlInit()
{
    uint64_t pinMask = 0;
    pinMask |= (1 << m_gpioPowerCtrl[0].pinx);

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = pinMask;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLDOWN_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    gpio_set_level(m_gpioPowerCtrl[0].pinx,m_gpioPowerCtrl[0].dftLevel);

    adcInit();
}

void powerCtrlSelectUart()
{
    gpio_set_level(m_gpioPowerCtrl[0].pinx,CH442E_UART);
}

void powerCtrlSelectPD()
{
    gpio_set_level(m_gpioPowerCtrl[0].pinx,CH442E_PowerDelivery);
}

