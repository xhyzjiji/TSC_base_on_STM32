//#include "include_h_files.h"
//系统内部设备头文件
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "Sys_Config.h"
//#include "system_conf.h"
#include "stm32f10x_it.h"

//系统数据类型
#include "Traffic_Param.h"
#include "time.h"
#include "Delay.h"
#include "RA8806_CTRL.h"
#include "core_cm3.h"

//系统外部接口使用函数
#include "USART_OP.h"
#include "SPI_OP.h"
#include "TIME_OP.h"
#include "KEY_OP.h"
#include "LED_OP.h"
#include "E2PROM_OP.h"
#include "ENC28J60.h"

//外围器件使用函数
#include "netconf.h"
#include "ethernetif.h"
#include "stdio.h"

//Lwip
#include "lwip/err.h"
#include "lwip/dhcp.h"
#include "etharp.h"

//网络应用层
#include "tcp_server.h"
#include "tcp_client.h"
#include "arch/cc.h"

//全局变量区
#include "Ex_Var.h"
#include "global_val.h"

//系统标志位
uint8_t RTC_FLAG_SECSET;  //秒中断时置位
uint8_t RTC_FLAG_ALRSET;  //下一时段中断置位
uint8_t RTC_FLAG_HalfSecSET;  //半秒可调中断置位
uint8_t Time_FLAG_DayOrNight;  //每一个相位结束后检测当前时间
uint8_t Phase_FLAG_Start;
uint8_t Cycle_FLAG_Start;
uint8_t Init_FLAG_Finished;
uint8_t Manual_FLAG;
uint8_t No_UpdateLED_FLAG;

uint8_t LCD_FLAG_Using;
uint8_t LCD_FLAG_Oping;
uint8_t LCD_FLAG_UpdateIrq;

struct tm Current_Time;  //记录当前时间

struct Schedule_TypeDef my_Schedule;  //记录当前方案

//一些设置不允许马上被更新，如当前亮灯组合等，所以要进行存储，一些可马上反应的设置则可以直接修改到当前读取的方案中，
//这是一下全局变量设定的规则和原因
uint8_t Current_CutDown;
uint8_t Current_PhaseTable;
uint8_t Current_TimeTable;

uint16_t Current_Crossway_LED[6];
uint16_t temp_Current_Crossway_LED[6];
uint16_t temp_Current_Crossway_Yellow[6];
uint16_t Current_Crossway_Flash[6];
uint16_t Current_Crossway_Flash_Allowence[6];
PhaseTable_TypeDef Phase_OneShot;

uint8_t KEY_Buffer[KEYBuffer_Size];
uint8_t KEY_BufferPointer;
uint8_t KEY_BufferBottomOfStack;
uint8_t KEY_Press;

uint8_t Trg;
uint8_t Cont;
KEY_TypeDef CKey;
uint8_t press_cycle;

struct Menu_TypeDef* Current_Menu;  //当前指向的菜单指针
uint8_t CurrentMenu_ItemNumber;	 //当前菜单选项数目

struct Menu_TypeDef MainMenu;
struct MenuItem_TypeDef MainMenu_Items[MainMenu_ItemNum];
struct Menu_TypeDef PhaseMenu;
struct MenuItem_TypeDef PhaseMenu_Items[PhaseMenu_ItemNum];
struct Menu_TypeDef PeriodOfTimeMenu;
struct MenuItem_TypeDef PeriodOfTimeMenu_Items[PeriodOfTimeMenu_ItemNum];
struct Menu_TypeDef FlashMenu;
struct MenuItem_TypeDef FlashMenu_Items[FlashMenu_ItemNum];
struct Menu_TypeDef TimeSetMenu;
struct MenuItem_TypeDef TimeSetMenu_Items[TimeSetMenu_ItemNum];
struct Menu_TypeDef LEDColorMenu;
struct MenuItem_TypeDef LEDColorMenu_Items[LEDColorMenu_ItemNum];
struct Menu_TypeDef RTCSetMenu;
struct MenuItem_TypeDef RTCSetMenu_Items[RTCSetMenu_ItemNum];
struct Menu_TypeDef ManualModeMenu;
struct MenuItem_TypeDef ManualModeMenu_Items[ManualModeMenu_ItemNum];

uint16_t used_if_record[CROSSWAY_MAX];

time_syn time_update_status = DS1302_NEED_SYN;

uint8_t lcd_refresh_time;

/*__ASM uint32_t __R16BIT(uint16_t value)
{
  UXTH r0, r0
  rbit r0, r0
  bx lr
}

uint8_t addfunc(uint8_t x, uint8_t y, uint8_t z, uint8_t m, uint8_t n, uint8_t o)
{
	uint8_t p;
	p = x+y+z+m+n+o;
	return p;
}
*/
#define need_debug 0

int main(void)
{
	//uint8_t i;
	
	#if need_debug
	uint8_t a,b;
		
	//抓紧时间插入JTAG下载程序
	for(a=255; a>0; a--){
		for(b=255; b>0; b--){
			delay_us(20);
		}
	}
	#endif

	RCC_Config();
	#if need_debug
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  //上电复位后，IO口默认处于JTAG复用模式，关闭JTAG调试口，请拔除JTAG线
	#endif
	delay_us(1);

	NVIC_Config();

	GPIO_Config();
	GPIO_Initial();

	USART_Config();
	I2C1_Config();
	SPI1_Config();
	TIM_Config(TIM2);
	TIM_Basic_Config(TIM6);
	
	Var_Init();
	LCD_Init();
	LCD_IconInit();
	Menu_Init();
	LCD_InterfaceInit();

	Get_Schedule();
	LED_Init();
	LwIP_Init();
	EXTI_Config();

	TIM_Cmd(TIM6, ENABLE);

	tcp_server_init();
	tcp_client_request();

	//i=0;
	while(1){
		if(KEY_Press > 0 || lcd_refresh_time > 80){
			lcd_refresh_time = 0;
			if(Current_Menu->cur_funptr != NULL) Current_Menu->cur_funptr((KEY_TypeDef)KEY_Buffer[KEY_BufferPointer]);
		}
		lwip_eth_poll(LocalTime); 
	/*	printf("%d", i);
		i++;
		delay_us(10);*/
	}
		
}
