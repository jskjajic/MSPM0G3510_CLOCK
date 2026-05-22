#include <ti/eeprom/emulation_type_a/eeprom_emulation_type_a.h> 
#include "ti_msp_dl_config.h"
#include "bsp.h"

#define set_time_key_select_up     'A'
#define set_time_key_select_down   'B'
#define set_time_key_board_push_add 'C'
#define set_time_key_board_push_reduce 'D'
#define FLASHADDRESS 0x00008000
#define  FLASH_UPDATING_TIME 60

uint16_t month_day_total[12]={0,31,59,90,120,151,181,212,243,273,304,334};

void uart_stop(void);
void pwm_buzzer(void);//闹钟响应，pwm驱动buzzer
void disable_pwm_buzzer(void);//闹钟响应结束，关闭pwm驱动buzzer
void set_clock_light(void);//闹钟设置模式

void OLED_ShowDate(uint8_t x,uint8_t y,uint16_t year,uint8_t month,uint8_t day);//显示年、月、日和时间
void OLED_ShowTime(uint8_t x,uint8_t y,uint8_t count_model);//显示计时模式和时分秒
void year_month_day_deal(void);

void sw_time_fast_model_deal(void);
void sw_time_very_fast_model_deal(void);
void sw_time_normal_model_deal(void);

void set_time_light(void);//设置时间高亮
void scan_keyboard(void);//按键扫描函数

void timer_start(void);//计时器开启
void timer_stop(void);//计时器关闭

void uart_start(void);//uart启动
void uart_receive_data(uint32_t *date);//uart接收数据

uint32_t*p=NULL;
uint32_t*q=NULL;

uint8_t  uart_stop_sign=0;
uint32_t uart_sign[8]={0};//接收数据标签
uint32_t uart_sign_count=0;//接收的数据标签实时变化长度
uint32_t uart_sign_len=0;//记录数据标签的最终长度
uint32_t uart_sign_end=0;//接收数据标签结束标志

uint32_t uart_data[8]={0};//接收数据
uint32_t uart_data_count=0;//接收的数据标签实时变化长度
uint32_t uart_data_len=0;//记录数据标签的最终长度
uint32_t uart_data_end=0;//接收数据标签结束标志

uint32_t uart_year_sign[]={'y','e','a','r','\n'};//数据标签
uint32_t uart_month_sign[]={'m','o','n','t','h','\n'};
uint32_t uart_day_sign[]={'d','a','y','\n'};
uint32_t uart_hour_sign[]={'h','o','u','r','\n'};
uint32_t uart_min_sign[]={'m','i','n','\n'};
uint32_t uart_second_sign[]={'s','e','c','o','n','d','\n'};

uint32_t uart_sign_check_exit=0;
uint32_t uart_sign_check[6]={0};//标志哪一个数据标签正确接收
uint32_t uart_data_sign=0;

uint8_t timer_start_sign=0;//计时器开启标志
uint8_t timer_start_one=0;//计时器开启标志
char    timer_key_press='C';//标志计时器按钮
uint32_t timer_count=0;
char timer_count_ch[5];

volatile uint32_t second=0;

volatile uint8_t one_second_normal=0;//正常计时标志
volatile uint8_t one_second_fast=0;//快速计时标志
volatile uint8_t one_second_very_fast=0;//极速计时标志

volatile uint8_t sw_time_fast=0;//拨码开关切换到快速计时模式
volatile uint8_t sw_time_fast_start=0;//拨码开关切换到快速计时模式

volatile uint8_t sw_time_very_fast=0;//拨码开关切换到极速计时模式
volatile uint8_t sw_time_very_fast_start=0;//拨码开关切换到极速计时模式

volatile uint8_t sw_time_normal_start=0;//拨码开关切换到正常计时模式

volatile uint8_t count_model=1;//计时模式标志
volatile uint8_t old_count_model=1;//记录计时模式

volatile uint32_t base_year=2025;//基准年
volatile uint32_t base_month=12;//基准月
volatile uint32_t base_day=31;//基准日
volatile uint32_t base_weekday=3;//基准星期

uint32_t now_year=2026;//年
uint32_t now_month=2;//月
uint32_t now_day=17;//日

volatile uint32_t old_now_year=2025;//记录年
volatile uint32_t old_now_month=12;//记录月
volatile uint32_t old_now_day=31;//记录日

uint32_t time_hour=0;//时
uint32_t time_min=0;//分
uint32_t time_second=0;//秒
 
volatile uint8_t sw_set_time=0;//拨码开关切换到设置时间模式
volatile uint8_t sw_set_time_start=0;//拨码开关切换到设置时间模式

volatile uint8_t sw_set_time_select=1;//选择需要修改的时间参数
volatile uint8_t old_sw_set_time_select=1;//记录选择需要修改的时间参数

