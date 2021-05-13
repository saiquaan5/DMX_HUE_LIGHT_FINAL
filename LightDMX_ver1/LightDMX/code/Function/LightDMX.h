#ifndef LIGHTDMX_H
#define LIGHTDMX_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdlib.h>
#include "IRDA.h"

#define MAX_PULSE	254
#define DMX_VALUE_SIZE 3


#define CMD_Write_Add_To_Light                      200
#define CMD_Read_Add_From_Light                     201
#define CMD_Test_Red                                202
#define CMD_Test_Green                              203
#define CMD_Test_Blue                               204
#define CMD_Test_All                                205
#define CMD_Trans_Limit_Power                       206
#define CMD_Test_OFF              									207
#define CMD_Respond_To_Remote     									208

typedef struct __attribute__((packed))
{
  uint8_t byte_start;
  uint8_t byte_CMD;
  uint8_t byte_data;
  uint8_t byte_end;
}RxBuff_Irda_t;

enum LIGHTDMX_ADDRESS{
	ADDRESS=0,
	RCALIB,
	GCALIB,
	BCALIB,
	RVALUE,
	GVALUE,
	BVALUE
};

typedef struct {
	uint8_t id;
	uint8_t max[DMX_VALUE_SIZE];		//0->100%
	uint8_t value[DMX_VALUE_SIZE];
	uint8_t debug[3];
	uint8_t isdebug;
	uint16_t channel;
}LightDMX_Structure;

typedef void (*u8ConfigFn)();

typedef uint8_t (*u8I2C_ReadFn)(uint16_t,volatile uint8_t*,uint16_t );
typedef uint8_t (*u8I2C_WriteFn)(uint16_t,volatile uint8_t*,uint16_t );

typedef void (*vOutputDMX)(volatile uint8_t*);

uint8_t u8_LightDMX_Init(u8ConfigFn,u8I2C_ReadFn,u8I2C_WriteFn,vOutputDMX);
void u8_LightDMX_Run(void);
void Caculator_chanel(void);

extern QueueHandle_t  xDMX_Queue;
extern volatile LightDMX_Structure lightDMX_data;

#define OUTPUT_EVENT	'0'
#define SETVALUE_EVENT	'1'
#define SETID_EVENT	'2'
#define SETMAX_EVENT	'3'
#define GETDEBUG_EVENT	'4'

typedef struct{
	uint8_t cmd;
	uint8_t value[3];
}DMX_Command_Queue;

#define OUTPUT_COMMAND_QUEUE {OUTPUT_EVENT,0,0,0}
#define DEBUGRED_COMMAND_QUEUE {GETDEBUG_EVENT,1,2,20}

enum debugEnumValue{
	DEBUG_R70=0,
	DEBUG_R90,
	DEBUG_G70,
	DEBUG_G90,
	DEBUG_B70,
	DEBUG_B90,
	DEBUG_ALL,
	DEBUG_MAX
};

#endif