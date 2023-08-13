#ifndef _INPUTKEY_H
#define _INPUTKEY_H

typedef enum
{
    Down,
    Middle,
    Up,
    PinMax,
}inputKey_type_t;

typedef enum
{
    state_None,
    state_click,
    state_dbclick,
    state_press,
}inputKey_State_t;

void inputKeyInit(int    updateIntervalMs);

void updateKeyValue();

uint8_t getKeyLevel(inputKey_type_t inputKey);

inputKey_State_t getKeyState(inputKey_type_t inputKey);

bool isKeyDown(inputKey_type_t inputKey);

#endif