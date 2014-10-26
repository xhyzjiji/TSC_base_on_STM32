/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stdio.h"
#include "USART_OP.h"
#include "RA8806_CTRL.h"
#include "KEY_OP.h"
#include "Delay.h"
#include "Ex_Var.h"
#include "time_op.h"
#include "LED_OP.h"

#include "stm32f10x.h"
#include "enc28j60.h"
#include "include_global_val.h"

#define SYSTEMTICK_PERIOD_MS  10

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
	lcd_refresh_time ++;
	LocalTime += SYSTEMTICK_PERIOD_MS;
	system_time ++;
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */
void EXTI0_IRQHandler(void)
{
	uint8_t EIR_dat = 0x00;
	
	EIR_dat = enc28j60Read(EIR);
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)				  //判别是否有数据包到达
	{
		if(EIR_dat&0x40){	//检测PKTIF中断请求标志位，接收中断
			ETH_INT=1;
			printf("PKTIF\n");
		}
		else if(EIR_dat&0x01){	//检测RXERIF中断请求标志位，接收错误中断
			ETH_INT=1;
			printf("RXERIF\n");		
		}
		else if(EIR_dat&0x10){
			//链路状态改变中断需要读取PHIR清除中断标志位
			printf("link state changed\n");
			if(enc28j60PhyRead(PHIR)&0x14){
				if(enc28j60PhyRead(PHSTAT2)&0x0400){
					printf("link activate\n");
				}
				else{
					printf("link invalid\n");
					//enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
				}
			}
		}
		//ethernetif_input(网口名);								  //设置接收完成标志
		EXTI_ClearITPendingBit(EXTI_Line0);					  //清除中断请求标志
	}
}

