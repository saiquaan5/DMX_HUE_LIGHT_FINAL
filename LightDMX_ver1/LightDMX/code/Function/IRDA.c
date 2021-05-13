#include "IRDA.h"


void IRDA_Init(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |	RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	USART_SetPrescaler(USART1, 1);
	USART_IrDAConfig(USART1, USART_IrDAMode_Normal);
	USART_IrDACmd(USART1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	IR_enable(1);
}

void IR_enable(uint8_t en){
	if(en){
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	}else{
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	}
}

void IRWriteString(const char *txtString)
{
	while((*txtString)!='\0')
	{
	 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {};   
    USART_SendData(USART1,(uint8_t)*txtString++); 
	}
}

void IRWrite(uint8_t u8data){
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {};   
	USART_SendData(USART1,u8data);
}

void IRWriteWLength(const char *txtString)
{
	while((*txtString)!='\0')
	{
	 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {};   
    USART_SendData(USART1,(uint8_t)*txtString++); 
	}
}

void printir(const char *format,...){
    char temp[200];
    __va_list arg;
    va_start (arg, format);
    vsprintf (temp, format, arg);
    va_end (arg);
    IRWriteString(temp);
}
