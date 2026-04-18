#include "OLED_SSD1306.h"
#include "Font.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>



uint8_t OLED_GRAM[OLED_PAGE][OLED_COLUMN];


//=============================================I2C底层驱动===============================================
static HAL_StatusTypeDef OLED_Transmit(uint8_t *pdata, uint16_t size){
    return HAL_I2C_Master_Transmit(&hi2c1, OLED_SSD1306_I2C_ADDR, pdata, size, 100);
}

void OLED_SendCmd(uint8_t cmd){
  uint8_t SendBuffer[2]={0x00, cmd};
  OLED_Transmit(SendBuffer, 2);
}

void OLED_SendData(uint8_t data){
  uint8_t SendBUffer[2]={0x40,data};
  OLED_Transmit(SendBUffer, 2);
}
//============================================OLED底层驱动===============================================

void OLED_Init(){

    HAL_Delay(100);


    OLED_SendCmd(0xAE);	//设置显示开启/关闭，0xAE关闭，0xAF开启

    OLED_SendCmd(0xD5);	//设置显示时钟分频比/振荡器频率
    OLED_SendCmd(0x80);	//0x00~0xFF
    
    OLED_SendCmd(0xA8);	//设置多路复用率
    OLED_SendCmd(0x3F);	//0x0E~0x3F

    OLED_SendCmd(0xD3);	//设置显示偏移
    OLED_SendCmd(0x00);	//0x00~0x7F

    OLED_SendCmd(0x40);	//设置显示开始行，0x40~0x7F

    OLED_SendCmd(0xA1);	//设置左右方向，0xA1正常，0xA0左右反置

    OLED_SendCmd(0xC8);	//设置上下方向，0xC8正常，0xC0上下反置

    OLED_SendCmd(0xDA);	//设置COM引脚硬件配置
    OLED_SendCmd(0x12);

    OLED_SendCmd(0x81);//设置对比度
    OLED_SendCmd(0xCF);	//0x00~0xFF

    OLED_SendCmd(0xD9);	//设置预充电周期
    OLED_SendCmd(0xF1);

    OLED_SendCmd(0xDB);	//设置VCOMH取消选择级别
    OLED_SendCmd(0x30);

    OLED_SendCmd(0xA4);	//设置整个显示打开/关闭

    OLED_SendCmd(0xA6);	//设置正常/反色显示，0xA6正常，0xA7反色

    OLED_SendCmd(0x8D);	//设置充电泵
    OLED_SendCmd(0x14);

    HAL_Delay(10);

    OLED_SendCmd(0xAF);//开启OLED显示

    HAL_Delay(100);
}

/**
 * @brief 设置颜色模式 黑底白字或白底黑字
 * @param ColorMode 颜色模式COLOR_NORMAL/COLOR_REVERSED
 * @note 此函数直接设置屏幕的颜色模式
 */
void OLED_SetColorMode(OLED_ColorMode mode)
{
  if (mode == OLED_COLOR_NORMAL)
  {
    OLED_SendCmd(0xA6); // 正常显示
  }
  if (mode == OLED_COLOR_REVERSED)
  {
    OLED_SendCmd(0xA7); // 反色显示
  }
}
//=========================================GRAM显存操作函数====================================


void OLED_SetCursor(uint8_t X ,uint8_t Page){
  OLED_SendCmd(0x00 | (X & 0x0F));
  OLED_SendCmd(0x10 | ((X & 0xF0)>>4));
  OLED_SendCmd(0xB0 | Page);
}


/**
 * @brief 将当前显存显示到屏幕上
 * @note 此函数是移植本驱动时的重要函数 将本驱动库移植到其他驱动芯片时应根据实际情况修改此函数
 */
void OLED_ShowFrame()
{
  static uint8_t sendBuffer[OLED_COLUMN + 1];
  sendBuffer[0] = 0x40;
  for (uint8_t i = 0; i < OLED_PAGE; i++)
  {
    // OLED_SendCmd(0xB0 + i); // 设置页地址
    // OLED_SendCmd(0x00);     // 设置列地址低4位
    // OLED_SendCmd(0x10);     // 设置列地址高4位
    OLED_SetCursor(0, i);
    memcpy(sendBuffer + 1, OLED_GRAM[i], OLED_COLUMN);
    OLED_Transmit(sendBuffer, OLED_COLUMN + 1);
  }
}


// ==========================================打印文字函数========================================

// void OLED_Clear(){
//   for (uint8_t j = 0 ; j < 8; j++) {
//     OLED_SetCursor(0, j);
//     for(uint8_t i = 0; i < 128 ;i++){
//       OLED_SendData(0x00);
//     }

