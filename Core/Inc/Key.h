#ifndef __KEY_H
#define __KEY_H

#include "tim.h"
//==============================================================================
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
//     if (htim == &htim4) {
//         key_tick();
//     }
// }
//===============================================================================
#define KEY_HOLD               0x01
#define KEY_DOWN               0x02
#define KEY_UP                 0x04
#define KEY_SINGLE             0x08
#define KEY_DOUBLE             0x10
#define KEY_LONG               0x20
#define KEY_REPEAT             0x40


void key_tick();
uint8_t Key_GetState();
uint8_t Key_CheckFlag(uint8_t Flag);




void led_tick();

#endif