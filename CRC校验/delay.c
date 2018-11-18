
/******************** (C) COPYRIGHT 2010 SLZR-SZ Tech Co.,LTD. ************
* File Name          : delay.c
* Author             : SLZR sz,ksli
* Date First Issued  : 31/12/2010
* Description        : SPI Inteface file
*******************************************************************************
*******************************************************************************
* History:
* 05/01/2011		 : V1.0		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "serial.h"
#include "stm32f4xx.h"
#include "lib_ztr.h"
#include "stm32f4xx_tim.h"


__IO unsigned int timer;//����delayxms����
__IO unsigned int timm[3];//�����ⲿ�¼���ʱ�ж�

//����ܴ��н�
//PE
#define LED_DAT			GPIO_Pin_5		//PE5
#define LED_CLK			GPIO_Pin_6		//PE6
#define LED_DATCLK_GPIO	GPIOE  
//����ܹ����˿��� 
//PD
#define LED_DG1			GPIO_Pin_9       //PB9
#define LED_DG1_GPIO	GPIOB  
//PE
#define LED_DG2			GPIO_Pin_0		//PE0	
#define LED_DG3			GPIO_Pin_1
#define LED_DG4			GPIO_Pin_2
#define LED_DG5			GPIO_Pin_3
#define LED_DG6			GPIO_Pin_4
#define LED_DG26_GPIO	GPIOE  

char LED_Value[6];//�����������ʾ�ַ�
volatile unsigned char LED_V_INDEX=0;//��ǰˢ�µ������
__IO unsigned int ledDisdly;

#define DIS_NUM 18
//Ҫ����ַ���ʾ��������ں���
const unsigned char LED_TABLE[DIS_NUM]={0xEB,0x28,0xB3,0xBA,0x78,0xDA,0xDB,0xA8,0xFB,0xFA, \
									0x10,0x02,0xF9,0xD3,0xD1,0x79,0x00,0xC3};//0-9,-,_,A,E,F,H, ,C,
extern void Delay(unsigned int ticks);
extern void delayus ( void );

//////////////////�������ʾ/////////////////////////

//��λ�����
void LED_SEND_Byte(unsigned char dat)
{
	unsigned char i;
	GPIO_ResetBits(LED_DATCLK_GPIO, LED_CLK);
	for(i=0;i<8;i++){
		if(dat & 0x80)
			GPIO_SetBits(LED_DATCLK_GPIO, LED_DAT);
		else
			GPIO_ResetBits(LED_DATCLK_GPIO, LED_DAT);
//		delayus();
		GPIO_SetBits(LED_DATCLK_GPIO, LED_CLK);
//		delayus();
		dat <<= 1;
		GPIO_ResetBits(LED_DATCLK_GPIO, LED_CLK);
	}
}
//��ʼ������ܶ˿�
void LED_3INT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure the GPIO_LED pin */
GPIO_InitStructure.GPIO_Pin = (LED_DAT|LED_CLK);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_DATCLK_GPIO, &GPIO_InitStructure);
	GPIO_ResetBits(LED_DATCLK_GPIO, (LED_DAT|LED_CLK));

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = LED_DG1;
	GPIO_Init(LED_DG1_GPIO, &GPIO_InitStructure);
	GPIO_ResetBits(LED_DG1_GPIO, LED_DG1);

	GPIO_InitStructure.GPIO_Pin = LED_DG2|LED_DG3|LED_DG4|LED_DG5|LED_DG6;
	GPIO_Init(LED_DG26_GPIO, &GPIO_InitStructure);
	GPIO_ResetBits(LED_DG26_GPIO, LED_DG2|LED_DG3 );

// ���ö�ʱ����40ms�ж�һ��	
//	SysTick_Config(2880000);//(SystemFrequency / 1000)*40
//	SysTick_Config(720000);//(SystemFrequency / 1000)*10
//	SysTick_Config(22000);//(SystemFrequency / 1000)*10 0.3ms
	SysTick_Config(52000);  //52000, F407,0.3ms
	
	memset(LED_Value, ' ', 6);
	LED_V_INDEX = 0;
	LED_SEND_Byte(0x14);
}

extern __IO unsigned int XF_Sound_dly;
extern __IO unsigned int sound_dly;

extern __IO unsigned int beep_nNum;
extern __IO unsigned int beep_Ymun;
extern __IO unsigned int beep_enable;
extern __IO unsigned char beep_times;
extern __IO unsigned int beep_delay;

//#define BEEP_IO			GPIO_Pin_13        //PC13,PC14,PC15��ʹ�����ޣ�ֻ��ʹ��2M�ٶ�
//#define BEEP_IO_GPIO	GPIOC
//#define LED_RED ((unsigned char)1)

