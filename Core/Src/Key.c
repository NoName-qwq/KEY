#include "Key.h"
#include "key.h"
#include "tim.h"
#include <stdint.h>


#define KEY_UNPRESSED          0
#define KEY_PRESSED            1

#define KEY_TIME_DOUBLE        200
#define KEY_TIME_LONG          2000
#define KEY_TIME_REPEAT        100

uint8_t Key_Flag = 0;
// [6]REPEAT      [5]LONG     [4]DOUBLE     [3]SINGLE     [2]UP     [1]DOWN     [0]HOLD


void key_tick(){
    static uint8_t Count;
    static uint8_t CurrState,PrevState;
    static uint8_t S;
    static uint16_t Time;

    if (Time > 0) {
        Time --;
    }

    Count ++;
    if (Count == 20) {
        Count = 0;
        PrevState = CurrState;
        CurrState = Key_GetState();  


        if (CurrState == KEY_PRESSED) {
            Key_Flag |= KEY_HOLD;
        }else {
            Key_Flag &= ~KEY_HOLD;
        }
        if (PrevState == KEY_UNPRESSED && CurrState == KEY_PRESSED) {
            Key_Flag |= KEY_DOWN;
        }
        if (PrevState == KEY_PRESSED && CurrState == KEY_UNPRESSED) {
            Key_Flag |= KEY_UP;
        }

        if (S == 0) {
            if (CurrState == KEY_PRESSED) {
                Key_Flag |= KEY_DOWN;
                Time = KEY_TIME_LONG;
                S = 1;
            }            
        }else if (S == 1) {
            if (CurrState == KEY_UNPRESSED) {
                Time = KEY_TIME_DOUBLE;
                S = 2;
            }else if (Time == 0) {
                Time = KEY_TIME_REPEAT;
                Key_Flag |= KEY_LONG;
                S = 4;
            }
        }else if (S == 2) {
            if (CurrState == KEY_PRESSED) {
                Key_Flag |= KEY_DOUBLE;
                S = 3;
            }else if (Time == 0) {
                Key_Flag |= KEY_SINGLE;
                S = 0;
            }
        }else if (S == 3) {
            if (CurrState == KEY_UNPRESSED) {
                S = 0;
            }
        }else if (S == 4) {
            if (CurrState == KEY_UNPRESSED) {
                S = 0;
            }else if (Time == 0) {
                Time = KEY_TIME_REPEAT;
                Key_Flag |= KEY_REPEAT;
            }
        }
        
    }
}


uint8_t Key_GetState(){ 
    if (!HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)) {
        return KEY_PRESSED;
    }
    return KEY_UNPRESSED;
}


uint8_t Key_CheckFlag(uint8_t Flag)
{
    if (Key_Flag & Flag) 
    {
        if (Flag != KEY_HOLD) 
        {
            Key_Flag &= ~Flag;
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