unsigned char key_board[16]={'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};

int Keybord_out[]={Keybord_out_PIN_H1_PIN,Keybord_out_PIN_H2_PIN,Keybord_out_PIN_H3_PIN,Keybord_out_PIN_H4_PIN};
int Keybord_in[]={Keybord_in_PIN_V1_PIN,Keybord_in_PIN_V2_PIN,Keybord_in_PIN_V3_PIN,Keybord_in_PIN_V4_PIN};

GPIO_Regs* sw_gpio[]={SW_SW1_PORT,SW_SW2_PORT,SW_SW3_PORT};
uint32_t   sw_pins[]={SW_SW1_PIN,SW_SW2_PIN,SW_SW3_PIN};

uint32_t flash_save_time[6]={0}; //存储年月日时分秒
uint8_t flash_save_overtime=0;  //存储时间计时更新


// 修复：新增按键扫描辅助变量
static uint8_t key_prev_state[16] = {0};
static uint8_t key_press_event[16] = {0};

uint32_t flash_address= FLASHADDRESS;
uint8_t  flash_erase=1;

uint8_t flash_read_time=0;//上电后先读取，然后不再触发
uint32_t flash_read_data=0;
uint8_t flash_count=0;
uint8_t sw3_done=0;//校时后更新到flash
uint8_t sw_clear_flash=0;//flash清空
uint8_t sw_time_to_12=0;//切换为12小时制
uint8_t sw_change_update_oled=0;//切换为小时制，更新oled

uint8_t sw_set_clock=0;//设置闹钟

uint32_t clock_year[3]={2023,2024,2025};
uint32_t clock_month[3]={7,8,9};
uint32_t clock_day[3]={18,19,20};
uint32_t clock_hour[3]={6,7,8};
uint32_t clock_min[3]={16,18,23};
uint32_t clock_second[3]={12,18,26};
uint32_t set_clock_select=1;//闹钟参数索引

uint32_t clock_second_1=0;
uint32_t clock_second_2=0;
uint32_t clock_second_3=0;

uint8_t pwm_buzzer_start=0;//开启或关闭buzzer
uint8_t pwm_buzzer_stop=1;//开启或关闭buzzer
uint16_t pwm_buzzer_count=0;

uint8_t clock_oled_update=0;//闹钟模式下，按键按下就刷新一次
uint8_t sw6_update=0;//闹钟模式切换，刷新oled
uint8_t sw3_update=0;//校时模式切换，刷新oled
uint8_t sw7_update=0;//计时模式切换，刷新oled

int main()
  {
		
   SYSCFG_DL_init();
	 OLED_Init();
	 uart_start();
		
	 NVIC_ClearPendingIRQ(SW_GPIOA_INT_IRQN); 
	 NVIC_EnableIRQ(SW_GPIOA_INT_IRQN);
	 
   NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN); 
	 NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);  
	
	 disable_pwm_buzzer();
		
	 while(1)
	 {	
  
		 if(uart_stop_sign)
		 {
			 uart_start();
			 uart_stop_sign=0;
		 }  
		 
		  if(sw6_update)
			{
				OLED_Clear();
				sw6_update=0;
			}
			if(sw3_update)
			{
				OLED_Clear();
				sw3_update=0;
			}
			if(sw7_update)
			{
				OLED_Clear();
				sw7_update=0;
			}
			
		  uint8_t x_x=0;
		  if(!flash_read_time)
			{
				while(flash_count<21&&(flash_read_data<0xFFFFFFFF))//一个扇区最大写入次数为21次
				{
					int i=0;
					for(;i<6;i++)
				  { 
						x_x=i;
						DL_FlashCTL_unprotectSector(FLASHCTL, FLASHADDRESS , DL_FLASHCTL_REGION_SELECT_MAIN);
            flash_read_data=*(volatile uint32_t *)(FLASHADDRESS+(flash_count*8*6)+i*8);//FLASHADDRESS+(flash_count*4*6)每一组的起始地址
						if(flash_read_data>=0xFFFFFFFF)//0x FF FF FF FF是无效数据
						break;					
					}
					if(i>=6)
					flash_count++;
				}
			
			DL_FlashCTL_protectSector(FLASHCTL,FLASHADDRESS,DL_FLASHCTL_REGION_SELECT_MAIN);
				
			if((flash_count<1)||((flash_count==1)&&(x_x==6)))//无有效记录
			{}
			else	
			{
				flash_count--;//读取存储的数据
				for(int i=0;i<6;i++)
				{
					switch(i)
					{
						case 0:
						{
								DL_FlashCTL_unprotectSector(FLASHCTL, FLASHADDRESS , DL_FLASHCTL_REGION_SELECT_MAIN);
								now_year=*(volatile uint32_t *)(FLASHADDRESS+(flash_count*8*6)+i*8);
							  if(now_year>=1)
								{
								  now_year=now_year%10000;
								}
								else
                {
								  now_year=base_year;
								}									
								break;
						}
						
						case 1:
						{
								DL_FlashCTL_unprotectSector(FLASHCTL, FLASHADDRESS , DL_FLASHCTL_REGION_SELECT_MAIN);
								now_month=*(volatile uint32_t *)(FLASHADDRESS+(flash_count*8*6)+i*8);
							  if((now_month>=1)&&(now_month<=12))
								{}
								else
								{
								  now_month=base_month;
								}
								break;
						}
						
						case 2:
						{
								DL_FlashCTL_unprotectSector(FLASHCTL, FLASHADDRESS , DL_FLASHCTL_REGION_SELECT_MAIN);
								now_day=*(volatile uint32_t *)(FLASHADDRESS+(flash_count*8*6)+i*8);
							  switch(now_month)   
								{
								  case 1:
									case 3:
									case 5:
									case 7:
									case 8:
									case 10:
									case 12:
									{
									  if((now_day>=1)&&(now_day<=31))
										{}
										else
										{
										  now_day=base_day;
										}
										break;
									}
									case 4:
									case 6:
									case 9:
									case 11:
									{
										if((now_day>=1)&&(now_day<=30))
										{}
										else
										{
										  now_day=base_day;
										}
									  break;
									}
									case 2:
									{
										if(((now_year%4==0)&&(now_year%100!=0))||(now_year%400==0))
										{
										   if((now_day>=1)&&(now_day<=29))
											 {}
											 else
											 {
											   now_day=base_day;
											 }
										}
										else
										{
											if((now_day>=1)&&(now_day<=28))
											 {}
											 else
											 {
											   now_day=base_day;
											 }
										}
									  break;
									}
								}
								break;
						}
						
						case 3:
						{
								DL_FlashCTL_unprotectSector(FLASHCTL, FLASHADDRESS , DL_FLASHCTL_REGION_SELECT_MAIN);
								time_hour=*(volatile uint32_t *)(FLASHADDRESS+(flash_count*8*6)+i*8);
							  if((time_hour>=0)&&(time_hour<=23))
								{}
								else
								{
								  time_hour=second/3600;
								}
								break;
						}
						
						case 4:
						{
								DL_FlashCTL_unprotectSector(FLASHCTL, FLASHADDRESS , DL_FLASHCTL_REGION_SELECT_MAIN);
								time_min=*(volatile uint32_t *)(FLASHADDRESS+(flash_count*8*6)+i*8);
							  if((time_min>=0)&&(time_min<=59))
								{}
								else
								{
								  time_min=(second-time_hour*3600)%60;
								}
								break;
						}
						
						case 5:
						{
								DL_FlashCTL_unprotectSector(FLASHCTL, FLASHADDRESS , DL_FLASHCTL_REGION_SELECT_MAIN);
								time_second=*(volatile uint32_t *)(FLASHADDRESS+(flash_count*8*6)+i*8);
							  if((time_second>=0)&&(time_second<=59))
								{}
								else
								{
								  time_second=second%60;
								}
								break;
						}
						
						default:break;
								
					}
				}
				
				second=time_hour*3600+time_min*60+time_second;
				
			}
			
			  flash_read_time=1;
			}
			
			if((!flash_save_overtime)||(sw3_done&&(!sw_set_time))||(sw_clear_flash))//一分钟就自动保存一次或者校时一次就更新
			{
				sw3_done=0;//重置校时标志
				if(flash_erase||sw_clear_flash)
				{
					sw_clear_flash=0; 
					flash_erase=0;
					flash_address= FLASHADDRESS;
					DL_FlashCTL_unprotectSector(FLASHCTL,FLASHADDRESS,DL_FLASHCTL_REGION_SELECT_MAIN);
				  DL_FlashCTL_eraseMemoryFromRAM(FLASHCTL,FLASHADDRESS,DL_FLASHCTL_COMMAND_SIZE_SECTOR);
				}
						
				if((flash_address+8*6)>=FLASHADDRESS+1024)
				{
				   flash_erase=1;
				}
				else
				{	
					 time_hour=second/3600;
	         time_min=(second-time_hour*3600)/60;
	         time_second=second%60; 
					
					 flash_save_time[0]=now_year; //存储时间
					 flash_save_time[1]=now_month;
					 flash_save_time[2]=now_day;
					 flash_save_time[3]=time_hour;
					 flash_save_time[4]=time_min;
					 flash_save_time[5]=time_second;
					
					 for(int k=0;k<6;k++)
					 {
							DL_FlashCTL_unprotectSector(FLASHCTL,FLASHADDRESS,DL_FLASHCTL_REGION_SELECT_MAIN);
 						  DL_FlashCTL_programMemoryFromRAM32WithECCGenerated(FLASHCTL,flash_address,(uint32_t*)&flash_save_time[k]);
						  flash_address+=0x00000008;
					 }
					 
					 DL_FlashCTL_protectSector(FLASHCTL,FLASHADDRESS,DL_FLASHCTL_REGION_SELECT_MAIN);
			   }
			 }
			 
			 uint32_t current_hour=second/3600;
			 uint32_t current_min=(second-current_hour*3600)/60;
			 uint32_t current_second=second%60;
			 
			 if(!sw_time_to_12)//24小时制
			 {
				 if((clock_hour[0]==current_hour)&&(clock_min[0]==current_min)&&(clock_second[0]==current_second)&&(clock_year[0]==now_year)&&(clock_month[0]==now_month)&&(clock_day[0]==now_day))
				 {
					 pwm_buzzer_start=1; 
				 }
				 	 
			 if((clock_hour[1]==current_hour)&&(clock_min[1]==current_min)&&(clock_second[1]==current_second)&&(clock_year[1]==now_year)&&(clock_month[1]==now_month)&&(clock_day[1]==now_day))
				 {
				   pwm_buzzer_start=1;
				 }
				 
			 if((clock_hour[2]==current_hour)&&(clock_min[2]==current_min)&&(clock_second[2]==current_second)&&(clock_year[2]==now_year)&&(clock_month[2]==now_month)&&(clock_day[2]==now_day))
				 {
				   pwm_buzzer_start=1;
				 }
				 
			 }
			 else//12小时制
			 {
				 if(clock_second_1>=13*3600)//12小时制的下午
			   {
					 if(((clock_hour[0]+12)==current_second)&&(clock_min[0]==current_min)&&(clock_second[0]==current_second)&&(clock_year[0]==now_year)&&(clock_month[0]==now_month)&&(clock_day[0]==now_day))
					 {
						 pwm_buzzer_start=1;
					 }
				 }
				 else//12小时制的上午
				 {
				    if((clock_hour[0]==current_hour)&&(clock_min[0]==current_min)&&(clock_second[0]==current_second)&&(clock_year[0]==now_year)&&(clock_month[0]==now_month)&&(clock_day[0]==now_day))
					  {
						  pwm_buzzer_start=1;
					  }
				 }
				 
				 
				 if(clock_second_2>=13*3600)//12小时制的下午
				 {
					 if(((clock_hour[1]+12)==current_hour)&&(clock_min[1]==current_min)&&(clock_second[1]==current_second)&&(clock_year[1]==now_year)&&(clock_month[1]==now_month)&&(clock_day[1]==now_day))//12小时制的下午
				   {
				    pwm_buzzer_start=1;
				   }
			   }
				 else//12小时制的上午
				 {
				    if((clock_hour[1]==current_hour)&&(clock_min[1]==current_min)&&(clock_second[1]==current_second)&&(clock_year[1]==now_year)&&(clock_month[1]==now_month)&&(clock_day[1]==now_day))
					  {
						  pwm_buzzer_start=1;
					  }
				 }
				 
				 
				 if(clock_second_3>=13*3600)//12小时制的下午
				 {
						 if(((clock_hour[2]+12)==current_hour)&&(clock_min[2]==current_min)&&(clock_second[2]==current_second)&&(clock_year[2]==now_year)&&(clock_month[2]==now_month)&&(clock_day[2]==now_day))//12小时制的下午
					 {
							pwm_buzzer_start=1;
					 }
				 }
				 else//12小时制的上午
				 {
				    if((clock_hour[2]==current_hour)&&(clock_min[2]==current_min)&&(clock_second[2]==current_second)&&(clock_year[2]==now_year)&&(clock_month[2]==now_month)&&(clock_day[2]==now_day))
					  {
						  pwm_buzzer_start=1;
					  }
				 }
				 
			 }
			 
				 
				 if(pwm_buzzer_start)//启动buzzer
				 {
					 if(pwm_buzzer_stop)
					 pwm_buzzer();
					 pwm_buzzer_stop=0;
				 }
				 else
				 {
					 if(!pwm_buzzer_stop)
					 disable_pwm_buzzer();//关闭buzzer
					 pwm_buzzer_stop=1;
				 }
			 
		   old_now_year=now_year;
			 old_now_month=now_month;
			 old_now_day=now_day;
			 old_count_model=count_model;
			 
				 
		   if(sw_set_time&&(!sw_set_clock)&&(!timer_start_sign))//设置时间年、月、日、时、分、秒
			 {
				 if(!sw_set_time_start)
				 {
//					 DL_TimerA_stopCounter(TIMER_1_INST);	
//		       DL_TimerA_clearInterruptStatus(TIMER_1_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);
//		       DL_TimerA_disableInterrupt(TIMER_1_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);	
//		       NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
//	         NVIC_DisableIRQ(TIMER_1_INST_INT_IRQN);
//					 delay_cycles(sysosc*0.002); 
//					 
//					 DL_TimerG_stopCounter(TIMER_2_INST);	
//		       DL_TimerG_clearInterruptStatus(TIMER_2_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);
//		       DL_TimerG_disableInterrupt(TIMER_2_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);	
//		       NVIC_ClearPendingIRQ(TIMER_2_INST_INT_IRQN);
//	         NVIC_DisableIRQ(TIMER_2_INST_INT_IRQN);
//					 delay_cycles(sysosc*0.002);
//					 
//					 DL_TimerA_stopCounter(TIMER_0_INST);	
//		       DL_TimerA_clearInterruptStatus(TIMER_0_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);
//		       DL_TimerA_disableInterrupt(TIMER_0_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);	
//		       NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
//	         NVIC_DisableIRQ(TIMER_0_INST_INT_IRQN);
//					 delay_cycles(sysosc*0.002);
					 
					 sw_set_time_start=1;
				 }
				 
		     scan_keyboard();		
				 
				 clock_oled_update=0;
				 for(int i=0;i<4;i++)
				 {
					 if(key_press_event[3+4*i])
					 {
						 clock_oled_update=1;
						 break;
					 }
				 }
				 
				 if(key_press_event[3]) //A键
				{
					if(sw_set_time_select>1)
					sw_set_time_select--;
					else
					sw_set_time_select=6;
					
					sw_set_time_select%=7;
				}
				else if(key_press_event[3+4]) //B键
				{
					if(sw_set_time_select<6)
					sw_set_time_select++;
					else
				  sw_set_time_select=1;
					sw_set_time_select%=7;
				}
				else if(key_press_event[3+4*2]) //C键
				{
					switch(sw_set_time_select)
					{
						case 1:now_year++;break;
						case 2:
						{
							if((now_month<=11)&&(now_month>=1))
							{
								 now_month++;
							}
							else if(now_month==12)
							{
							   now_month=1;
							}
							break;
						}
						case 3:
						{
							switch(now_month)
							{
								case 1:
								case 3:
								case 5:
								case 7:
								case 8:
								case 10:
								case 12:
							  {
									if((now_day<=30)&&(now_day>=1))
									now_day++;
									else if(now_day==31)
									{
										now_day=1;
									}
									break;
								}
								case 4:
								case 6:
								case 9:
								case 11:
								{
								  if((now_day<=29)&&(now_day>=1))
									now_day++;
									else if(now_day==30)
									{
										now_day=1;
									}
									break;
								}
								case 2:
								{
								  if(((now_year%4==0)&&(now_year%100!=0))||(now_year%400==0))
									{
									  if((now_day<=28)&&(now_day>=1))
											now_day++;
										else if(now_day==29)
											now_day=1;
									}
									else
									{
									  if((now_day<=27)&&(now_day>=1))
											now_day++;
										else if(now_day==28)
											now_day=1;
									}
									break;
								}
							}
							break;
						}
						case 4:second=(second+3600)%86400;break;
						case 5:second=(second+60)%86400;break;
						case 6:second=(second+1)%86400;break;
						default:break;
					}					
				}
				else if(key_press_event[3+4*3]) //D键
				{
					 switch(sw_set_time_select)
					{
						case 1:now_year--;break;
						case 2:
						{
							if((now_month>=2)&&(now_month<=12))
							now_month--;
							else if(now_month==1)
							now_month=12;
							break;
						}
						case 3:
						{
							switch (now_month)
							{	
								case 1:
								case 3:
								case 5:
								case 7:
								case 8:
								case 10:
								case 12:
								{
								  if((now_day<=31)&&(now_day>=2))
									 now_day--;
									else if(now_day==1)
										now_day=31;
									break;
								}
								case 4:
								case 6:
								case 9:
								case 11:
								{
								  if((now_day<=30)&&(now_day>=2))
								  now_day--;
									else if(now_day==1)
									now_day=30;
							    break;
								}
								
								case 2:
							  {
									if(((now_year%4==0)&&(now_year%100!=0))||(now_year%400==0))
									{
									  if((now_day<=29)&&(now_day>=2))
											now_day--;
										else if(now_day==1)
											now_day=29;
									}
									else
									{
									  if((now_day<=28)&&(now_day>=2))
											now_day--;
										else if(now_day==1)
											now_day=28;
									}
									break;
								}
							}
							break;
						}
						case 4:
						{
							if(second>=3600)
							second=(second-3600)%86400;
							else
							{
							  second=(86400-(3600-second))%86400;
							}
							break;
						}
						case 5:
						{
							if(second>=60)
							second=(second-60)%86400;
							else
							{
							  second=(86400-(60-second))%86400;
							}
							break;
						}
						case 6:
						{
							if(second>=1)
							second=(second-1)%86400;
							else
							{
							  second=(86400-(1-second))%86400;
							}
							break;
						}
						default:break;
					}				 
				}
				
				else if(key_press_event[2]) //3键,now_year+10
				{
				  if(sw_set_time_select==1)
					{
					  now_year+=10;
					}
				}
				else if(key_press_event[6]) //6键,now_year-10
				{
				   if(sw_set_time_select==1)
					{
					  now_year-=10;
					}
				}
     		else if(key_press_event[10]) //9键,now_year+100
				{
				   if(sw_set_time_select==1)
					{
					  now_year+=100;
					}
				}
        else if(key_press_event[14]) //#键,now_year-100
				{
				   if(sw_set_time_select==1)
					{
					  now_year-=100;
					}
				}				
				
				   if(clock_oled_update)
				   OLED_Clear();
				   set_time_light();//时间选择模式的画面显示
				   delay_cycles(sysosc*0.002);
			}
			 
       else if((!sw_set_clock)&&(!sw_set_time)&&(!timer_start_sign))//正常计时模式
			 {
					 if(sw_set_time_start)
					 {
							sw_set_time_start=0;
							OLED_Clear();					 
					 }
					 
					 if(sw_time_fast)
					 {
						 sw_time_fast_start=0;
						 sw_time_fast_model_deal();
					 }
					 else if(sw_time_very_fast)
					 {
						 sw_time_very_fast_start=0;
						 sw_time_very_fast_model_deal();
					 }
					 else if((!sw_time_fast)&&(!sw_time_very_fast))
					 {
						 sw_time_normal_start=0;
						 sw_time_normal_model_deal(); 			    
					 }
								 
				 second++;//秒计时加一
				 year_month_day_deal();//一天结束后，更新年、月、日
				 if((sw_change_update_oled)||(old_count_model!=count_model)||((old_now_year!=now_year)||(old_now_month!=now_month)||(old_now_day!=now_day)))
					{
						 sw_change_update_oled=0;
						 OLED_Clear();
					}
				 OLED_ShowDate(0,0,now_year,now_month,now_day);//显示年、月、日和星期
				 OLED_ShowTime(0,4,count_model);//显示计时模式和时分秒
			}
					 
			else if(sw_set_clock&&(!sw_set_time)&&(!timer_start_sign))//设置闹钟
			{
					 if(!uart_stop_sign)
				   {
						 uart_stop();
						 uart_stop_sign=1;
				   }
					 
					 scan_keyboard();	
            
				   clock_oled_update=0;
					 for(int i=0;i<4;i++)
					 {
						 if(key_press_event[3+4*i])
						 {
							 clock_oled_update=1;
							 break;
						 }
					 }
				
						if(key_press_event[3]) //A键
						{
							if(set_clock_select>1)
							set_clock_select--;
							else
							set_clock_select=18;
						}
						else if(key_press_event[3+4]) //B键
						{
							if(set_clock_select<18)
							set_clock_select++;
							else
							set_clock_select=1;
						}
						else if(key_press_event[3+4*2]) //C键
						{
							switch(set_clock_select)
							{
								case 1:clock_year[0]++;break;
								case 2:
								{
									if((clock_month[0]<=11)&&(clock_month[0]>=1))
									{
										 clock_month[0]++;
									}
									else if(clock_month[0]==12)
									{
										 clock_month[0]=1;
									}
									break;
								}
								case 3:
								{
									switch(clock_month[0])
									{
										case 1:
										case 3:
										case 5:
										case 7:
										case 8:
										case 10:
										case 12:
										{
											if((clock_day[0]<=30)&&(clock_day[0]>=1))
											clock_day[0]++;
											else if(clock_day[0]==31)
											{
												clock_day[0]=1;
											}
											break;
										}
										case 4:
										case 6:
										case 9:
										case 11:
										{
											if((clock_day[0]<=29)&&(clock_day[0]>=1))
											clock_day[0]++;
											else if(clock_day[0]==30)
											{
												clock_day[0]=1;
											}
											break;
										}
										case 2:
										{
											if(((clock_year[0]%4==0)&&(clock_year[0]%100!=0))||(clock_year[0]%400==0))
											{
												if((clock_day[0]<=28)&&(clock_day[0]>=1))
													clock_day[0]++;
												else if(clock_day[0]==29)
													clock_day[0]=1;
											}
											else
											{
												if((clock_day[0]<=27)&&(clock_day[0]>=1))
													clock_day[0]++;
												else if(clock_day[0]==28)
													clock_day[0]=1;
											}
											break;
										}
									}
									break;
								}
														
								case 4:clock_second_1=(clock_second_1+3600)%86400;break;
						    case 5:clock_second_1=(clock_second_1+60)%86400;break;
						    case 6:clock_second_1=(clock_second_1+1)%86400;break;
								
								case 7:clock_year[1]++;break;
								case 8:
								{
									if((clock_month[1]<=11)&&(clock_month[1]>=1))
									{
										 clock_month[1]++;
									}
									else if(clock_month[1]==12)
									{
										 clock_month[1]=1;
									}
									break;
								}
								case 9:
								{
									switch(clock_month[1])
									{
										case 1:
										case 3:
										case 5:
										case 7:
										case 8:
										case 10:
										case 12:
										{
											if((clock_day[1]<=30)&&(clock_day[1]>=1))
											clock_day[1]++;
											else if(clock_day[1]==31)
											{
												clock_day[1]=1;
											}
											break;
										}
										case 4:
										case 6:
										case 9:
										case 11:
										{
											if((clock_day[1]<=29)&&(clock_day[1]>=1))
											clock_day[1]++;
											else if(clock_day[1]==30)
											{
												clock_day[1]=1;
											}
											break;
										}
										case 2:
										{
											if(((clock_year[1]%4==0)&&(clock_year[1]%100!=0))||(clock_year[1]%400==0))
											{
												if((clock_day[1]<=28)&&(clock_day[1]>=1))
													clock_day[1]++;
												else if(clock_day[1]==29)
													clock_day[1]=1;
											}
											else
											{
												if((clock_day[1]<=27)&&(clock_day[1]>=1))
													clock_day[1]++;
												else if(clock_day[1]==28)
													clock_day[1]=1;
											}
											break;
										}
									}
									break;
								}
								
															
								case 10:clock_second_2=(clock_second_2+3600)%86400;break;
						    case 11:clock_second_2=(clock_second_2+60)%86400;break;
						    case 12:clock_second_2=(clock_second_2+1)%86400;break;
								
								
								case 13:clock_year[2]++;break;
								case 14:
								{
									if((clock_month[2]<=11)&&(clock_month[2]>=1))
									{
										 clock_month[2]++;
									}
									else if(clock_month[2]==12)
									{
										 clock_month[2]=1;
									}
									break;
								}
								case 15:
								{
									switch(clock_month[2])
									{
										case 1:
										case 3:
										case 5:
										case 7:
										case 8:
										case 10:
										case 12:
										{
											if((clock_day[2]<=30)&&(clock_day[2]>=1))
											clock_day[2]++;
											else if(clock_day[2]==31)
											{
												clock_day[2]=1;
											}
											break;
										}
										case 4:
										case 6:
										case 9:
										case 11:
										{
											if((clock_day[2]<=29)&&(clock_day[2]>=1))
											clock_day[2]++;
											else if(clock_day[2]==30)
											{
												clock_day[2]=1;
											}
											break;
										}
										case 2:
										{
											if(((clock_year[2]%4==0)&&(clock_year[2]%100!=0))||(clock_year[2]%400==0))
											{
												if((clock_day[2]<=28)&&(clock_day[2]>=1))
													clock_day[2]++;
												else if(clock_day[2]==29)
													clock_day[2]=1;
											}
											else
											{
												if((clock_day[2]<=27)&&(clock_day[2]>=1))
													clock_day[2]++;
												else if(clock_day[2]==28)
													clock_day[2]=1;
											}
											break;
										}
									}
									break;
								}
															
								case 16:clock_second_3=(clock_second_3+3600)%86400;break;
						    case 17:clock_second_3=(clock_second_3+60)%86400;break;
						    case 18:clock_second_3=(clock_second_3+1)%86400;break;
								
								default:break;
							}					
						}
						else if(key_press_event[3+4*3]) //D键
						{
							switch(set_clock_select)
							{
								case 1:clock_year[0]--;break;
								case 2:
								{
									if((clock_month[0]>=2)&&(clock_month[0]<=12))
									clock_month[0]--;
									else if(clock_month[0]==1)
									clock_month[0]=12;
									break;
								}
								case 3:
								{
									switch (clock_month[0])
									{	
										case 1:
										case 3:
										case 5:
										case 7:
										case 8:
										case 10:
										case 12:
										{
											if((clock_day[0]<=31)&&(clock_day[0]>=2))
											 clock_day[0]--;
											else if(clock_day[0]==1)
												clock_day[0]=31;
											break;
										}
										case 4:
										case 6:
										case 9:
										case 11:
										{
											if((clock_day[0]<=30)&&(clock_day[0]>=2))
											clock_day[0]--;
											else if(clock_day[0]==1)
											clock_day[0]=30;
											break;
										}
										
										case 2:
										{
											if(((clock_year[0]%4==0)&&(clock_year[0]%100!=0))||(clock_year[0]%400==0))
											{
												if((clock_day[0]<=29)&&(clock_day[0]>=2))
													clock_day[0]--;
												else if(clock_day[0]==1)
													clock_day[0]=29;
											}
											else
											{
												if((clock_day[0]<=28)&&(clock_day[0]>=2))
													clock_day[0]--;
												else if(clock_day[0]==1)
													clock_day[0]=28;
											}
											break;
										}
									}
									break;
								}
								case 4:
								{

									if(clock_second_1>=3600)
									clock_second_1=(clock_second_1-3600)%86400;
									else
									{
										clock_second_1=(86400-(3600-clock_second_1))%86400;
									}
									break;
								}
								case 5:
								{

									if(clock_second_1>=60)
									clock_second_1=(clock_second_1-60)%86400;
									else
									{
										clock_second_1=(86400-(60-clock_second_1))%86400;
									}
									break;
								}
								case 6:
								{

									if(clock_second_1>=1)
									clock_second_1=(clock_second_1-1)%86400;
									else
									{
										clock_second_1=(86400-(1-clock_second_1))%86400;
									}
									break;
								}
								
								
								
								case 7:clock_year[1]--;break;
								case 8:
								{
									if((clock_month[1]>=2)&&(clock_month[1]<=12))
									clock_month[1]--;
									else if(clock_month[1]==1)
									clock_month[1]=12;
									break;
								}
								case 9:
								{
									switch (clock_month[1])
									{	
										case 1:
										case 3:
										case 5:
										case 7:
										case 8:
										case 10:
										case 12:
										{
											if((clock_day[1]<=31)&&(clock_day[1]>=2))
											 clock_day[1]--;
											else if(clock_day[1]==1)
												clock_day[1]=31;
											break;
										}
										case 4:
										case 6:
										case 9:
										case 11:
										{
											if((clock_day[1]<=30)&&(clock_day[1]>=2))
											clock_day[1]--;
											else if(clock_day[1]==1)
											clock_day[1]=30;
											break;
										}
										
										case 2:
										{
											if(((clock_year[1]%4==0)&&(clock_year[1]%100!=0))||(clock_year[1]%400==0))
											{
												if((clock_day[1]<=29)&&(clock_day[1]>=2))
													clock_day[1]--;
												else if(clock_day[1]==1)
													clock_day[1]=29;
											}
											else
											{
												if((clock_day[1]<=28)&&(clock_day[1]>=2))
													clock_day[1]--;
												else if(clock_day[1]==1)
													clock_day[1]=28;
											}
											break;
										}
									}
									break;
								}
								case 10:
								{

									if(clock_second_2>=3600)
									clock_second_2=(clock_second_2-3600)%86400;
									else
									{
										clock_second[1]=(86400-(3600-clock_second_2))%86400;
									}
									break;
								}
								case 11:
								{

									if(clock_second_2>=60)
									clock_second_2=(clock_second_2-60)%86400;
									else
									{
										clock_second_2=(86400-(60-clock_second_2))%86400;
									}
									break;
								}
								case 12:
								{

									if(clock_second_2>=1)
									clock_second_2=(clock_second_2-1)%86400;
									else
									{
										clock_second_2=(86400-(1-clock_second_2))%86400;
									}
									break;
								}
								
								
								
								case 13:clock_year[2]--;break;
								case 14:
								{
									if((clock_month[2]>=2)&&(clock_month[2]<=12))
									clock_month[2]--;
									else if(clock_month[2]==1)
									clock_month[2]=12;
									break;
								}
								case 15:
								{
									switch (clock_month[2])
									{	
										case 1:
										case 3:
										case 5:
										case 7:
										case 8:
										case 10:
										case 12:
										{
											if((clock_day[2]<=31)&&(clock_day[2]>=2))
											 clock_day[2]--;
											else if(clock_day[2]==1)
												clock_day[2]=31;
											break;
										}
										case 4:
										case 6:
										case 9:
										case 11:
										{
											if((clock_day[2]<=30)&&(clock_day[2]>=2))
											clock_day[2]--;
											else if(clock_day[2]==1)
											clock_day[2]=30;
											break;
										}
										
										case 2:
										{
											if(((clock_year[2]%4==0)&&(clock_year[2]%100!=0))||(clock_year[2]%400==0))
											{
												if((clock_day[2]<=29)&&(clock_day[2]>=2))
													clock_day[2]--;
												else if(clock_day[2]==1)
													clock_day[2]=29;
											}
											else
											{
												if((clock_day[2]<=28)&&(clock_day[2]>=2))
													clock_day[2]--;
												else if(clock_day[2]==1)
													clock_day[2]=28;
											}
											break;
										}
									}
									break;
								}
								case 16:
								{

									if(clock_second_3>=3600)
									clock_second_3=(clock_second_3-3600)%86400;
									else
									{
										clock_second_3=(86400-(3600-clock_second_3))%86400;
									}
									break;
								}
								case 17:
								{

									if(clock_second_3>=60)
									clock_second_3=(clock_second_3-60)%86400;
									else
									{
										clock_second_3=(86400-(60-clock_second_3))%86400;
									}
									break;
								}
								case 18:
								{

									if(clock_second_3>=1)
									clock_second_3=(clock_second_3-1)%86400;
									else
									{
										clock_second_3=(86400-(1-clock_second_3))%86400;
									}
									break;
								}
								
								default:break;
							}				 
						}	
						
						 if(clock_oled_update)
						 {
							 OLED_Clear();
						 }   
             delay_cycles(sysosc*0.002);						 
					   set_clock_light();
						 delay_cycles(sysosc*0.002);		
			 }
			
			 else if((!sw_set_clock)&&(!sw_set_time))//计时模式
			 {
				  if(timer_start_sign)//进入计时模式
				  {
						if(!timer_start_one)//只使能一次定时器
						{
							timer_start();//计时器开启
							timer_count=0;//重置计时器
							timer_start_one=1;//防止计时器重复使能
						}
				    scan_keyboard();//按键扫描函数
						
						if(key_press_event[3])//A键开启计时
						{
						   timer_key_press='A';
						}
						else if(key_press_event[3+4])//B键暂停计时
						{
						   timer_key_press='B';
						}
						else if(key_press_event[3+4*2])//C键清空计时
						{
						   timer_key_press='C';
						}
				
						OLED_Clear();
						OLED_ShowString(0,0,"Timer:");
						
						switch(timer_key_press)
						{
							 case 'A':
              {
								OLED_ShowString(8*6+8*5,0,"start");
							  break;
							}		
               case 'B':
              {
								OLED_ShowString(8*6+8*6,0,"stop");
							  break;
							}	
               case 'C':
              {
								OLED_ShowString(8*6+8*5,0,"clear");
							  break;
							}	
               default:
							{
								OLED_ShowString(8*6+8*5,0,"start");
								break;
							}							
						}
						 
						 
						 if(timer_count>=0&&timer_count<=9)
						 {
//							   num_to_ch(timer_count,timer_count_ch,5);
//							   OLED_ShowString(0,2,timer_count_ch);
							   OLED_ShowNum(0,2,timer_count,1,16);
						     OLED_ShowChar(0+8*1,2,'s');
						 }
						 else if(timer_count>=10&&timer_count<=99)
						 {
//						     num_to_ch(timer_count,timer_count_ch,5);
//							   OLED_ShowString(0,2,timer_count_ch);
							   OLED_ShowNum(0,2,timer_count,2,16);
						     OLED_ShowChar(0+8*2,2,'s');
						 }
						 else if(timer_count>=100&&timer_count<=999)
						 {
//						     num_to_ch(timer_count,timer_count_ch,5);
//							   OLED_ShowString(0,2,timer_count_ch);
							   OLED_ShowNum(0,2,timer_count,3,16);
						     OLED_ShowChar(0+8*3,2,'s');
						 }
//						 else if(timer_count>=1000&&timer_count<=9999)
//						 {
//						     num_to_ch(timer_count,timer_count_ch,5);
//							   OLED_ShowString(0,2,timer_count_ch);
//						     OLED_ShowChar(0+8*4,2,'s');
//						 }
//						 else if(timer_count>=10000&&timer_count<=99999)
//						 {
//						     num_to_ch(timer_count,timer_count_ch,5);
//							   OLED_ShowString(0,2,timer_count_ch);
//						     OLED_ShowChar(0+8*5,2,'s');
//						 }

 									 
					}
					else//退出计时模式
          {
						if(timer_start_one)
						{
							timer_stop();//计时器关闭
							timer_start_one=0;
						}
			      delay_cycles(sysosc*0.002);
					}
			 } 
	 }
}


