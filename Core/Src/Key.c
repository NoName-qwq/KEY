#include "Key.h"
#include "key.h"
#include "main.h"
#include "tim.h"
#include <stdint.h>


//===========按键状态=============================================================

#define KEY_UNPRESSED          0
#define KEY_PRESSED            1


//==========按键标志==============================================================

uint8_t Key_Flag[KEY_COUNT];                //多按键标志
// [6]REPEAT      [5]LONG     [4]DOUBLE     [3]SINGLE     [2]UP     [1]DOWN     [0]HOLD


void key_tick(){
    static uint8_t Count,i;
    static uint8_t CurrState[KEY_COUNT],PrevState[KEY_COUNT];
    static uint8_t S[KEY_COUNT];
    static uint16_t Time[KEY_COUNT];

    for (i = 0; i < KEY_COUNT; i++) {
        if (Time[i] > 0) {
            Time[i] --;
        }
    }

    Count ++;
    if (Count == 20) {
        Count = 0;

        for (i = 0; i < KEY_COUNT; i++) {

            PrevState[i] = CurrState[i];
            CurrState[i] = Key_GetState(i);  


            if (CurrState[i] == KEY_PRESSED) {
                Key_Flag[i] |= KEY_HOLD;
            }else {
                Key_Flag[i] &= ~KEY_HOLD;
            }
            if (PrevState[i] == KEY_UNPRESSED && CurrState[i] == KEY_PRESSED) {
                Key_Flag[i] |= KEY_DOWN;
            }
            if (PrevState[i] == KEY_PRESSED && CurrState[i] == KEY_UNPRESSED) {
                Key_Flag[i] |= KEY_UP;
            }

            if (S[i] == 0) {
                if (CurrState[i] == KEY_PRESSED) {
                    Key_Flag[i] |= KEY_DOWN;
                    Time[i] = KEY_TIME_LONG;
                    S[i] = 1;
                }            
            }else if (S[i] == 1) {
                if (CurrState[i] == KEY_UNPRESSED) {
                    Time[i] = KEY_TIME_DOUBLE;
                    S[i] = 2;
                }else if (Time[i] == 0) {
                    Time[i] = KEY_TIME_REPEAT;
                    Key_Flag[i] |= KEY_LONG;
                    S[i] = 4;
                }
            }else if (S[i] == 2) {
                if (CurrState[i] == KEY_PRESSED) {
                    Key_Flag[i] |= KEY_DOUBLE;
                    S[i] = 3;
                }else if (Time[i] == 0) {
                    Key_Flag[i] |= KEY_SINGLE;
                    S[i] = 0;
                }
            }else if (S[i] == 3) {
                if (CurrState[i] == KEY_UNPRESSED) {
                    S[i] = 0;
                }
            }else if (S[i] == 4) {
                if (CurrState[i] == KEY_UNPRESSED) {
                    S[i] = 0;
                }else if (Time[i] == 0) {
                    Time[i] = KEY_TIME_REPEAT;
                    Key_Flag[i] |= KEY_REPEAT;
                }
            }


        }
        
    }
}


uint8_t Key_GetState(uint8_t KeyNum){ 
    if (KeyNum == 0) {
        if (!HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)) {
            return KEY_PRESSED;
        }
    }else if (KeyNum == 1) {
        if (!HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)) {
            return KEY_PRESSED;
        }
    }
//如果按键增加可以继续else if
//注意按键硬件连接方式，key0和key1默认引脚读取到0为按下

    return KEY_UNPRESSED;
}


uint8_t Key_CheckFlag(uint8_t KeyNum,uint8_t Flag)
{
    if (Key_Flag[KeyNum] & Flag) 
        {
            if (Flag != KEY_HOLD) 
            {
                Key_Flag[KeyNum] &= ~Flag;
            }
            return 1;
        }
    return 0;
}







// void led_tick(){
//     static uint16_t LED_Count;
//     LED_Count++;
//     LED_Count%=1000;

//     if (LED_Count < 500) {
//         HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
//     }else {
//         HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
//     }
// }
