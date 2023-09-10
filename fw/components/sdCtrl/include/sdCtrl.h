#ifndef _SDCTRL_H
#define _SDCTRL_H

void SDCardCtrlInit();

void sdCtrlSelectStrapping();

void sdCtrlSelectSDCard();

void SDIOInit(const char* base_path);
#endif