void uart_receive_data(uint32_t *date)//uart接收数据
{
			 
	    if(uart_data_count<8)
			{
					uart_data[uart_data_count]=DL_UART_Main_receiveDataBlocking(UART_0_INST);
					uart_data_count++;
				
				 if((uart_data_count<=8)&&(uart_data[uart_data_count-1]=='\n'))//成功接收
				 {
						uart_data_len=uart_data_count-1;//扣除\n
						*date=0;
						for(int k=0;k<uart_data_len;k++)
						{
							*date+=pow(10,uart_data_len-1-k)*(uart_data[k]-'0');
						}
						
						uart_sign_count=0;
						uart_sign_len=0;
						uart_sign_end=0;
						
						for(int x=0;x<8;x++)
						{
							uart_sign[x]=0;
						}
						
						uart_sign_check_exit=0;
						
//						 if(uart_sign_check[0])
//						 {
//							 uint8_t data_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','y','e','a','r','.','\n'};
//						   uint8_t *v=data_ok;
//							 do
//							 {
//								 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//								 if((*v)!='\n')
//								 v++;
//							 } while((*v)!='\n');		
//					   }
//						 
//						 else if(uart_sign_check[1])
//						 {
//							 uint8_t data_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','m','o','n','t','h','.','\n'};
//						   uint8_t *v=data_ok;
//							 do
//							 {
//								 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//								 if((*v)!='\n')
//								 v++;
//							 } while((*v)!='\n');		
//					   }
//						  
//						 else if(uart_sign_check[2])
//						 {
//							 uint8_t data_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','d','a','y','.','\n'};
//						   uint8_t *v=data_ok;
//							 do
//							 {
//								 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//								 if((*v)!='\n')
//								 v++;
//							 } while((*v)!='\n');		
//					   }
//						 
//						 else if(uart_sign_check[3])
//						 {
//							 uint8_t data_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','h','o','u','r','.','\n'};
//						   uint8_t *v=data_ok;
//							 do
//							 {
//								 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//								 if((*v)!='\n')
//								 v++;
//							 } while((*v)!='\n');		
//					   }
//						 
//						 else if(uart_sign_check[4])
//						 {
//							 uint8_t data_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','m','i','n','.','\n'};
//						   uint8_t *v=data_ok;
//							 do
//							 {
//								 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//								 if((*v)!='\n')
//								 v++;
//							 } while((*v)!='\n');		
//					   }
//						 
//						 else if(uart_sign_check[5])
//						 {
//							 uint8_t data_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','s','e','c','o','n','d','.','\n'};
//						   uint8_t *v=data_ok;
//							 do
//							 {
//								 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//								 if((*v)!='\n')
//								 v++;
//							 } while((*v)!='\n');		
//					   }
						  
				 }
			}
		 
		 else
		 {
				 uart_sign_count=0;
				 uart_sign_len=0;
				 uart_sign_end=0;
				 
				 for(int x=0;x<8;x++)
					{
						uart_sign[x]=0;
					}			 
			 
//				if(uart_sign_check[0])
//				 {
//					 uint8_t data_ok[]={'f','a','i','l','e',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','y','e','a','r','.','\n'};
//					 uint8_t *v=data_ok;
//					 do
//					 {
//						 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//						 if((*v)!='\n')
//						 v++;
//					 } while((*v)!='\n');		
//				 }
//				 
//				 else if(uart_sign_check[1])
//				 {
//					 uint8_t data_ok[]={'f','a','i','l','e',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','m','o','n','t','h','.','\n'};
//					 uint8_t *v=data_ok;
//					 do
//					 {
//						 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//						 if((*v)!='\n')
//						 v++;
//					 } while((*v)!='\n');		
//				 }
//					
//				 else if(uart_sign_check[2])
//				 {
//					 uint8_t data_ok[]={'f','a','i','l','e',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','d','a','y','.','\n'};
//					 uint8_t *v=data_ok;
//					 do
//					 {
//						 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//						 if((*v)!='\n')
//						 v++;
//					 } while((*v)!='\n');		
//				 }
//				 
//				 else if(uart_sign_check[3])
//				 {
//					 uint8_t data_ok[]={'f','a','i','l','e',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','h','o','u','r','.','\n'};
//					 uint8_t *v=data_ok;
//					 do
//					 {
//						 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//						 if((*v)!='\n')
//						 v++;
//					 } while((*v)!='\n');		
//				 }
//				 
//				 else if(uart_sign_check[4])
//				 {
//					 uint8_t data_ok[]={'f','a','i','l','e',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','m','i','n','.','\n'};
//					 uint8_t *v=data_ok;
//					 do
//					 {
//						 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//						 if((*v)!='\n')
//						 v++;
//					 } while((*v)!='\n');		
//				 }
//				 
//				 else if(uart_sign_check[5])
//				 {
//					 uint8_t data_ok[]={'f','a','i','l','e',' ','r','e','c','e','i','v','e',' ','d','a','t','a',' ','s','e','c','o','n','d','.','\n'};
//					 uint8_t *v=data_ok;
//					 do
//					 {
//						 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
//						 if((*v)!='\n')
//						 v++;
//					 } while((*v)!='\n');		
//				 }
			  
		 }
}