void EXTI1_IRQHandler(void)
{
	if(LCD_FLAG_UpdateIrq == 1){
		LCD_FLAG_UpdateIrq = 0;
		LCD_UpdateInfo();
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

/*
void EXTI15_10_IRQHandler(void)
{
	//测试专用代码
	volatile uint8_t xtemp;
	volatile uint8_t ytemp;
	volatile uint8_t MAMR_temp;
	volatile uint8_t WLCR_temp;
	volatile uint8_t MISC_temp;
	volatile uint8_t apptop, appbottom, appleft, appright;
	volatile uint8_t FNCR_temp;
	
	//EXTI_GetFlagStatus(***)只是单纯检测中断标志位是否置位，并不代表有中断请求
	//EXTI_GetITStatus(***)检测某一线路是否产生了中断
	if(EXTI_GetFlagStatus(EXTI_Line10) == SET){
		//delay_100ns(500);
		delay_us(1800);
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10)==(uint8_t)Bit_RESET){
			//printf("KEY1 Press\n");
			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
				KEY_Press ++;
				KEY_Buffer[KEY_BufferBottomOfStack] = KEY1;	 //RIGHT
				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
					KEY_BufferBottomOfStack = 0;
				}
				else{
					KEY_BufferBottomOfStack ++;
				}
			}
			else{  //KEY_Press == KEYBuffer_Size
				//忽略此次按键操作
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);	//防止按键共同触发
		return;
	}
	else if(EXTI_GetFlagStatus(EXTI_Line11) == SET){
		//delay_100ns(500);
		delay_us(1800);
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11)==(uint8_t)Bit_RESET){
			//printf("KEY2 Press\n");
			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
				KEY_Press ++;
				KEY_Buffer[KEY_BufferBottomOfStack] = KEY2;	  //UP
				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
					KEY_BufferBottomOfStack = 0;
				}
				else{
					KEY_BufferBottomOfStack ++;
				}
			}
			else{  //KEY_Press == KEYBuffer_Size
				//忽略此次按键操作
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);
		return;
	}
	else if(EXTI_GetFlagStatus(EXTI_Line12) == SET){
		//delay_100ns(500);
		delay_us(1800);
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12)==(uint8_t)Bit_RESET){
			//printf("KEY3 Press\n");
			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
				KEY_Press ++;
				KEY_Buffer[KEY_BufferBottomOfStack] = KEY3;	  //DOWN
				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
					KEY_BufferBottomOfStack = 0;
				}
				else{
					KEY_BufferBottomOfStack ++;
				}
			}
			else{  //KEY_Press == KEYBuffer_Size
				//忽略此次按键操作
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);
		return;
	}
	else if(EXTI_GetFlagStatus(EXTI_Line13) == SET){
		//delay_100ns(500);
		delay_us(1800);
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13)==(uint8_t)Bit_RESET){
			printf("KEY4 Press\n");
			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
				KEY_Press ++;
				KEY_Buffer[KEY_BufferBottomOfStack] = KEY4;	  //ENTER
				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
					KEY_BufferBottomOfStack = 0;
				}
				else{
					KEY_BufferBottomOfStack ++;
				}
			}
			else{  //KEY_Press == KEYBuffer_Size
				//忽略此次按键操作
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);
		return;
	}
	else if(EXTI_GetFlagStatus(EXTI_Line14) == SET){
		//delay_100ns(500);
		delay_us(1800);
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14)==(uint8_t)Bit_RESET){
			//printf("KEY5 Press\n");
			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
				KEY_Press ++;
				KEY_Buffer[KEY_BufferBottomOfStack] = KEY5;	  //ESC
				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
					KEY_BufferBottomOfStack = 0;
				}
				else{
					KEY_BufferBottomOfStack ++;
				}
			}
			else{  //KEY_Press == KEYBuffer_Size
				//忽略此次按键操作
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);
		return;
	}
	else{
		//delay_100ns(500);
		delay_us(1800);
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15)==(uint8_t)Bit_RESET){
			//printf("KEY6 Press\n");
			
			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
				KEY_Press ++;
				KEY_Buffer[KEY_BufferBottomOfStack] = KEY6;	  //LEFT
				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
					KEY_BufferBottomOfStack = 0;
				}
				else{
					KEY_BufferBottomOfStack ++;
				}
			}
			else{  //KEY_Press == KEYBuffer_Size
				//忽略此次按键操作
			}
			
			//测试专用代码
			//xtemp = LCD_ReadREG(CURX);
			//ytemp = LCD_ReadREG(CURY);
			//MAMR_temp = LCD_ReadREG(MAMR);
			//WLCR_temp = LCD_ReadREG(WLCR);
			//MISC_temp = LCD_ReadREG(MISC);
			//apptop = LCD_ReadREG(AWTR);
			//appbottom = LCD_ReadREG(AWBR);
			//appleft = LCD_ReadREG(AWLR);
			//appright = LCD_ReadREG(AWRR);
			//FNCR_temp = LCD_ReadREG(FNCR);
		}
		EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);
		return;
	}
}
*/

void RTC_IRQHandler(void)  //整秒操作
{
	if(RTC_GetFlagStatus(RTC_FLAG_SEC) == SET){
		TIM_Cmd(TIM2, ENABLE);
		RTC_ClearITPendingBit(RTC_FLAG_SEC);

		RTC_FLAG_SECSET = 1;
		Current_Time = Time_ConvUnixToCalendar(Time_GetUnixTime());	  //每秒获取一次当前时间供当前运行信息
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, ((GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0))==(uint8_t)Bit_SET)?Bit_RESET:Bit_SET);

		if(Init_FLAG_Finished == 1 && Manual_FLAG == 0 && No_UpdateLED_FLAG == 0){
			LED_Sec_Display();
			if(LCD_FLAG_Using == 0 && LCD_FLAG_Oping == 0){
				LCD_UpdateInfo();	//刷新屏幕信息
			}
			else{
				LCD_FLAG_UpdateIrq = 1;  //申请刷新屏幕信息
			}
		}
		else if(Manual_FLAG == 1){
			LED_Sec_Display();
			if(LCD_FLAG_Using == 0 && LCD_FLAG_Oping == 0){
				LCD_UpdateInfo();	//刷新屏幕信息
			}
			else{
				LCD_FLAG_UpdateIrq = 1;  //申请刷新屏幕信息
			}
		}
		else if(No_UpdateLED_FLAG == 1){
			if(LCD_FLAG_Using == 0 && LCD_FLAG_Oping == 0){
				LCD_UpdateInfo();	//刷新屏幕信息
			}
			else{
				LCD_FLAG_UpdateIrq = 1;  //申请刷新屏幕信息
			}
		}
		return;
	}
	if(RTC_GetFlagStatus(RTC_FLAG_ALR) == SET){
		RTC_ClearITPendingBit(RTC_FLAG_ALR);
	}
	return;
}