//   }
// }

void OLED_PrintChar(uint8_t x,uint8_t y,char ch,uint8_t fontsize){
  if (fontsize == 6) {
    OLED_DrawImage(x, y, 6, 8, OLED_F6x8[ch - ' ']);
  }else if (fontsize == 8) {
    OLED_DrawImage(x, y, 8, 16, OLED_F8x16[ch - ' ']);
  }
}

void OLED_PrintString(uint8_t x ,uint8_t y ,char* string ,uint8_t fontsize){
  for (uint8_t i = 0 ; string[i] != '\0' ; i++) {
    OLED_PrintChar(x+i*fontsize, y, string[i], fontsize);
  }
}

void OLED_DrawImage(uint8_t x ,uint8_t y ,uint8_t width ,uint8_t height ,const uint8_t* image){
  for (uint8_t j = 0; j < (height - 1)/8 + 1; j++) {
    for (uint8_t i = 0; i < width; i++) {
      OLED_GRAM[y / 8 + j][x + i] |= (image[i + j * width] << (y % 8));
      OLED_GRAM[y / 8 + 1 + j][x + i] |= (image[i + j * width] >> (8 - (y % 8)));
    }
  }

}


void OLED_PrintChinese(uint8_t x,uint8_t y,char* chinese){
  char singlechinese[4]={0};
  uint8_t pchinese = 0;
  uint8_t pindex = 0;
  for (uint8_t i = 0; chinese[i] != '\0'; i++) {
    singlechinese[pchinese] = chinese[i];
    pchinese ++;
    if (pchinese >= 3) {
      pchinese = 0;

      for (pindex = 0; strcmp(OLED_CF16x16[pindex].Index, "") != 0; pindex++) {
        if (strcmp(OLED_CF16x16[pindex].Index, singlechinese) == 0) {
          break;
        }
      }

      OLED_DrawImage(x+((i+1)/3-1)*16, y, 16, 16,OLED_CF16x16[pindex].Data);

    }

  }
}


/**
 * @brief 清空显存 绘制新的一帧
 */
void OLED_NewFrame()
{
  memset(OLED_GRAM, 0x00, sizeof(OLED_GRAM));
}

//========================================打印数字函数===================================

/**
  * 函    数：次方函数
  * 参    数：X 底数
  * 参    数：Y 指数
  * 返 回 值：等于X的Y次方
  */
static uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默认为1
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}


void OLED_ShowNum(uint8_t x , uint8_t y , uint32_t number , uint8_t len , uint8_t fontsize){
  for (uint8_t i = 0;  i < len; i++) {
    OLED_PrintChar(x+i*fontsize, y,number / OLED_Pow(10, len - i - 1) % 10 + '0', fontsize);
  }
}

void OLED_ShowSignedNum(uint8_t x , uint8_t y , int32_t number , uint8_t len , uint8_t fontsize){
    uint32_t Number1;  
  if (number >= 0) {
    OLED_PrintChar(x, y, '+', fontsize);
    Number1 = number;
  }else {
    OLED_PrintChar(x, y, '-', fontsize);
    Number1 = -number;
  }
  OLED_ShowNum(x+fontsize, y, Number1, len, fontsize);

}

void OLED_ShowHexNum(uint8_t x , uint8_t y , uint32_t number , uint8_t len , uint8_t fontsize){
  uint8_t SingleNumber;
  for (uint8_t i = 0; i < len; i++) {
    SingleNumber = number / OLED_Pow(16, len - 1 - i) % 16;
    if (SingleNumber < 10) {
      OLED_PrintChar(x + i * fontsize, y, SingleNumber + '0', fontsize);
    }else {
    OLED_PrintChar(x + i * fontsize, y, SingleNumber - 10 + 'A', fontsize);
    }
  }

}

void OLED_ShowFloatNum(uint8_t x , uint8_t y , double number , uint8_t IntLen , uint8_t FraLen , uint8_t fontsize){
  uint32_t Temp;

  if (number >= 0) {
    OLED_PrintChar(x, y, '+', fontsize);
  }else {
    OLED_PrintChar(x, y, '-', fontsize);
    number = -number;
  }

  OLED_ShowNum(x + fontsize, y, number, IntLen, fontsize);
  OLED_PrintChar(x + (IntLen + 1) *fontsize, y, '.', fontsize);

  number -= (uint32_t)number;
  Temp = OLED_Pow(10, FraLen); 

  OLED_ShowNum(x + (IntLen + 2)*fontsize, y, ((uint32_t)(number * Temp))%Temp, FraLen, fontsize);


}


