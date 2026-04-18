#ifndef __OLED_SSD1306_H__
#define __OLED_SSD1306_H__

#include <stdint.h>
#include "string.h"
#include "i2c.h"



#define OLED_SSD1306_I2C_ADDR   0x78

#define OLED_COLUMN             128
#define OLED_PAGE               8
#define OLED_ROWS               8*OLED_PAGE  

extern uint8_t OLED_GRAM[OLED_PAGE][OLED_COLUMN];

typedef enum {
  OLED_COLOR_NORMAL = 0, // 正常模式 黑底白字
  OLED_COLOR_REVERSED    // 反色模式 白底黑字
} OLED_ColorMode;


void OLED_SendCmd(uint8_t cmd);
void OLED_SendData(uint8_t data);


void OLED_Init();
void OLED_SetColorMode(OLED_ColorMode mode);

void OLED_NewFrame();
void OLED_SetCursor(uint8_t X ,uint8_t Page);
void OLED_ShowFrame();
void OLED_Clear();

void OLED_PrintChar(uint8_t x,uint8_t y,char ch,uint8_t fontsize);
void OLED_PrintString(uint8_t x ,uint8_t y ,char* string ,uint8_t fontsize);
void OLED_DrawImage(uint8_t x ,uint8_t y ,uint8_t width ,uint8_t height ,const uint8_t* image);
void OLED_PrintChinese(uint8_t x,uint8_t y,char* chinese);

void OLED_ShowNum(uint8_t x , uint8_t y , uint32_t number , uint8_t len , uint8_t fontsize);
void OLED_ShowSignedNum(uint8_t x , uint8_t y , int32_t number , uint8_t len , uint8_t fontsize);
void OLED_ShowHexNum(uint8_t x , uint8_t y , uint32_t number , uint8_t len , uint8_t fontsize);
void OLED_ShowFloatNum(uint8_t x , uint8_t y , double number , uint8_t IntLen , uint8_t FraLen , uint8_t fontsize);


void OLED_DrawPoint(uint8_t x, uint8_t y);
uint8_t OLED_GetPoint(uint8_t x, uint8_t y);
void OLED_DrawLine(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1);

#endif