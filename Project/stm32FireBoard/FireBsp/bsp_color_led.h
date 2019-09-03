#ifndef __COLOR_LED_H
#define	__COLOR_LED_H


#include "stm32f1xx_hal.h"



/********************��ʱ��ͨ��**************************/

#define   COLOR_TIMx                      TIM3

#define   COLOR_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd
#define   COLOR_TIM_CLK                   RCC_APB1Periph_TIM3
#define   COLOR_TIM_GPIO_CLK              (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)

//��Ƶ�������Ҫ��ӳ��
#define   COLOR_GPIO_REMAP_FUN()						GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 				

/************���***************/
#define  COLOR_RED_TIM_LED_PORT               GPIOB
#define  COLOR_RED_TIM_LED_PIN                GPIO_Pin_5

#define  COLOR_RED_TIM_OCxInit                TIM_OC2Init            //ͨ����ʼ������
#define  COLOR_RED_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig //ͨ���������ú���

//ͨ���ȽϼĴ�������TIMx->CCRx��ʽ�ɷ��ʸüĴ����������µıȽ�ֵ������ռ�ձ�
//�Ժ��װ��ʹ��������ʽ��COLOR_TIMx->COLOR_RED_CCRx ���ɷ��ʸ�ͨ���ıȽϼĴ���
#define  COLOR_RED_CCRx                       	CCR2		

/************�̵�***************/

#define  COLOR_GREEN_TIM_LED_PORT               GPIOB
#define  COLOR_GREEN_TIM_LED_PIN                GPIO_Pin_0

#define  COLOR_GREEN_TIM_OCxInit                TIM_OC3Init            //ͨ����ʼ������
#define  COLOR_GREEN_TIM_OCxPreloadConfig       TIM_OC3PreloadConfig //ͨ���������ú���

//ͨ���ȽϼĴ�������TIMx->CCRx��ʽ�ɷ��ʸüĴ����������µıȽ�ֵ������ռ�ձ�
//�Ժ��װ��ʹ��������ʽ��COLOR_TIMx->COLOR_GREEN_CCRx ���ɷ��ʸ�ͨ���ıȽϼĴ���
#define  COLOR_GREEN_CCRx                       CCR3

/************����***************/
#define   COLOR_BLUE_TIM_LED_PORT             GPIOB
#define   COLOR_BLUE_TIM_LED_PIN              GPIO_Pin_1

#define   COLOR_BLUE_TIM_OCxInit              TIM_OC4Init            //ͨ����ʼ������
#define   COLOR_BLUE_TIM_OCxPreloadConfig    TIM_OC4PreloadConfig  //ͨ���������ú���

//ͨ���ȽϼĴ�������TIMx->CCRx��ʽ�ɷ��ʸüĴ����������µıȽ�ֵ������ռ�ձ�
//�Ժ��װ��ʹ��������ʽ��COLOR_TIMx->COLOR_BLUE_CCRx ���ɷ��ʸ�ͨ���ıȽϼĴ���
#define   COLOR_BLUE_CCRx                      CCR4



void COLOR_TIMx_LED_Init(void);
void COLOR_TIMx_LED_Close(void);
void SetRGBColor(uint32_t rgb);
void SetColorValue(uint8_t r,uint8_t g,uint8_t b);
uint8_t GetColorValue(uint8_t iRgb);

void SetColorVal(uint8_t iCol, uint16_t rgb);

#endif /* __COLOR_LED_H */