void UART_0_INST_IRQHandler(void)
{
  switch(DL_UART_Main_getPendingInterrupt(UART_0_INST))
	{
	  case DL_UART_MAIN_IIDX_RX:
		{
			if(uart_sign[0]=='\n')
			{
			  uart_sign[0]=0;
				uart_sign_end=0;
				uart_sign_len=0;
				uart_sign_count=0;
			}
			
			DL_GPIO_togglePins(LED_LED7_PORT,LED_LED7_PIN);
			 
			if((!uart_sign_end))//还没有成功接收数据标签
			{		
					if((uart_sign_count<8)&&(uart_sign_count>=0))
					{
							uart_sign[uart_sign_count]=DL_UART_Main_receiveDataBlocking(UART_0_INST);
							uart_sign_count++;
							if(uart_sign[uart_sign_count-1]=='\n')
							{
								uart_sign_len=uart_sign_count;
								uart_sign_end=1;//数据标签接受结束
	
								for(int k=0;k<6;k++) 
								{
									uart_sign_check[k]=0;//重置
								}
								
								  for(int i=0;i<8;i++)
									{
										uart_data[i]=0;
									}
									
									uart_data_count=0;
									uart_data_len=0;
									
								
						 for(int i=0;i<6;i++)
						 {
							 if(!uart_sign_check_exit)
							 {
								switch(i)
								{ 
									case 0:
									{
										p=uart_sign;//指向数据标签地址头
										q=uart_year_sign;//指向年的数据标签头
										while((*p!='\n')&&(*q!='\n'))//均没有读取结束
										{
											 if(*p==*q)
											 {
												 p++;
												 q++;
											 }
											 else //输入错误
											 {
												 break;  
											 }
										}
								
										if((*p=='\n')&&(*q=='\n'))//成功匹配数据标签,标签为年
										{
											 uart_sign_check[0]=1;//标志是哪一个数据标签成功接收
											 
									     uart_sign_check_exit=1;
											
											 uint8_t year_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','s','i','g','n',' ','y','e','a','r','.','\n'};
											 uint8_t *v=year_ok;
											 do
											 {
												 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
												 if((*v)!='\n')
												 v++;
											 } while((*v)!='\n');
											    
										}

										
										break;
									}
									
									case 1:
									{
										p=uart_sign;//指向数据标签地址头
										q=uart_month_sign;//指向年的数据标签头
										while((*p!='\n')&&(*q!='\n'))//均没有读取结束
										{
											 if(*p==*q)
											 {
												 p++;
												 q++;
											 }
											 else //输入错误
											 {
												 break;  
											 }
										}
								
										if((*p=='\n')&&(*q=='\n'))//成功匹配数据标签,标签为月
										{
											 uart_sign_check[1]=1;
											 uart_sign_check_exit=1;
											
                       uint8_t month_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','s','i','g','n',' ','m','o','n','t','h','.','\n'};
											 uint8_t *v=month_ok;
											 do
											 {
												 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
												 if((*v)!='\n')
												 v++;
											 } while((*v)!='\n');											
											
										}

										
										break;
									}
									
									case 2:
									{
										p=uart_sign;//指向数据标签地址头
										q=uart_day_sign;//指向年的数据标签头
										while((*p!='\n')&&(*q!='\n'))//均没有读取结束
										{
											 if(*p==*q)
											 {
												 p++;
												 q++;
											 }
											 else //输入错误
											 {
												 break;  
											 }
										}
								
										if((*p=='\n')&&(*q=='\n'))//成功匹配数据标签,标签为日
										{
											 uart_sign_check[2]=1;
											 uart_sign_check_exit=1;
											
											 uint8_t day_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','s','i','g','n',' ','d','a','y','.','\n'};
											 uint8_t *v=day_ok;
											 do
											 {
												 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
												 if((*v)!='\n')
												 v++;
											 } while((*v)!='\n');			
										}

										
										break;
									}
									
									case 3:
									{
										p=uart_sign;//指向数据标签地址头
										q=uart_hour_sign;//指向年的数据标签头
										while((*p!='\n')&&(*q!='\n'))//均没有读取结束
										{
											 if(*p==*q)
											 {
												 p++;
												 q++;
											 }
											 else //输入错误
											 {
												 break;  
											 }
										}
								
										if((*p=='\n')&&(*q=='\n'))//成功匹配数据标签,标签为时
										{
											 uart_sign_check[3]=1;
											 uart_sign_check_exit=1;
											
											 uint8_t hour_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','s','i','g','n',' ','h','o','u','r','.','\n'};
											 uint8_t *v=hour_ok;
											 do
											 {
												 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
												 if((*v)!='\n')
												 v++;
											 } while((*v)!='\n');			
										}

										
										break;
									}
									
									case 4:
									{
										p=uart_sign;//指向数据标签地址头
										q=uart_min_sign;//指向年的数据标签头
										while((*p!='\n')&&(*q!='\n'))//均没有读取结束
										{
											 if(*p==*q)
											 {
												 p++;
												 q++;
											 }
											 else //输入错误
											 {
												 break;  
											 }
										}
								
										if((*p=='\n')&&(*q=='\n'))//成功匹配数据标签,标签为分
										{
											 uart_sign_check[4]=1;
											 uart_sign_check_exit=1;
											
											 uint8_t min_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','s','i','g','n',' ','m','i','n','.','\n'};
											 uint8_t *v=min_ok;
											 do
											 {
												 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
												 if((*v)!='\n')
												 v++;
											 } while((*v)!='\n');		
										}

										
										break;
									}
									
									case 5:
									{
										p=uart_sign;//指向数据标签地址头
										q=uart_second_sign;//指向年的数据标签头
										while((*p!='\n')&&(*q!='\n'))//均没有读取结束
										{
											 if(*p==*q)
											 {
												 p++;
												 q++;
											 }
											 else //输入错误
											 {
												 break;  
											 }
										}
								
										if((*p=='\n')&&(*q=='\n'))//成功匹配数据标签,标签为秒
										{
											 uart_sign_check[5]=1;
											 uart_sign_check_exit=1;
											
											 uint8_t second_ok[]={'s','u','c','c','e','s','s',' ','r','e','c','e','i','v','e',' ','s','i','g','n',' ','s','e','c','o','n','d','.','\n'};
											 uint8_t *v=second_ok;
											 do
											 {
												 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
												 if((*v)!='\n')
												 v++;
											 } while((*v)!='\n');		
										}

										
										break;
									}
									
									default:break;
								}
							
							}								
							
						 }
						 
						 if(!uart_sign_check_exit)
						 {
                 uint8_t sign_faile[]={'f','a','i','l','e',' ','r','e','c','e','i','v','e',' ','s','i','g','n','.','\n'};
								 uint8_t *v=sign_faile;
								 do
								 {
									 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
									 if((*v)!='\n')
									 v++;
								 } while((*v)!='\n');	 
                 
                 uart_sign_count=0;
						     uart_sign_len=0;
                 uart_sign_end=1;								 
						 }
						 		 
					
							}
						
					}
					
					else if(uart_sign_count>=8)//溢出清零
					{
						 uint8_t uart_sign_overflow[]={'u','a','r','t',' ','o','v','e','r','f','l','o','w','.'};
						 uint8_t *v=uart_sign_overflow;
						 do
						 {
							 DL_UART_Main_transmitDataBlocking(UART_0_INST,*v);
							 if((*v)!='\n')
							 v++;
						 } while((*v)!='\n');		
											 
						 for(int i=0;i<8;i++)
						 {
							 uart_sign[i]=0;
						 }
						 uart_sign_count=0;
						 uart_sign_len=0;
					}
		 }
			
			else	//接收数据
			{
					if( uart_sign_check[0])//年标志
					{
						uart_receive_data(&now_year); 
					}
					else if( uart_sign_check[1])//月标志
					{
						uart_receive_data(&now_month);   
					}
					else if( uart_sign_check[2])//日标志
					{
						uart_receive_data(&now_day); 
					}
					else if( uart_sign_check[3])//时标志
					{
						uart_receive_data(&time_hour); 
					}
					else if( uart_sign_check[4])//分标志
					{
						uart_receive_data(&time_min); 
					}
					else if( uart_sign_check[5])//秒标志
					{
						uart_receive_data(&time_second); 
					}
					
					second=time_hour*3600+time_min*60+time_second;
				
			}
			
			break;
		}
	  default:break;
	}
}


