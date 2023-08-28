#ifndef _POWERCTRL_H
#define _POWERCTRL_H

void PowerCtrlInit();

void updateAdcValue();

uint32_t getPowerVoltage();

void powerCtrlSelectUart();

void powerCtrlSelectPD();

#endif