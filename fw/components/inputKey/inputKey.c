#include "driver/gpio.h"
#include "gpioDriver.h"
#include "inputKey.h"

typedef struct
{
    char*           pinName;
    uint32_t        pinx;
    int             validLevel;//for pullup pin, default level is "1", valid level is "0"
}gpioInfo_t;

typedef struct
{
    gpioInfo_t*         pGpioInfo;
    uint8_t             level;
    bool                isValidLevel;
    uint32_t            setTimeMs;//duration time of valid level
    uint32_t            setCount;
    inputKey_State_t    keyState;
}inputPin_Info_t;

typedef struct
{
    uint32_t        pinNum;
    uint32_t        updateIntervalMs;
    uint32_t        maxRecordMs;
    uint32_t        maxRecordCount;
    uint32_t        pressDetectMs;
    inputPin_Info_t pinInfo[PinMax];
    InputKeyOpt_t   opt;
}inputKey_Info_t;

static gpioInfo_t m_gpioInputKey[PinMax] = 
{
    {"Down",    21, 0},
    {"Middle",  19, 0},
    {"Up",      18, 0},
};

static inputKey_Info_t g_inputKeyInfo;

#define GetInKeyValue(p) gpio_get_level(m_gpioInputKey[p].pinx)


void updateKeyValue()
{
    for (int i = 0; i < g_inputKeyInfo.pinNum; i++)
    {
        inputPin_Info_t *pPinInfo = &(g_inputKeyInfo.pinInfo[i]);
        pPinInfo->level = GetInKeyValue(i);
        pPinInfo->isValidLevel = (pPinInfo->level == pPinInfo->pGpioInfo->validLevel)?true:false;
        if (pPinInfo->isValidLevel)
        {
            pPinInfo->setCount++;
            if (pPinInfo->setCount > g_inputKeyInfo.maxRecordCount)
            {
                pPinInfo->setCount = g_inputKeyInfo.maxRecordCount;
            }

            pPinInfo->setTimeMs = pPinInfo->setCount * g_inputKeyInfo.updateIntervalMs;
            if (pPinInfo->setTimeMs >= g_inputKeyInfo.pressDetectMs)
            {
                pPinInfo->keyState = state_press;
            }
        }
        else
        {
            if (pPinInfo->setTimeMs > 0 && pPinInfo->keyState == state_None)
            {
                pPinInfo->keyState = state_click;
            }
            else
            {
                pPinInfo->keyState = state_None;
            }
            pPinInfo->setCount = 0;
            pPinInfo->setTimeMs = 0;
        }
    }
}

uint8_t getKeyLevel(inputKey_type_t inputKey)
{
    if (inputKey < g_inputKeyInfo.pinNum)
    {
        return g_inputKeyInfo.pinInfo[inputKey].level;
    }
    return 0;
}

inputKey_State_t getKeyState(inputKey_type_t inputKey)
{
    if (inputKey < g_inputKeyInfo.pinNum)
    {
        return g_inputKeyInfo.pinInfo[inputKey].keyState;
    }
    return 0;
}

bool isKeyDown(inputKey_type_t inputKey)
{
    if (inputKey < g_inputKeyInfo.pinNum)
    {
        return g_inputKeyInfo.pinInfo[inputKey].isValidLevel;
    }
    return 0;
}

void inputKeyInit(int    updateIntervalMs,InputKeyOpt_t **keyopt)
{
    uint64_t pinMask = 0;
    g_inputKeyInfo.pinNum = PinMax;
    g_inputKeyInfo.updateIntervalMs = updateIntervalMs;
    g_inputKeyInfo.maxRecordMs = 10*1000;
    g_inputKeyInfo.maxRecordCount = g_inputKeyInfo.maxRecordMs/updateIntervalMs;
    g_inputKeyInfo.pressDetectMs = 1000;

    for (int i = 0; i < g_inputKeyInfo.pinNum; i++)
    {
        g_inputKeyInfo.pinInfo[i].pGpioInfo = &(m_gpioInputKey[i]);
        pinMask |= (1 << m_gpioInputKey[i].pinx);
    }

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = pinMask;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    g_inputKeyInfo.opt.updateKeyValue   = updateKeyValue;
    g_inputKeyInfo.opt.getKeyLevel      = getKeyLevel;
    g_inputKeyInfo.opt.getKeyState      = getKeyState;
    g_inputKeyInfo.opt.isKeyDown        = isKeyDown;

    *keyopt = &(g_inputKeyInfo.opt);

    updateKeyValue();
}