void uart_start(void)
{
   NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN); 
	 NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	 DL_UART_Main_enableInterrupt(UART_0_INST,DL_UART_MAIN_INTERRUPT_RX);
	 DL_UART_Main_enable(UART_0_INST);
}

void uart_stop(void)
{
	 DL_UART_Main_disable(UART_0_INST);
	 DL_UART_Main_disableInterrupt(UART_0_INST,DL_UART_MAIN_INTERRUPT_RX);
   NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN); 
	 NVIC_DisableIRQ(UART_0_INST_INT_IRQN);
}

void pwm_buzzer(void)//闹钟响应，pwm驱动buzzer
{
   NVIC_ClearPendingIRQ(PWM_0_INST_INT_IRQN); 
	 NVIC_EnableIRQ(PWM_0_INST_INT_IRQN);
	 DL_TimerG_clearInterruptStatus(PWM_0_INST,DL_TIMERG_EVENT_ZERO_EVENT);
	 DL_TimerG_enableInterrupt(PWM_0_INST,DL_TIMERG_EVENT_ZERO_EVENT);
	 DL_TimerG_startCounter(PWM_0_INST);
}

void disable_pwm_buzzer(void)//闹钟响应结束，关闭pwm驱动buzzer
{
	 DL_TimerG_stopCounter(PWM_0_INST);
	 DL_TimerG_clearInterruptStatus(PWM_0_INST,DL_TIMERG_EVENT_ZERO_EVENT);
	 DL_TimerG_disableInterrupt(PWM_0_INST,DL_TIMERG_EVENT_ZERO_EVENT);
   NVIC_ClearPendingIRQ(PWM_0_INST_INT_IRQN); 
	 NVIC_DisableIRQ(PWM_0_INST_INT_IRQN);
}

// 修复：新增通用按键扫描函数
void scan_keyboard(void)
{
    uint8_t key_current_state[16] = {0};
    
    // 扫描4x4矩阵键盘（逐行扫描）
    for(int row = 0; row < 4; row++)
    {
        // 拉低当前行
        DL_GPIO_clearPins(Keybord_out_PORT, Keybord_out[row]);
        // 其余行拉高
        for(int r = 0; r < 4; r++)
        {
            if(r != row)
                DL_GPIO_setPins(Keybord_out_PORT, Keybord_out[r]);
        }
        // 短暂延时消抖
        delay_cycles(sysosc * 0.02);
        // 读取4列状态
        for(int col = 0; col < 4; col++)
        {
            if(!DL_GPIO_readPins(Keybord_in_PORT, Keybord_in[col]))
            {
							  delay_cycles(sysosc * 0.02);
							  if(!DL_GPIO_readPins(Keybord_in_PORT, Keybord_in[col]))
                key_current_state[row * 4 + col] = 1; // 按键按下
								else
								key_current_state[row * 4 + col] = 0; // 按键释放
            }
            else
            {
                key_current_state[row * 4 + col] = 0; // 按键释放
            }
        }
    }
    
    // 检测按键按下事件（下降沿）
    for(int i = 0; i < 16; i++)
    {
        if(key_current_state[i] && !key_prev_state[i])
        {
            key_press_event[i] = 1; // 标记按键按下
        }
        else
        {
            key_press_event[i] = 0;
        }
        key_prev_state[i] = key_current_state[i]; // 更新上一次状态
    }
}


void PWM_0_INST_IRQHandler()
{
   switch(DL_TimerG_getPendingInterrupt(PWM_0_INST))
	 {
	   case DL_TIMERG_IIDX_ZERO:
		 {
			 if(pwm_buzzer_start)
			 pwm_buzzer_count++;
			 
			 if(pwm_buzzer_start&&pwm_buzzer_count>=5000)
			 { 
				 pwm_buzzer_count=0;
		     pwm_buzzer_start=0;
			 }
			 
			 break;
		 }
		 default:break;
	 }
}

