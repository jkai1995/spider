#ifndef _POWERCTRL_H
#define _POWERCTRL_H

typedef enum
{
    PC_UART,
    PC_PowerDelivery,
    PC_Max,
} PowerCtrl_type_t;

typedef struct
{
    void                (*updateAdcValue)   (void);
    uint32_t            (*getPowerVoltagemV)  (void);
    void                (*powerCtrlSelect)  (PowerCtrl_type_t pc_t);
    PowerCtrl_type_t    (*getPowerCtrlType) (void);
}PowerOpt_t;

void PowerCtrlInit(PowerOpt_t **opt);
#endif