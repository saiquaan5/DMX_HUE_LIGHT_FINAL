

#ifndef _IRDA_H_
#define _IRDA_H_

#include "stm32f10x.h"
#include "serial_tracer.h"
#include <stdio.h >
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>


void IR_enable(uint8_t en);
void IRDA_Init(void);
void printir(const char *format,...);
void IRWrite(uint8_t u8data);
#endif