void TIM2_IRQHandler(void)	//半秒操作
{
	if(TIM_GetFlagStatus(TIM2, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		RTC_FLAG_HalfSecSET = 1;

		if(Init_FLAG_Finished == 1 && Manual_FLAG == 0 && No_UpdateLED_FLAG == 0){
			LED_HalfSec_Display();
			if(Current_CutDown == 0){
			Phase_FLAG_Start = 1;  //相位开始标志
			//判断昼夜-一相位检测一次
			Time_JudgeDayOrNight();
	
			//检测是否完成周期，否-继续完成这个周期，是-检测时段是否要更改
			if(Current_PhaseTable < (Phase_OneShot.Phase_Number-1)){
				Current_PhaseTable ++;
			}
			else{  //Current_PhaseTable == (my_Schedule.Phase_OneTable[Current_TimeTable].Phase_Number-1)
				Cycle_FLAG_Start = 1;	//周期开始标志
				Current_PhaseTable = 0;
				
				//周期开始，检测是否进入下一个时段
				//日交替时，如何判断？
				Time_JudgeTimeTable();

				Phase_OneShot = my_Schedule.PhaseTable[Current_TimeTable];
			}
	
			Current_CutDown = Phase_OneShot.Last_Time[Current_PhaseTable];
	
			LED_Interpretation(Current_TimeTable, Current_PhaseTable);
			}
			else{
				Current_CutDown --;
			}
		}

		else if(Manual_FLAG == 1){
////////////进入手动调试模式，获取时段与相位方法改变
			LED_HalfSec_Display();
			
			if(Current_CutDown == 0){
				Time_JudgeDayOrNight();
				Current_CutDown = my_Schedule.PhaseTable[Current_TimeTable].Last_Time[Current_PhaseTable];
			}
			else{
				Current_CutDown --;
			}			
		}

		else if(No_UpdateLED_FLAG == 1){
		}

		//GPIO_WriteBit(GPIOA, GPIO_Pin_0, ((GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0))==(uint8_t)Bit_SET)?Bit_RESET:Bit_SET);
		return;
	}
	return;
}

void TIM6_IRQHandler(void)
{
	uint8_t cont_temp;
	uint16_t gpioc_data;

	//printf("KEY_SCAN\n");
	gpioc_data = GPIO_ReadInputData(GPIOC);
	
	if(TIM_GetFlagStatus(TIM6, TIM_IT_Update) == SET){
		//printf("TIM6_20ms_Interrupt\n");
		//GPIOC->ODR ^= 0x00000200;
		
		cont_temp = Cont;  //上一次的按键状态

		//Trg = ((~(GPIO_ReadInputData(GPIOC))>>8)&0x3F) & (~Cont);
		//Cont = (~(GPIO_ReadInputData(GPIOC))>>8)&0x3F;

		Trg = ((~gpioc_data>>8)&0x3F) & (~Cont);
		Cont = (~gpioc_data>>8)&0x3F;

		if(Trg != 0x00){  //短按应改为弹起触发
			switch(Trg){  
				case 0x01:  CKey = KEY2;  break;
				case 0x02:  CKey = KEY1;  break;
				case 0x04:  CKey = KEY3;  break;
				case 0x08:  CKey = KEY4;  break;
				case 0x10:  CKey = KEY5;  break;
				case 0x20:  CKey = KEY6;  break;
				default:  CKey = Nothing;  break;
			}
			press_cycle = 0;
		}
		else{  //Trg == 0x00
			if(Cont != 0x00){
				if(cont_temp == Cont){
					if(press_cycle>10){
						switch(Cont){
							case 0x01:  CKey = KEY2_Cont;  break;
							case 0x02:  CKey = KEY1_Cont;  break;
							case 0x04:  CKey = KEY3_Cont;  break;
							case 0x08:  CKey = KEY4_Cont;  break;
							case 0x10:  CKey = KEY5_Cont;  press_cycle = 0; break;
							case 0x20:  CKey = KEY6_Cont;  press_cycle = 0; break;
							case 0x03:  CKey = KEY1_2;  break;
							case 0x05:  CKey = KEY2_3;  break;
							case 0x09:  CKey = KEY2_4;  break;
							case 0x06:  CKey = KEY1_3;  break;
							case 0x0A:  CKey = KEY1_4;  break;
							case 0x0C:  CKey = KEY3_4;  break;
							default:  CKey = Nothing;  break;
						}
					}
					else{
						press_cycle ++;
						CKey = Nothing;
					}
				}
			}
			else{
				press_cycle = 0;
				CKey = Nothing;
			}
		}

		if(CKey != Nothing){
			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
				KEY_Buffer[KEY_BufferBottomOfStack] = CKey;	 //RIGHT
				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
					KEY_BufferBottomOfStack = 0;
				}
				else{
					KEY_BufferBottomOfStack ++;
				}
				KEY_Press ++;
				//if(KEY_Press<KEYBuffer_Size) KEY_Buffer[KEY_BufferBottomOfStack] = Nothing;
			}
		}
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
	return;
}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