void set_clock_light(void)//设置闹钟高亮
{
	if((set_clock_select<=6)&&(set_clock_select>=1))
	{
		OLED_ShowString(0,0,"clock_1:");
	}
	else if((set_clock_select<=12)&&(set_clock_select>=7))
	{
	  OLED_ShowString(0,0,"clock_2:");
	}
	else if((set_clock_select<=18)&&(set_clock_select>=13))
	{
	  OLED_ShowString(0,0,"clock_3:");
	}
	
	if((set_clock_select%6)<=3&&((set_clock_select%6)!=0))
	{
		 OLED_ShowString(0,2,"year:");
		 
		 if((set_clock_select<=6)&&(set_clock_select>=1))
		 {
			 OLED_ShowNum(0+8*5,2,clock_year[0],4,16);
		 }
		 else if((set_clock_select<=12)&&(set_clock_select>=7))
		 {
		   OLED_ShowNum(0+8*5,2,clock_year[1],4,16);
		 }
		 else if((set_clock_select<=18)&&(set_clock_select>=13))
		 {
			 OLED_ShowNum(0+8*5,2,clock_year[2],4,16);
		 }
		
	   OLED_ShowString(0,4,"month:");
		 
			 if((set_clock_select<=6)&&(set_clock_select>=1))
			 {
				 if((clock_month[0]>=10)&&(clock_month[0]<=12))
				 OLED_ShowNum(0+8*6,4,clock_month[0],2,16);
				 else if((clock_month[0]>=1)&&(clock_month[0]<=9))
				 {
					 OLED_ShowNum(0+8*6,4,0,1,16);
					 OLED_ShowNum(0+8*7,4,clock_month[0],1,16);
				 }
				 else
				 {
					 OLED_ShowString(0+8*6,4,"error");
				 }
			 }
			 else if((set_clock_select<=12)&&(set_clock_select>=7))
			 {
				 if((clock_month[1]>=10)&&(clock_month[1]<=12))
				 OLED_ShowNum(0+8*6,4,clock_month[1],2,16);
				 else if((clock_month[1]>=1)&&(clock_month[1]<=9))
				 {
					 OLED_ShowNum(0+8*6,4,0,1,16);
					 OLED_ShowNum(0+8*7,4,clock_month[1],1,16);
				 }
				  else
				 {
					 OLED_ShowString(0+8*6,4,"error");
				 }
			 }
			 else if((set_clock_select<=18)&&(set_clock_select>=13))
			 {
				 if((clock_month[2]>=10)&&(clock_month[2]<=12))
				 OLED_ShowNum(0+8*6,4,clock_month[2],2,16);
				 else if((clock_month[2]>=1)&&(clock_month[2]<=9))
				 {
					 OLED_ShowNum(0+8*6,4,0,1,16);
					 OLED_ShowNum(0+8*7,4,clock_month[2],1,16);
				 }
				  else
				 {
					 OLED_ShowString(0+8*6,4,"error");
				 }
			 }
	   				 
		 OLED_ShowString(0,6,"day:");	
		 
			 
					 if((set_clock_select<=6)&&(set_clock_select>=1))
					 {
						   switch(clock_month[0])
							 {
								 case 1:
								 case 3:
								 case 5:
								 case 7:
								 case 8:
								 case 10:
								 case 12:
								 {
									 if((clock_day[0]>=10)&&(clock_day[0]<=31))
									 OLED_ShowNum(0+8*4,6,clock_day[0],2,16);
									 else if((clock_day[0]>=1)&&(clock_day[0]<=9))
									 {
										 OLED_ShowNum(0+8*4,6,0,1,16);
										 OLED_ShowNum(0+8*5,6,clock_day[0],1,16);
									 }
									 else
									 {
										 OLED_ShowString(0+8*4,6,"error");
									 }
									 break;
								 }
								 
								  case 4:
								  case 6:
								  case 9:
								  case 11:
									{
											if((clock_day[0]>=10)&&(clock_day[0]<=30))
											OLED_ShowNum(0+8*4,6,clock_day[0],2,16);
											else if((clock_day[0]>=1)&&(clock_day[0]<=9))
											{
												OLED_ShowNum(0+8*4,6,0,1,16);
												OLED_ShowNum(0+8*5,6,clock_day[0],1,16);
											}
											else
										  {
											  OLED_ShowString(0+8*4,6,"error");
										  }
									    break;
									}
									
									case 2:
									{
										
											if(((clock_year[0]%4==0)&&(clock_year[0]%100!=0))||(clock_year[0]%400==0))
											{
												 if((clock_day[0]>=10)&&(clock_day[0]<=29))
												 OLED_ShowNum(0+8*4,6,clock_day[0],2,16);
												 else if((clock_day[0]>=1)&&(clock_day[0]<=9))
												 {
													 OLED_ShowNum(0+8*4,6,0,1,16);
													 OLED_ShowNum(0+8*5,6,clock_day[0],1,16);
												 }
													else
												 {
													 OLED_ShowString(0+8*4,6,"error");
												 }
											}
											else
											{
												 if((clock_day[0]>=10)&&(clock_day[0]<=28))
												 {
													 OLED_ShowNum(0+8*4,6,clock_day[0],2,16);
												 }
												 else if((clock_day[0]>=1)&&(clock_day[0]<=9))
												 {
													 OLED_ShowNum(0+8*4,6,0,1,16);
													 OLED_ShowNum(0+8*5,6,clock_day[0],1,16);
												 }
												 else
												 {
													 OLED_ShowString(0+8*4,6,"error");
												 }
											}
												
									    break;
									}
									
									default:break;
						   }
					 }
					 else if((set_clock_select<=12)&&(set_clock_select>=7))
					 {
						   switch(clock_month[1])
							 {	 
								 case 1:
								 case 3:
								 case 5:
								 case 7:
								 case 8:
								 case 10:
								 case 12:
								 {
									 if((clock_day[1]>=10)&&(clock_day[1]<=31))
									 OLED_ShowNum(0+8*4,6,clock_day[1],2,16);
									 else if((clock_day[1]>=1)&&(clock_day[1]<=9))
									 {	 
										 OLED_ShowNum(0+8*4,6,0,1,16);
										 OLED_ShowNum(0+8*5,6,clock_day[1],1,16);
									 }
									 else
									 {
										 OLED_ShowString(0+8*4,6,"error");
									 }
									 break;
								 }
								 
								  case 4:
								  case 6:
								  case 9:
								  case 11:
									{
										if((clock_day[1]>=10)&&(clock_day[1]<=30))
										OLED_ShowNum(0+8*4,6,clock_day[1],2,16);
										else if((clock_day[1]>=1)&&(clock_day[1]<=9))
										{
											OLED_ShowNum(0+8*4,6,0,1,16);
											OLED_ShowNum(0+8*5,6,clock_day[1],1,16);
										}
										else
									  {
										 OLED_ShowString(0+8*4,6,"error");
									  }
									  break;
									}
									
									case 2:
									{
										    if(((clock_year[1]%4==0)&&(clock_year[1]%100!=0))||(clock_year[1]%400==0))
												{
													 if((clock_day[1]>=10)&&(clock_day[1]<=29))
													 OLED_ShowNum(0+8*4,6,clock_day[1],2,16);
													 else if((clock_day[1]>=1)&&(clock_day[1]<=9))
													 {
														 OLED_ShowNum(0+8*4,6,0,1,16);
														 OLED_ShowNum(0+8*5,6,clock_day[1],1,16);
													 }
													 else
													 {
														 OLED_ShowString(0+8*4,6,"error");
													 }
												}
												else
												{
													 if((clock_day[1]>=10)&&(clock_day[1]<=28))
													 {
														 OLED_ShowNum(0+8*4,6,clock_day[1],2,16);
													 }
													 else if((clock_day[1]>=1)&&(clock_day[1]<=9))
													 {
														 OLED_ShowNum(0+8*4,6,0,1,16);
														 OLED_ShowNum(0+8*5,6,clock_day[1],1,16);
													 }
													 else
													 {
														 OLED_ShowString(0+8*4,6,"error");
													 }
											 }
									     break;
									}
									
									default:break;
						   }
					 }
					 else if((set_clock_select<=18)&&(set_clock_select>=13))
					 {
						 switch(clock_month[2])
						 {
							   case 1:
								 case 3:
								 case 5:
								 case 7:
								 case 8:
								 case 10:
								 case 12:
								 {
									 if((clock_day[2]>=10)&&(clock_day[2]<=31))
									 OLED_ShowNum(0+8*4,6,clock_day[2],2,16);
									 else if((clock_day[2]>=1)&&(clock_day[2]<=9))
									 {
										 OLED_ShowNum(0+8*4,6,0,1,16);
										 OLED_ShowNum(0+8*5,6,clock_day[2],1,16);
									 }
									 else
									 {
										 OLED_ShowString(0+8*4,6,"error");
									 }
									 break;
								 }
								 
								  case 4:
								  case 6:
								  case 9:
								  case 11:
									{
										if((clock_day[2]>=10)&&(clock_day[2]<=30))
										OLED_ShowNum(0+8*4,6,clock_day[2],2,16);
										else if((clock_day[2]>=1)&&(clock_day[2]<=9))
										{
											OLED_ShowNum(0+8*4,6,0,1,16);
											OLED_ShowNum(0+8*5,6,clock_day[2],1,16);
										}
										else
									  {
										 OLED_ShowString(0+8*4,6,"error");
									  }
									  break;
									}
									
									case 2:
									{
										
										 if(((clock_year[2]%4==0)&&(clock_year[2]%100!=0))||(clock_year[2]%400==0))
												{
													 if((clock_day[2]>=10)&&(clock_day[2]<=29))
													 OLED_ShowNum(0+8*4,6,clock_day[2],2,16);
													 else if((clock_day[2]>=1)&&(clock_day[2]<=9))
													 {
														 OLED_ShowNum(0+8*4,6,0,1,16);
														 OLED_ShowNum(0+8*5,6,clock_day[2],1,16);
													 }
													 else
													 {
														 OLED_ShowString(0+8*4,6,"error");
													 }
												}
												else
												{
													 if((clock_day[2]>=10)&&(clock_day[2]<=28))
													 {
														 OLED_ShowNum(0+8*4,6,clock_day[2],2,16);
													 }
													 else if((clock_day[2]>=1)&&(clock_day[2]<=9))
													 {
														 OLED_ShowNum(0+8*4,6,0,1,16);
														 OLED_ShowNum(0+8*5,6,clock_day[2],1,16);
													 }
													 else
													 {
														 OLED_ShowString(0+8*4,6,"error");
													 }
											 }
									     break;
										
									}
									
									default:break;
					   }
					 }

					 
		if((set_clock_select<=6)&&(set_clock_select>=1)) //clock_1
		{
				for(int j=0;j<8;j++)
				{ 
					for(int i=0;i<8;i++)
					{
					 if(set_clock_select<=3)
					 OLED_DrawPoint(127-j,(set_clock_select*2+1)*8+i,1);
					 else
					 OLED_DrawPoint(127-j,((set_clock_select-3)*2+1)*8+i,1); 
					}
					if(j<4)
					{
						if(set_clock_select<=3)
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,(set_clock_select*2+1)*8+j,1);
						}
						else
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3)*2+1)*8+j,1);
						}
					}
					else
					{
						if(set_clock_select<=3)
						{
							for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,(set_clock_select*2+1)*8+j,1);
						}
						else
						{
						  for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3)*2+1)*8+j,1);
						}
					}
				}		
		}
		
		else if((set_clock_select<=12)&&(set_clock_select>=7)) //clock_2
		{
			
			  for(int j=0;j<8;j++)
				{ 
					for(int i=0;i<8;i++)
					{
					 if(set_clock_select<=9)
					 OLED_DrawPoint(127-j,((set_clock_select-6)*2+1)*8+i,1);
					 else
					 OLED_DrawPoint(127-j,((set_clock_select-3-6)*2+1)*8+i,1); 
					}
					if(j<4)
					{
						if(set_clock_select<=9)
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6)*2+1)*8+j,1);
						}
						else
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6)*2+1)*8+j,1);
						}
					}
					else
					{
						if(set_clock_select<=9)
						{
							for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6)*2+1)*8+j,1);
						}
						else
						{
						  for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6)*2+1)*8+j,1);
						}
					}
				}		
			
		}
		
		else if((set_clock_select<=18)&&(set_clock_select>=13)) //clock_3
		{
		    for(int j=0;j<8;j++)
				{ 
					for(int i=0;i<8;i++)
					{
					 if(set_clock_select<=15)
					 OLED_DrawPoint(127-j,((set_clock_select-6-6)*2+1)*8+i,1);
					 else
					 OLED_DrawPoint(127-j,((set_clock_select-3-6-6)*2+1)*8+i,1); 
					}
					if(j<4)
					{
						if(set_clock_select<=15)
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6-6)*2+1)*8+j,1);
						}
						else
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6-6)*2+1)*8+j,1);
						}
					}
					else
					{
						if(set_clock_select<=15)
						{
							for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6-6)*2+1)*8+j,1);
						}
						else
						{
						  for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6-6)*2+1)*8+j,1);
						}
					}
				}						
		}
		
  }
	else
	{ 
		
		OLED_ShowString(0,2,"hour:");
		OLED_ShowString(0,4,"min:");
		OLED_ShowString(0,6,"s:");
		
	  if((set_clock_select<=6)&&(set_clock_select>=1)) //clock_1
		{
			  if(!sw_time_to_12)//24小时制
				{
					clock_hour[0]=clock_second_1/3600;
					clock_min[0]=(clock_second_1-clock_hour[0]*3600)/60;
				}
				else//12小时制
				{
					clock_hour[0]=clock_second_1/3600;
					clock_min[0]=(clock_second_1-clock_hour[0]*3600)/60;
					if((clock_hour[0]>=13)&&(clock_hour[0]<=23))
					{
						clock_hour[0]-=12;
					}
				}
				clock_second[0]=clock_second_1%60;			
			
				if(clock_hour[0]>=10&&clock_hour[0]<=23)
				{
					OLED_ShowNum(0+8*5,2,clock_hour[0],2,16);
				}	
				else if((clock_hour[0]>=0)&&(clock_hour[0]<=9))
				{
					OLED_ShowNum(0+8*5,2,0,1,16);
					OLED_ShowNum(0+8*6,2,clock_hour[0],1,16);
				}
				
				if(clock_min[0]>=10&&clock_min[0]<=60)
				{
					OLED_ShowNum(0+8*4,4,clock_min[0],2,16);
				}	
				else if((clock_min[0]>=0)&&(clock_min[0]<=9))
				{
					OLED_ShowNum(0+8*4,4,0,1,16);
					OLED_ShowNum(0+8*5,4,clock_min[0],1,16);
				}
				
				if(clock_second[0]>=10&&clock_second[0]<=60)
			{
				OLED_ShowNum(0+8*2,6,clock_second[0],2,16);
			}	
			else if((clock_second[0]>=0)&&(clock_second[0]<=9))
			{
				OLED_ShowNum(0+8*2,6,0,1,16);
				OLED_ShowNum(0+8*3,6,clock_second[0],1,16);
			}
			
			
			
				for(int j=0;j<8;j++)
				{ 
					for(int i=0;i<8;i++)
					{
					 if(set_clock_select<=3)
					 OLED_DrawPoint(127-j,(set_clock_select*2+1)*8+i,1);
					 else
					 OLED_DrawPoint(127-j,((set_clock_select-3)*2+1)*8+i,1); 
					}
					if(j<4)
					{
						if(set_clock_select<=3)
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,(set_clock_select*2+1)*8+j,1);
						}
						else
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3)*2+1)*8+j,1);
						}
					}
					else
					{
						if(set_clock_select<=3)
						{
							for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,(set_clock_select*2+1)*8+j,1);
						}
						else
						{
						  for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3)*2+1)*8+j,1);
						}
					}
				}	
						
	  }
		
	  else if((set_clock_select<=12)&&(set_clock_select>=7)) //clock_2
		{
			  if(!sw_time_to_12)//24小时制
				{
					clock_hour[1]=clock_second_2/3600;
					clock_min[1]=(clock_second_2-clock_hour[1]*3600)/60;
				}
				else//12小时制
				{
					clock_hour[1]=clock_second_2/3600;
					clock_min[1]=(clock_second_2-clock_hour[1]*3600)/60;
					if((clock_hour[1]>=13)&&(clock_hour[1]<=23))
					{
						clock_hour[1]-=12;
					}
				}
				clock_second[1]=clock_second_2%60;
			  
				if(clock_hour[1]>=10&&clock_hour[1]<=23)
				{
					OLED_ShowNum(0+8*5,2,clock_hour[1],2,16);
				}	
				else if((clock_hour[1]>=0)&&(clock_hour[1]<=9))
				{
					OLED_ShowNum(0+8*5,2,0,1,16);
					OLED_ShowNum(0+8*6,2,clock_hour[1],1,16);
				}
				
				if(clock_min[1]>=10&&clock_min[1]<=60)
				{
					OLED_ShowNum(0+8*4,4,clock_min[1],2,16);
				}	
				else if((clock_min[1]>=0)&&(clock_min[1]<=9))
				{
					OLED_ShowNum(0+8*4,4,0,1,16);
					OLED_ShowNum(0+8*5,4,clock_min[1],1,16);
				}
				if(clock_second[1]>=10&&clock_second[1]<=60)
				{
					OLED_ShowNum(0+8*2,6,clock_second[1],2,16);
				}	
				else if((clock_second[1]>=0)&&(clock_second[1]<=9))
				{
					OLED_ShowNum(0+8*2,6,0,1,16);
					OLED_ShowNum(0+8*3,6,clock_second[1],1,16);
				}
				
				
				
				 for(int j=0;j<8;j++)
				{ 
					for(int i=0;i<8;i++)
					{
					 if(set_clock_select<=9)
					 OLED_DrawPoint(127-j,((set_clock_select-6)*2+1)*8+i,1);
					 else
					 OLED_DrawPoint(127-j,((set_clock_select-3-6)*2+1)*8+i,1); 
					}
					if(j<4)
					{
						if(set_clock_select<=9)
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6)*2+1)*8+j,1);
						}
						else
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6)*2+1)*8+j,1);
						}
					}
					else
					{
						if(set_clock_select<=9)
						{
							for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6)*2+1)*8+j,1);
						}
						else
						{
						  for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6)*2+1)*8+j,1);
						}
					}
				}		
				
	  }
		
		else if((set_clock_select<=18)&&(set_clock_select>=13)) //clock_3
		{
			   if(!sw_time_to_12)//24小时制
				{
					clock_hour[2]=clock_second_3/3600;
					clock_min[2]=(clock_second_3-clock_hour[2]*3600)/60;
				}
				else//12小时制
				{
					clock_hour[2]=clock_second_3/3600;
					clock_min[2]=(clock_second_3-clock_hour[2]*3600)/60;
					if((clock_hour[2]>=13)&&(clock_hour[2]<=23))
					{
						clock_hour[2]-=12;
					}
				}
				clock_second[2]=clock_second_3%60;
			
				if(clock_hour[2]>=10&&clock_hour[2]<=23)
				{
					OLED_ShowNum(0+8*5,2,clock_hour[2],2,16);
				}	
				else if((clock_hour[2]>=0)&&(clock_hour[2]<=9))
				{
					OLED_ShowNum(0+8*5,2,0,1,16);
					OLED_ShowNum(0+8*6,2,clock_hour[2],1,16);
				}
				
				if(clock_min[2]>=10&&clock_min[2]<=60)
				{
					OLED_ShowNum(0+8*4,4,clock_min[2],2,16);
				}	
				else if((clock_min[2]>=0)&&(clock_min[2]<=9))
				{
					OLED_ShowNum(0+8*4,4,0,1,16);
					OLED_ShowNum(0+8*5,4,clock_min[2],1,16);
				}
				
				if(clock_second[2]>=10&&clock_second[2]<=60)
				{
					OLED_ShowNum(0+8*2,6,clock_second[2],2,16);
				}	
				else if((clock_second[2]>=0)&&(clock_second[2]<=9))
				{
					OLED_ShowNum(0+8*2,6,0,1,16);
					OLED_ShowNum(0+8*3,6,clock_second[2],1,16);
				}
				
				
				   for(int j=0;j<8;j++)
				{ 
					for(int i=0;i<8;i++)
					{
					 if(set_clock_select<=15)
					 OLED_DrawPoint(127-j,((set_clock_select-6-6)*2+1)*8+i,1);
					 else
					 OLED_DrawPoint(127-j,((set_clock_select-3-6-6)*2+1)*8+i,1); 
					}
					if(j<4)
					{
						if(set_clock_select<=15)
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6-6)*2+1)*8+j,1);
						}
						else
						{
							for(int n=0;n<j+1;n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6-6)*2+1)*8+j,1);
						}
					}
					else
					{ 
						if(set_clock_select<=15)
						{
							for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-6-6)*2+1)*8+j,1);
						}
						else
						{
						  for(int n=0;n<(8-j);n++)
						  OLED_DrawPoint(127-7-n,((set_clock_select-3-6-6)*2+1)*8+j,1);
						}
					}
				}	
				
	  }
	}
}