extern void led_convert(unsigned char type);
extern void audio_TPA2005_Control(unsigned char flag);
extern void XF_Sound_Control(unsigned char flag);
//�����������ʾ��ˢ������ܣ�41msˢ��һ��
void SysTick_Handler(void)
{
//POC
#define SOUND_PR1	GPIO_Pin_2        //PC2
#define SOUND4_PIO	GPIO_Pin_3        //PC3
#define SOUND7_PIO	GPIO_Pin_1        //PC1
#define SOUND8_PIO	GPIO_Pin_0        //PC0

//POA
#define SOUND1_PIO	GPIO_Pin_4        //PA4
#define SOUND2_PIO	GPIO_Pin_1		  //PA1	
#define SOUND3_PIO	GPIO_Pin_0        //PA0
#define SOUND5_PIO	GPIO_Pin_6		  //PA6
#define SOUND6_PIO	GPIO_Pin_5		  //PA5

	unsigned char ret;
	
	if(sound_dly > 0){	//����
		sound_dly--;
		if(sound_dly == 0){
			GPIO_SetBits(GPIOC, SOUND_PR1);
// 			GPIO_ResetBits(GPIOC, SOUND_PR1|SOUND4_PIO|SOUND7_PIO|SOUND8_PIO);
// 			GPIO_ResetBits(GPIOA, SOUND1_PIO|SOUND2_PIO|SOUND3_PIO|SOUND5_PIO|SOUND6_PIO);
			audio_TPA2005_Control(0);
		}
	}
	
//POC
#undef SOUND_PR1
#undef SOUND4_PIO
#undef SOUND8_PIO
#undef SOUND7_PIO

//POA
#undef SOUND1_PIO
#undef SOUND2_PIO
#undef SOUND3_PIO
#undef SOUND5_PIO
#undef SOUND6_PIO

	if (XF_Sound_dly>0) {
		XF_Sound_dly--;
		if (XF_Sound_dly==0) {
			XF_Sound_Control(0);
		}
	}


	if(beep_enable){//��Ҫ��
		GPIO_SetBits(BEEP_IO_GPIO, BEEP_IO);
		beep_enable--;
	}
	else{
		if(beep_times > 1){//����һ�Σ����ǻ���1�����ϡ�
			if(beep_delay){//������ʱ
				GPIO_ResetBits(BEEP_IO_GPIO, BEEP_IO);
				beep_delay--;
			}
			else{
				beep_times--;
				beep_delay = beep_nNum;
				beep_enable = beep_Ymun;
			}
		}
		else if(beep_times == 1){//�Ѿ�����һ���ˣ��ر�BEEP
			GPIO_ResetBits(BEEP_IO_GPIO, BEEP_IO);
			beep_times = 0;
		}
	}

	ledDisdly++;
	if(ledDisdly >= 15)
		ledDisdly = 0;
	else
		return;

	GPIO_SetBits(LED_DG1_GPIO,   LED_DG1);//�ر���ʾ��������˸
	GPIO_SetBits(LED_DG26_GPIO,  LED_DG2|LED_DG3|LED_DG4|LED_DG5|LED_DG6);

	if(LED_V_INDEX >= 6)
		LED_V_INDEX = 0;
	for (ret = 0; ret < 6; ret++)
	{
		if((LED_Value[LED_V_INDEX] == ' ')||(LED_Value[LED_V_INDEX] == 0)){//�ո�Ӧ�ò���ʾ,����һ�ַ�
			LED_V_INDEX++;	
			if(LED_V_INDEX >= 6)
				LED_V_INDEX = 0;
		}
		else
			break;
	}
	if(ret >= 6)
		return;//û��Ҫ��ʾ�����ݣ�ȫ�ǿո�

	ret = ((LED_Value[LED_V_INDEX] & 0x7F)-'0');
	if(ret > (DIS_NUM-1))
		ret = 16;
	ret = LED_TABLE[ret];
	if(LED_Value[LED_V_INDEX] & 0x80)//����С����
		ret |= 0x04;
	
	LED_SEND_Byte((~ret));//��������ܣ�Ҫȡ��..
	
// 	debugdata((unsigned char*)&LED_V_INDEX, 1, 0);
// 	debugstring(":");
// 	debugdata((unsigned char*)&ret, 1, 0);

	switch(LED_V_INDEX) {
	case 0:
		GPIO_ResetBits(LED_DG1_GPIO, LED_DG1);
		break;
	case 1:
		GPIO_ResetBits(LED_DG26_GPIO, LED_DG2);
		break;
	case 2:
		GPIO_ResetBits(LED_DG26_GPIO, LED_DG3);
		break;
	case 3:
		GPIO_ResetBits(LED_DG26_GPIO, LED_DG4);
		break;
	case 4:
		GPIO_ResetBits(LED_DG26_GPIO, LED_DG5);
		break;
	case 5:
		GPIO_ResetBits(LED_DG26_GPIO, LED_DG6);
		break;
	default:
		break;
	}
	LED_V_INDEX++;//ָ����һ�����
}