//=========================================绘图函数=====================================
void OLED_DrawPoint(uint8_t x, uint8_t y){
  if (x >= OLED_COLUMN || y >= OLED_ROWS) {return;}
  OLED_GRAM[y / 8][x] |= 0x01 << (y % 8);
}


uint8_t OLED_GetPoint(uint8_t x, uint8_t y){
  if (x >= OLED_COLUMN || y >= OLED_ROWS) {return 0;}
  if (OLED_GRAM[y / 8][x] & 0x01 << (y % 8)) {
    return 1;
  }
  return 0;
}

/**
  * 函    数：OLED画线
  * 参    数：X0 指定一个端点的横坐标，范围：0~127
  * 参    数：Y0 指定一个端点的纵坐标，范围：0~63
  * 参    数：X1 指定另一个端点的横坐标，范围：0~127
  * 参    数：Y1 指定另一个端点的纵坐标，范围：0~63
  * 返 回 值：无
  * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawLine(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1)
{
	int16_t x, y, dx, dy, d, incrE, incrNE, temp;
	int16_t x0 = X0, y0 = Y0, x1 = X1, y1 = Y1;
	uint8_t yflag = 0, xyflag = 0;
	
	if (y0 == y1)		//横线单独处理
	{
		/*0号点X坐标大于1号点X坐标，则交换两点X坐标*/
		if (x0 > x1) {temp = x0; x0 = x1; x1 = temp;}
		
		/*遍历X坐标*/
		for (x = x0; x <= x1; x ++)
		{
			OLED_DrawPoint(x, y0);	//依次画点
		}
	}
	else if (x0 == x1)	//竖线单独处理
	{
		/*0号点Y坐标大于1号点Y坐标，则交换两点Y坐标*/
		if (y0 > y1) {temp = y0; y0 = y1; y1 = temp;}
		
		/*遍历Y坐标*/
		for (y = y0; y <= y1; y ++)
		{
			OLED_DrawPoint(x0, y);	//依次画点
		}
	}
	else				//斜线
	{
		/*使用Bresenham算法画直线，可以避免耗时的浮点运算，效率更高*/
		/*参考文档：https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
		/*参考教程：https://www.bilibili.com/video/BV1364y1d7Lo*/
		
		if (x0 > x1)	//0号点X坐标大于1号点X坐标
		{
			/*交换两点坐标*/
			/*交换后不影响画线，但是画线方向由第一、二、三、四象限变为第一、四象限*/
			temp = x0; x0 = x1; x1 = temp;
			temp = y0; y0 = y1; y1 = temp;
		}
		
		if (y0 > y1)	//0号点Y坐标大于1号点Y坐标
		{
			/*将Y坐标取负*/
			/*取负后影响画线，但是画线方向由第一、四象限变为第一象限*/
			y0 = -y0;
			y1 = -y1;
			
			/*置标志位yflag，记住当前变换，在后续实际画线时，再将坐标换回来*/
			yflag = 1;
		}
		
		if (y1 - y0 > x1 - x0)	//画线斜率大于1
		{
			/*将X坐标与Y坐标互换*/
			/*互换后影响画线，但是画线方向由第一象限0~90度范围变为第一象限0~45度范围*/
			temp = x0; x0 = y0; y0 = temp;
			temp = x1; x1 = y1; y1 = temp;
			
			/*置标志位xyflag，记住当前变换，在后续实际画线时，再将坐标换回来*/
			xyflag = 1;
		}
		
		/*以下为Bresenham算法画直线*/
		/*算法要求，画线方向必须为第一象限0~45度范围*/
		dx = x1 - x0;
		dy = y1 - y0;
		incrE = 2 * dy;
		incrNE = 2 * (dy - dx);
		d = 2 * dy - dx;
		x = x0;
		y = y0;
		
		/*画起始点，同时判断标志位，将坐标换回来*/
		if (yflag && xyflag){OLED_DrawPoint(y, -x);}
		else if (yflag)		{OLED_DrawPoint(x, -y);}
		else if (xyflag)	{OLED_DrawPoint(y, x);}
		else				{OLED_DrawPoint(x, y);}
		
		while (x < x1)		//遍历X轴的每个点
		{
			x ++;
			if (d < 0)		//下一个点在当前点东方
			{
				d += incrE;
			}
			else			//下一个点在当前点东北方
			{
				y ++;
				d += incrNE;
			}
			
			/*画每一个点，同时判断标志位，将坐标换回来*/
			if (yflag && xyflag){OLED_DrawPoint(y, -x);}
			else if (yflag)		{OLED_DrawPoint(x, -y);}
			else if (xyflag)	{OLED_DrawPoint(y, x);}
			else				{OLED_DrawPoint(x, y);}
		}	
	}
}
