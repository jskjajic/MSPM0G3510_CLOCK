#ifndef OLED_H
#define OLED_H

#include "ti_msp_dl_config.h"

#define u8 unsigned char
#define u32 unsigned int
#define sysosc 32000000

void OLED_Set_Pos(unsigned char x, unsigned char y);//位置设置函数
void OLED_WR_Byte(u8 dat, u8 cmd);//字节写入函数
void OLED_ShowChar(u8 x, u8 y, char chr);//显示字母
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowString(u8 x, u8 y, char *chr);
//void ch_op(char*ch1,char key_ch[],int len,int sign);//拼接所要显示的字符
//void ch_opx(char*ch1,char key_ch[],int len);//拼接所要显示的字符
//void ch_opy(char*ch1,char*ch2,char*ch3,int len3);//拼接所要显示的字符
void num_to_ch(int num,char *num_ch,int len);
//int  count(int y);
//char keybord_check(void);
void OLED_DrawPoint(u8 x, u8 y, u8 state);
//int  pwm_duty_read(GPIO_Regs* pin_port,int pin,int i);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2);
//u32 oled_pow(u8 m, u8 n);
//void OLED_show_float(uint8_t x,uint8_t y,float num,int little_len);
#endif