//��ʾ�ַ�����С���㣬�����ʾ�����ַ������ÿ�����ֶ���С����Ϊ12���ַ�
void LED_Dis3(char *str)
{
	unsigned char i;
	unsigned char index=0;
	
	memset(LED_Value, ' ', 6);
	for(i=0; i<6; i++){// ���������������ʾ
		if(str[index] == 0){//������
			break;
		}

		if(str[index] == '.'){//�����һ������ܣ����i==0��
			LED_Value[i] = ('0'|0x80);//��ʾ0.
			index++;//ָ����һ���������ʾ������
		}
		else{
			switch (str[index])
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				LED_Value[i] = str[index];
				break;
			case '-':
				LED_Value[i] = '9'+1;
				break;
			case '_':
				LED_Value[i] = '9'+2;
				break;
			case 'A':
				LED_Value[i] = '9'+3;
				break;
			case 'E':
				LED_Value[i] = '9'+4;
				break;
			case 'F':
				LED_Value[i] = '9'+5;
				break;
			case 'H':
				LED_Value[i] = '9'+6;
				break;
			case 'C':
				LED_Value[i] = '9'+8;
				break;
			case ' ':
				LED_Value[i] = '9'+7;
				break;
			default:
				LED_Value[i] = '9'+7;
				break;
			}
			index++;
			if(str[index] == '.'){//�ǲ�����Ҫ��С����
				LED_Value[i] |= 0x80;
				index++;
			}
		}
	}
// 	debugstring("LED_DIS6:");
// 	debugdata((unsigned char*)LED_Value, 6, 1);
}
//////////////////�������������ʾ/////////////////////////

void TIM2_IRQHandler(void)
{
    if(0<(timer))
    {
		timer -= 1;
    }
	if(0<timm[0])
		timm[0]--;
	if(0<timm[1])
		timm[1]--;
	if(0<timm[2])
		timm[2]--;
	if((timer==0)&&(timm[0]==0)&&(timm[1]==0)&&(timm[2]==0)){
		TIM_Cmd(TIM2, DISABLE);
	}
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}
/*---------------------------------------------------------
 Function Name       : timer_init
 Object              : Init timer counter
 Input Parameters    : none
 Output Parameters   : TRUE
-----------------------------------------------------------*/
void timer0_init (void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	uint16_t PrescalerValue;
	//Step1.ʱ�����ã�����TIM1 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
//1.����RCC��ʹ��TIMx
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//2.����NVIC���������ȼ��ȣ�Ҳ����ʹ�ò�ѯ�������������ǽ������ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//3.����TIMx
//����Ԥ��Ƶ����Ƶϵ��71����APB2=72M, TIM1_CLK=72/72=1MHz  APB1=36M, TIM1_CLK=36/36=1MHz
//TIM_Period��TIM1_ARR��=1000�����������ϼ�����1000����������¼�������ֵ���� 
//���ü������������
	TIM_TimeBaseStructure.TIM_Period = 2000;//��ô��Ҫ2��?
//Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_Prescaler = 83;  //41
//���ü�������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//���жϣ�����һ�����жϺ����������ж� 
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, DISABLE);

	timm[0] = 0;
	timm[1] = 0;
	timm[2] = 0;
	timer = 0;

}

void timer0_close(void)
{
	//* stop timer0
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_Cmd(TIM2, DISABLE);
}
/*---------------------------------------------------------
NAME: SET TIMER
FUNC������ȫ�ֵ�TIME[] ����
-----------------------------------------------------------*/
void set_timer(unsigned int tick)
{//����TIME ʱ��
	timer = tick ;
//	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_Cmd(TIM2, ENABLE);
}

void set_timer0(unsigned int tick, unsigned char mode)
{//����TIME ʱ��
	if(mode > 2)
		mode = 2;
	timm[mode] = tick;
//	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_Cmd(TIM2, ENABLE);
}

/*---------------------------------------------------------
              �õ���ʱ��ʱ��
-----------------------------------------------------------*/
unsigned int get_timer(void)
{
	return timer;
}
unsigned int get_timer0(unsigned char mode)
{
	if(mode > 2)
		mode = 2;
	return timm[mode];
}

//#define WAIT_TIME 72000000
#define WAIT_TIME 168000000
void delay ( void )
{
//* Set in Volatile for Optimisation
    volatile unsigned int    i ;
//* loop delay
    for ( i = 0 ;(i < WAIT_TIME/100 );i++ ) ;
}

extern void clr_wdt(void);
/***************************************************
* ���ܣ�ms��ʱ����
* ���룺xms-��ʱ��ֵ
* �����NULL
****************************************************/
void delayxms(unsigned int xms)
{
	set_timer(xms);
	while(1){
		clr_wdt();
		if(get_timer()==0){
			return;
		}
	}
}
/***************************************************
* ���ܣ�s��ʱ����
* ���룺xms-��ʱ��ֵ
* �����NULL
****************************************************/
void delayxs(unsigned int xs)
{
	unsigned int i;
	i = 1000*xs;
	set_timer(i);
	while(1){
		clr_wdt();
		if(get_timer()==0){
			return;
		}
	}
}
