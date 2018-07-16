  
	#include "sys.h"
#include "usart.h"	
#include "stm32f4xx.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
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
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���         

struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;
FILE __stdin;
FILE __stderr;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	USART2->DR = (unsigned char) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
unsigned char USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
unsigned short USART_RX_STA=0;       //����״̬���	  
  
void USART2_IRQHandler(void)
{
	unsigned char res;	
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=USART2->DR; 
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	} 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif										 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
void uart_init(unsigned int pclk2,unsigned int bound)
{  	 
	float temp;
	unsigned short mantissa;
	unsigned short fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV@OVER8=0
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTA��ʱ��  
	RCC->APB1ENR|=1<<17;  	//ʹ�ܴ���2ʱ�� 
	
	GPIOA->MODER &= ~((3UL << 2*2 ));
	GPIOA->MODER |= ((2UL << 2*2 ));                /* PA2 is output                     */
	
	GPIOA->OSPEEDR &= ~((3UL << 2*2 ));
	GPIOA->OSPEEDR |= ((2UL << 2*2 ));              /* PA2 is 50MHz Fast Speed           */
	
	GPIOA->OTYPER &= ~((1UL << 2 ));
	GPIOA->OTYPER |= ((0UL << 2 ));               /* PA2 is output Push-Pull           */
	
	GPIOA->PUPDR &= ~((3UL << 2*2 ));
	GPIOA->PUPDR |= ((1UL << 2*2 ));                /* PA2 is Pull down                  */
	
	GPIOA->MODER &= ~((3UL << 2*3 ));
	GPIOA->MODER |= ((2UL << 2*3 ));                /* PA3 is output                     */
	
	//GPIOA->OSPEEDR &= ~((3UL << 2*3 ));
	//GPIOA->OSPEEDR |= ((2UL << 2*3 ));              /* PA3 is 50MHz Fast Speed           */
	
	//GPIOA->OTYPER &= ~((1UL << 3 ));
	//GPIOA->OTYPER |= ((0UL << 3 ));               /* PA3 is output Push-Pull           */
	
	GPIOA->PUPDR &= ~((3UL << 2*3 ));
	GPIOA->PUPDR |= ((0UL << 2*3 ));                /* PA3 is Pull down                  */
	
	
	
	//GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	//GPIO_AF_Set(GPIOA,2,7);	//PA2,AF7
	//GPIO_AF_Set(GPIOA,3,7);//PA3,AF7  	   
	GPIOA->AFR[1] |= ((2UL << 4*7 ));
	GPIOA->AFR[1] |= ((3UL << 4*7 ));  
	
	//����������
 	USART2->BRR=mantissa; 	//����������	 
	USART2->CR1&=~(1<<15); 	//����OVER8=0 
	USART2->CR1|=1<<3;  	//���ڷ���ʹ�� 
#if EN_USART2_RX		  	//���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<2;  	//���ڽ���ʹ��
	USART2->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 
#endif
	USART2->CR1|=1<<13;  	//����ʹ��
}