void set_time_light(void)//设置时间高亮
{
	OLED_ShowString(0,0,"setTime:");
	
	if(sw_set_time_select<=3)
	{
		 OLED_ShowString(0,2,"year:");
		 OLED_ShowNum(0+8*5,2,now_year,4,16);
		
	   OLED_ShowString(0,4,"month:");
		
		 if((now_month>=10)&&(now_month<=12))
		 {
			 OLED_ShowNum(0+8*6,4,now_month,2,16);
	   }
		 else if((now_month>=1)&&(now_month<=9))
		 {
		   OLED_ShowNum(0+8*6,4,0,1,16);
			 OLED_ShowNum(0+8*7,4,now_month,1,16);
		 }
		 else
		 {
			 OLED_ShowString(0+8*6,4,"error");
		 }
		 
		 OLED_ShowString(0,6,"day:");	
		 
		 switch(now_month)
		 {
			 case 1:
			 case 3:
			 case 5:
			 case 7:
			 case 8:
			 case 10:
			 case 12:
			 {
				 if((now_day>=10)&&(now_day<=31))
				 {
					 OLED_ShowNum(0+8*4,6,now_day,2,16);
				 }
				 else if((now_day>=1)&&(now_day<=9))
				 {
					 OLED_ShowNum(0+8*4,6,0,1,16);
					 OLED_ShowNum(0+8*5,6,now_day,1,16);
				 }
				 else
				 {
					 OLED_ShowString(0+8*4,6,"error");
				 }
				 break;
			 }

			 case 4:
			 case 6:
			 case 9:
			 case 11:
			 {
				 if((now_day>=10)&&(now_day<=30))
				 {
					 OLED_ShowNum(0+8*4,6,now_day,2,16);
				 }
				 else if((now_day>=1)&&(now_day<=9))
				 {
					 OLED_ShowNum(0+8*4,6,0,1,16);
					 OLED_ShowNum(0+8*5,6,now_day,1,16);
				 }
				 else
				 {
					 OLED_ShowString(0+8*4,6,"error");
				 }
			   break;
			 }
		
			 case 2:
			 {
				 if(((now_year%4==0)&&(now_year%100!=0))||(now_year%400==0))
				 {
					 if((now_day>=10)&&(now_day<=29))
					 {
						 OLED_ShowNum(0+8*4,6,now_day,2,16);
					 }
					 else if((now_day>=1)&&(now_day<=9))
					 {
						 OLED_ShowNum(0+8*4,6,0,1,16);
						 OLED_ShowNum(0+8*5,6,now_day,1,16);
					 }
					 else
					 {
					   OLED_ShowString(0+8*4,6,"error");
					 }
				 }
				 else
				 {
				   if((now_day>=10)&&(now_day<=28))
					 {
						 OLED_ShowNum(0+8*4,6,now_day,2,16);
					 }
					 else if((now_day>=1)&&(now_day<=9))
					 {
						 OLED_ShowNum(0+8*4,6,0,1,16);
						 OLED_ShowNum(0+8*5,6,now_day,1,16);
					 }
					 else
					 {
					   OLED_ShowString(0+8*4,6,"error");
					 }
				 }
			   break;
			 }
		
			 default:break;
	   }
				
		for(int j=0;j<8;j++)
	  { 
			for(int i=0;i<8;i++)
			{
			 OLED_DrawPoint(127-j,(sw_set_time_select*2+1)*8+i,1);
			}
			if(j<4)
			{
				for(int n=0;n<j+1;n++)
				OLED_DrawPoint(127-7-n,(sw_set_time_select*2+1)*8+j,1);
			}
			else
			{
				for(int n=0;n<(8-j);n++)
				OLED_DrawPoint(127-7-n,(sw_set_time_select*2+1)*8+j,1);
			}
    } 
    		
  }
	else
	{
		time_hour=second/3600;
	  time_min=(second-time_hour*3600)/60;
	  time_second=second%60;
		
		OLED_ShowString(0,2,"hour:");
		
		if(time_hour>=10&&time_hour<=24)
		{
			OLED_ShowNum(0+8*5,2,time_hour,2,16);
		}	
		else if((time_hour>=0)&&(time_hour<=9))
		{
		  OLED_ShowNum(0+8*5,2,0,1,16);
			OLED_ShowNum(0+8*6,2,time_hour,1,16);
		}
		
	  OLED_ShowString(0,4,"min:");
		
		if(time_min>=10&&time_min<=60)
		{
			OLED_ShowNum(0+8*4,4,time_min,2,16);
		}	
		else if((time_min>=0)&&(time_min<=9))
		{
		  OLED_ShowNum(0+8*4,4,0,1,16);
			OLED_ShowNum(0+8*5,4,time_min,1,16);
		}
		
		OLED_ShowString(0,6,"s:");
		
		if(time_second>=10&&time_second<=60)
		{
			OLED_ShowNum(0+8*2,6,time_second,2,16);
		}	
		else if((time_second>=0)&&(time_second<=9))
		{
		  OLED_ShowNum(0+8*2,6,0,1,16);
			OLED_ShowNum(0+8*3,6,time_second,1,16);
		}
			
   	for(int j=0;j<8;j++)
	  { 
			for(int i=0;i<8;i++)
			{
			 OLED_DrawPoint(127-j,((sw_set_time_select-3)*2+1)*8+i,1);
			}
			if(j<4)
			{
				for(int n=0;n<j+1;n++)
				OLED_DrawPoint(127-7-n,((sw_set_time_select-3)*2+1)*8+j,1);
			}
			else
			{
				for(int n=0;n<(8-j);n++)
				OLED_DrawPoint(127-7-n,((sw_set_time_select-3)*2+1)*8+j,1);
			}
    } 
    
    
	}
}

void sw_time_fast_model_deal(void)
{
   if(sw_time_fast)
	 {
		 if(!sw_time_fast_start)//只触发一次使能
		 {
			 NVIC_ClearPendingIRQ(TIMER_2_INST_INT_IRQN);
			 NVIC_EnableIRQ(TIMER_2_INST_INT_IRQN);
       DL_TimerG_enableInterrupt(TIMER_2_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);	
       DL_TimerG_startCounter(TIMER_2_INST);	
			 sw_time_fast_start=1;
     }			 
			 while(!one_second_fast)
		 {
			 __WFE();
		 }
		 one_second_fast=0;
	 }
	 else
	 {
		  if(sw_time_fast_start)
			{
				DL_TimerG_stopCounter(TIMER_2_INST);	
		    DL_TimerG_clearInterruptStatus(TIMER_2_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);
		    DL_TimerG_disableInterrupt(TIMER_2_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);	
		    NVIC_ClearPendingIRQ(TIMER_2_INST_INT_IRQN);
	      NVIC_DisableIRQ(TIMER_2_INST_INT_IRQN);
			  sw_time_fast_start=0;
			}
	 }
}

void sw_time_very_fast_model_deal(void)
{
   if(sw_time_very_fast)
	 {
		 if(!sw_time_very_fast_start)
		 {
			 NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
			 NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
       DL_TimerA_enableInterrupt(TIMER_1_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);	
       DL_TimerA_startCounter(TIMER_1_INST);	
			 sw_time_very_fast_start=1;
     }			 
			 while(!one_second_very_fast)
		 {
			 __WFE();
		 }
		 one_second_very_fast=0;
	 }
	 else
	 {
		 if(sw_time_very_fast_start)
		 {
			 DL_TimerA_stopCounter(TIMER_1_INST);	
		   DL_TimerA_clearInterruptStatus(TIMER_1_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);
		   DL_TimerA_disableInterrupt(TIMER_1_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);	
		   NVIC_ClearPendingIRQ(TIMER_1_INST_INT_IRQN);
	     NVIC_DisableIRQ(TIMER_1_INST_INT_IRQN);
			 sw_time_very_fast_start=0;
		 }
	 }
}

void timer_start(void)
{
	NVIC_ClearPendingIRQ(TIMER_3_INST_INT_IRQN);
  NVIC_EnableIRQ(TIMER_3_INST_INT_IRQN);
  DL_TimerG_clearInterruptStatus(TIMER_3_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);
  DL_TimerG_enableInterrupt(TIMER_3_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);	
  DL_TimerG_startCounter(TIMER_3_INST);
}
void timer_stop(void)
{
	DL_TimerG_stopCounter(TIMER_3_INST);	
  DL_TimerG_clearInterruptStatus(TIMER_3_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);
  DL_TimerG_disableInterrupt(TIMER_3_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);	
  NVIC_ClearPendingIRQ(TIMER_3_INST_INT_IRQN);
  NVIC_DisableIRQ(TIMER_3_INST_INT_IRQN);
}

void sw_time_normal_model_deal(void)
{
   if((!sw_time_fast)&&(!sw_time_very_fast))
	 {
		 if(!sw_time_normal_start)
		 {
			 NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
			 NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
		   DL_TimerA_clearInterruptStatus(TIMER_0_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);
       DL_TimerA_enableInterrupt(TIMER_0_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);	
       DL_TimerA_startCounter(TIMER_0_INST);
			 sw_time_normal_start=1;
		 }			 
	   	while(!one_second_normal)
		 {
			 __WFE();
		 }
			one_second_normal=0;
	 }
	 else
	 {
		  if(sw_time_normal_start)
			{
				DL_TimerA_stopCounter(TIMER_0_INST);	
		    DL_TimerA_clearInterruptStatus(TIMER_0_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);
		    DL_TimerA_disableInterrupt(TIMER_0_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);	
		    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
	      NVIC_DisableIRQ(TIMER_0_INST_INT_IRQN);
				
				sw_time_normal_start=0;
			}
	 }
}

