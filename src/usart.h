#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "stdio.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����1��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/5/2
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.1 20150411
//�޸�OS_CRITICAL_METHOD���ж�Ϊ��SYSTEM_SUPPORT_OS
////////////////////////////////////////////////////////////////////////////////// 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern unsigned char  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern unsigned short USART_RX_STA;         		//����״̬���	

void uart_init(unsigned int pclk2,unsigned int bound); 
#endif	   
















