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

//===========按键状态=============================================================

#define KEY_HOLD               0x01
#define KEY_DOWN               0x02
#define KEY_UP                 0x04
#define KEY_SINGLE             0x08
#define KEY_DOUBLE             0x10
#define KEY_LONG               0x20
#define KEY_REPEAT             0x40

//===========可修改宏定义=========================================================
#define KEY_COUNT              2        // 按键数量

#define KEY_TIME_DOUBLE        200      // 双击时间
#define KEY_TIME_LONG          2000     // 长按时间
#define KEY_TIME_REPEAT        100      // 重复时间


//============外部函数=============================================================
void key_tick();

uint8_t Key_GetState(uint8_t KeyNum);
uint8_t Key_CheckFlag(uint8_t KeyNum,uint8_t Flag);

void led_tick();

#endif