void year_month_day_deal(void)
{
	  if(second>=86400)
			 {
				 second=0; 
				 switch(now_month)
				 {
					 case 1:
					 case 3:
					 case 5:
					 case 7:
					 case 8:
					 case 10:
					 case 12:
					 {
						 if(now_day==31)
						 {
							 if(now_month!=12)
							 {
								 now_month++;
							 }
							 else
							 {
								 now_month=1;
								 now_year++;
							 }
							 now_day=1;
						 }
						 else
						 {
							 now_day++;
						 }
						 break;
					 }
				
					 case 4:
					 case 6:
					 case 9:
					 case 11:
					 {
							if(now_day!=30)
							{
								now_day++;
							}						
							else
							{
								now_month++;
								now_day=1;
							}		
							break;									
					 }
					 
					 case 2:
					 {
							if(((now_year%4==0)&&(now_year%100!=0))||(now_year%400)==0)
							{
								 if(now_day!=29)
								 {
									 now_day++;
								 }				
								 else
								 {
									 now_month++;
									 now_day=1;
								 }								 
							}
							else
							{
								 if(now_day!=28)
								 {
									 now_day++;
								 }				
								 else
								 {
									 now_month++;
									 now_day=1;
								 }	
							}
							break;
					 }
		
					 default:break;
				 }
			 }
			 
}


void OLED_ShowDate(uint8_t x,uint8_t y,uint16_t year,uint8_t month,uint8_t day)
{
	 uint32_t leap_month_count=0;
	 uint32_t day_distance=0;
	 uint8_t  weekd=0;
	
   if(year>=2026)
	 {
			for(int i=base_year;i<=year;i++)  
			{
				if(((i%4==0)&&(i%100!=0))||(i%400==0))
				{
				  leap_month_count++;
				}
			}
			
			day_distance=(year-base_year-1)*365+month_day_total[month-1]+day;
			if((((year%4==0)&&(year%100!=0))||(year%400==0))&&(month<3))
			{
				day_distance+=leap_month_count-1;
			}
			else
			{
			  day_distance+=leap_month_count;
			}
			weekd=(base_weekday+day_distance)%7;
	 }
	 else if((year>=1)&&(year<2026))
	 {
	     for(int i=year;i<=base_year;i++)  
			{
				if(((i%4==0)&&(i%100!=0))||(i%400==0))
				{
				  leap_month_count++;
				}
			}
			day_distance=(base_year-year)*365+(365-month_day_total[month-1]-day);
			
			if(((year%4==0)&&(year%100!=0)||(year%400==0))&&(month>=3))
			{
				day_distance+=leap_month_count-1;
			}
			else
			{
			  day_distance+=leap_month_count;
			}
			
			if(day_distance%7<base_weekday)
			{
			  weekd=base_weekday-day_distance%7;
			}
			else if(day_distance%7==base_weekday)
			{
			  weekd=0;
			}
			else
			{
			  weekd=(7-day_distance%7)+base_weekday;
			}
	 }  
	
	  	OLED_ShowNum(x,y,year,4,16);
			OLED_ShowChar(x+8*4,y,'/');
	 
			if(month>=10&&month<=12)
			{
				OLED_ShowNum(x+8*5,y,month,2,16);
			} 
			else
			{
			  OLED_ShowNum(x+8*5,y,0,1,16);
				OLED_ShowNum(x+8*6,y,month,1,16);
			}
			OLED_ShowChar(x+8*7,y,'/');
			
			if((day>=10)&&(day<=31))
			{
			  OLED_ShowNum(x+8*8,y,day,2,16);
			}
			else
			{
			  OLED_ShowNum(x+8*8,y,0,1,16);
				OLED_ShowNum(x+8*9,y,day,1,16);
			}
			
			switch(weekd)
			{
				case 1:
				{
					OLED_ShowString(0,y+2,"Monday");
					break;
				}
				case 2:
				{
					OLED_ShowString(0,y+2,"Tuesday");
					break;
				}
				case 3:
				{
					OLED_ShowString(0,y+2,"Wednesday");
					break;
				}
				case 4:
				{
					OLED_ShowString(0,y+2,"Thursday");
					break;
				}
				case 5:
				{
				  OLED_ShowString(0,y+2,"Friday");
					break;
				}
				case 6:
				{
				  OLED_ShowString(0,y+2,"Saturday");
					break;
				}
				case 0:
				{
				  OLED_ShowString(0,y+2,"Sunday");
					break;
				}
				default:break;
			}
}

void GROUP1_IRQHandler()
{ 
  switch(DL_GPIO_getPendingInterrupt(GPIOA))
	{
		
	  case SW_SW1_IIDX://fast_model
		{
			if(DL_GPIO_readPins(SW_SW1_PORT,SW_SW1_PIN))
			{
				 DL_GPIO_setPins(LED_LED4_PORT,LED_LED4_PIN);
				 sw_time_fast=0;
			   count_model=1;
			}
			else
			{
				 DL_GPIO_clearPins(LED_LED4_PORT,LED_LED4_PIN);
				 sw_time_fast=1;
			   count_model=2;
			}
			DL_GPIO_clearInterruptStatus(GPIOA,SW_SW1_PIN);
			break;
	  }
		
		case SW_SW2_IIDX://very_fast_model
		{
			if(DL_GPIO_readPins(SW_SW2_PORT,SW_SW2_PIN))
			{
		  		 DL_GPIO_setPins(LED_LED5_PORT,LED_LED5_PIN);
				 sw_time_very_fast=0;
			   count_model=1;
			}
			else
			{
				 DL_GPIO_clearPins(LED_LED5_PORT,LED_LED5_PIN);
				 sw_time_very_fast=1;
			   count_model=3;
			}
			DL_GPIO_clearInterruptStatus(GPIOA,SW_SW2_PIN);
			break;
		}
		
		case SW_SW3_IIDX://setTime
		{
			sw3_update=1;
			if(DL_GPIO_readPins(SW_SW3_PORT,SW_SW3_PIN))
			{
				 sw_set_time=0;
			}
			else
			{
				 sw3_done=1;//标志校时事件
			   sw_set_time=1;	
		  }
			DL_GPIO_clearInterruptStatus(GPIOA,SW_SW3_PIN);
			break;
	  }
		
		case SW_SW4_IIDX://clear_flash
		{
		  if(DL_GPIO_readPins(SW_SW4_PORT,SW_SW4_PIN))
			{
				 sw_clear_flash=0;
			}
			else
			{
				 sw_clear_flash=1;
		  }
			DL_GPIO_clearInterruptStatus(GPIOA,SW_SW4_PIN);
			break;
		}
		
		case SW_SW5_IIDX://24_change_to_12
		{
			sw_change_update_oled=1;
			if(DL_GPIO_readPins(SW_SW5_PORT,SW_SW5_PIN))
			{
				 sw_time_to_12=0;
			}
			else
			{
				 sw_time_to_12=1;
		  }
			DL_GPIO_clearInterruptStatus(GPIOA,SW_SW5_PIN);
      break;		
		}
		
		case SW_SW6_IIDX://setClock
		{
			sw6_update=1;
		  if(DL_GPIO_readPins(SW_SW6_PORT,SW_SW6_PIN))
			{
				 sw_set_clock=0;
			}
			else
			{
				 sw_set_clock=1;
		  }
			DL_GPIO_clearInterruptStatus(GPIOA,SW_SW6_PIN);
		}
		
		case SW_SW7_IIDX://timer
		{
			sw7_update=1;
			if(DL_GPIO_readPins(SW_SW7_PORT,SW_SW7_PIN))
			{
				 timer_start_sign=0;
			}
			else
			{
				 timer_start_sign=1;//进入计时模式
		  }
			DL_GPIO_clearInterruptStatus(GPIOA,SW_SW7_PIN);
      break;		
		}
		
		default:break;
  }
}

void TIMER_0_INST_IRQHandler()
{
  switch(DL_TimerA_getPendingInterrupt(TIMER_0_INST))
	{
		case DL_TIMERA_IIDX_ZERO:
		{
			flash_save_overtime++;
			if(flash_save_overtime>=FLASH_UPDATING_TIME)
			{
				flash_save_overtime=0;
			}
			one_second_normal=1;
			DL_GPIO_togglePins(LED_LED1_PORT,LED_LED1_PIN);//功能1：LED灯没一秒闪烁一次
			DL_TimerA_clearInterruptStatus(TIMER_0_INST, DL_TIMERA_INTERRUPT_ZERO_EVENT);
			break;
		}
		default:break;
	}
}	

void TIMER_3_INST_IRQHandler()
{
  switch(DL_TimerG_getPendingInterrupt(TIMER_3_INST))
	{
		case DL_TIMERG_IIDX_ZERO:
		{
			switch(timer_key_press)
			{
			  case 'A':
				{
					timer_count++;
					timer_count%=1000;
				  break;
				}
				case 'B':
				{ 
				  break;
				}
				case 'C':
				{ 
					timer_count=0;
				  break;
				}
				default:
				{
					timer_count++;
					timer_count%=1000;
					break;
				}
			}
			DL_GPIO_togglePins(LED_LED6_PORT,LED_LED6_PIN);//功能1：LED灯没一秒闪烁一次
			DL_TimerG_clearInterruptStatus(TIMER_3_INST, DL_TIMERG_INTERRUPT_ZERO_EVENT);
			break;
		}
		default:break;
	}
}	

void TIMER_1_INST_IRQHandler()
{
  switch(DL_TimerA_getPendingInterrupt(TIMER_1_INST))
	{
		case DL_TIMERA_IIDX_ZERO:
		{
			one_second_very_fast=1;
			if(sw_time_very_fast)
			{
				DL_GPIO_togglePins(LED_LED3_PORT,LED_LED3_PIN);
			}
			else
			{
			  DL_GPIO_setPins(LED_LED3_PORT,LED_LED3_PIN);
			}
			DL_TimerA_clearInterruptStatus(TIMER_1_INST,DL_TIMERA_INTERRUPT_ZERO_EVENT);
			break;
		}
		default:break;
	}
}

void TIMER_2_INST_IRQHandler()
{
  switch(DL_TimerG_getPendingInterrupt(TIMER_2_INST))
	{
		case DL_TIMERG_IIDX_ZERO:
		{
			one_second_fast=1;
			if(sw_time_fast)
			{
				DL_GPIO_togglePins(LED_LED2_PORT,LED_LED2_PIN);
			}
			else
			{
			  DL_GPIO_setPins(LED_LED2_PORT,LED_LED2_PIN);
			}
			DL_TimerG_clearInterruptStatus(TIMER_2_INST,DL_TIMERG_INTERRUPT_ZERO_EVENT);
			break;
	 }
		default:break;
	}
}

void OLED_ShowTime(uint8_t x,uint8_t y,uint8_t count_model)
{
	if(count_model==1)
	{
	  OLED_ShowString(x,y,"Normal:");
	}
	else if(count_model==2)
	{
	  OLED_ShowString(x,y,"Fast:");
	}
	else if(count_model==3)
	{
	  OLED_ShowString(x,y,"Very fast:");
	}
	
	if(!sw_time_to_12)//24小时制
	{
		time_hour=second/3600;
		time_min=(second-time_hour*3600)/60;
	}
	else  //12小时制
	{
	  time_hour=second/3600;
		time_min=(second-time_hour*3600)/60;
		if(time_hour<=12)
		{}
		else
		{
		  time_hour-=12;
		}
	}
	time_second=second%60;
	
	if(time_hour>=10)
  {
		OLED_ShowNum(x,y+2,time_hour,2,16);
	}
	else
	{
		OLED_ShowNum(x,y+2,0,1,16);
		OLED_ShowNum(x+8,y+2,time_hour,1,16);
	}
	OLED_ShowChar(x+8*2,y+2,':');
	if(time_min>=10)
	{
		OLED_ShowNum(x+8*3,y+2,time_min,2,16);
	}
	else
	{
		OLED_ShowNum(x+8*3,y+2,0,1,16);
		OLED_ShowNum(x+8*4,y+2,time_min,1,16);
	}  
	OLED_ShowChar(x+8*5,y+2,':');
	if(time_second>=10)
	{
		OLED_ShowNum(x+8*6,y+2,time_second,2,16);
	}
	else
	{
		OLED_ShowNum(x+8*6,y+2,0,1,16);
	  OLED_ShowNum(x+8*7,y+2,time_second,1,16);
	}

	if(sw_time_to_12)
	{
		if((second/3600)<=11)
		OLED_ShowString(x+8*7+8*7,y+2,"AM");
		else
		OLED_ShowString(x+8*7+8*7,y+2,"PM");
	}
}

