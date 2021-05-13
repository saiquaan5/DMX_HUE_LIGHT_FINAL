#include "LightDMX.h"

u8I2C_ReadFn I2C_ReadFn=NULL;
u8I2C_WriteFn I2C_WriteFn=NULL;
u8ConfigFn LightDMXConfigFn=NULL;
vOutputDMX OutputDMXFn=NULL;

volatile LightDMX_Structure lightDMX_data;

void Caculator_chanel(void){
	lightDMX_data.channel=((uint16_t)(lightDMX_data.id-1)*3);
	// printf2("Chanel: %d\n", lightDMX_data.channel);
}

void v_LightReadID(){
	I2C_ReadFn(ADDRESS,&lightDMX_data.id,1);
	Caculator_chanel();
}

void v_LightSetID(uint8_t id){
	I2C_WriteFn(ADDRESS,&id,1);
	v_LightReadID();
}


uint8_t u8_LightDMX_Init(u8ConfigFn sidFn,u8I2C_ReadFn i2cReadFn,u8I2C_WriteFn i2cWriteFn,vOutputDMX Out){
	uint8_t id=1;uint8_t i;
	I2C_ReadFn=i2cReadFn;
	I2C_WriteFn=i2cWriteFn;
	LightDMXConfigFn=sidFn;
	OutputDMXFn=Out;
	if(I2C_ReadFn!=NULL)	{
		v_LightReadID();
	}
	if(LightDMXConfigFn!=NULL){
		LightDMXConfigFn();
	}
	if(I2C_WriteFn!=NULL){
		
		if(lightDMX_data.id>170){
			id=1;
			Caculator_chanel();
			I2C_WriteFn(ADDRESS,&id,1);
			lightDMX_data.id=1;
		}
		id=100;
		for(i=0;i<3;i++){
				if(lightDMX_data.max[i]>100){
					lightDMX_data.max[i]=100;
					id=100;
					I2C_WriteFn(RCALIB+i,&id,1);
				}
				if(lightDMX_data.max[i]<50){
					lightDMX_data.max[i]=50;
					id=50;
					I2C_WriteFn(RCALIB+i,&id,1);
				}
		}
	}
	
	IRDA_Init();
	
	return 0;
}

void vLightDMXGetMaxValue(){
	int i;
	float max;
	for(i=0;i<DMX_VALUE_SIZE;i++){
		max=lightDMX_data.max[i]/(float)100;
		max*=MAX_PULSE;
		lightDMX_data.value[i]=(lightDMX_data.value[i]<(int)max)?lightDMX_data.value[i]:(int)max;
	}
}

void vLightDMXDebug(uint8_t id,uint8_t during,uint8_t timecount){
	uint8_t debugDefault[DEBUG_MAX][3]={{220,0,0},{180,0,0},{0,220,0},{0,180,0},{0,0,220},{0,0,180},{220,220,220}};
	uint8_t debugNone[3]={0,0,0};
	//int i;
	//for(i=0;i<timecount;i++){
		//if(i%2){
			OutputDMXFn(debugDefault[id]);
		//}else{
			//OutputDMXFn(debugNone);
		//}
		vTaskDelay(during*1000);
}

void vSetMaxValue(uint8_t R,uint8_t G,uint8_t B){
	uint8_t value[3]={0};
	value[0]=(R>100)?100:R;
	value[1]=(G>100)?100:G;
	value[2]=(B>100)?100:B;
	I2C_WriteFn(RCALIB,value,3);
}

void vFeedbackIR(){
	printf2("{id:%d,max:[%d,%d,%d],test:[%d,%d,%d]}",lightDMX_data.id,lightDMX_data.max[0],lightDMX_data.max[1],lightDMX_data.max[2],lightDMX_data.debug[0],lightDMX_data.debug[1],lightDMX_data.debug[2]);
}

void vResponseIR(uint8_t cmd,uint8_t value){
	IRWrite(0xFE);
	IRWrite(cmd);
	IRWrite(value);
	IRWrite(0xEF);
}

//LightDMX Run
void u8_LightDMX_Run(void){
	const TickType_t xTicksToWait = 500 / portTICK_PERIOD_MS;
	DMX_Command_Queue ReceiveData;
	if(xQueueReceive(xDMX_Queue,&ReceiveData,xTicksToWait)== pdTRUE){
		switch(ReceiveData.cmd){
			case OUTPUT_EVENT:
				//vLightDMXGetMaxValue();
				OutputDMXFn(lightDMX_data.value);
			break;
			
			case SETMAX_EVENT:
						vSetMaxValue(ReceiveData.value[0],ReceiveData.value[1],ReceiveData.value[2]);
						I2C_ReadFn(RCALIB,(uint8_t*)lightDMX_data.max,3);
						vFeedbackIR();
						
			break;
			
			case GETDEBUG_EVENT:
						lightDMX_data.isdebug=1;
						vFeedbackIR();
						vLightDMXDebug(ReceiveData.value[0],ReceiveData.value[1],ReceiveData.value[2]);
						lightDMX_data.isdebug=0;
			break;
			
//			case SETID_EVENT:
//						v_LightSetID(ReceiveData.value[0]);
//						v_LightReadID();
//						vFeedbackIR();
//			break;
			
			case CMD_Write_Add_To_Light:
					printf2("reach here\n");

					printf2("Address = %d\n", ReceiveData.value[0]);
					
					v_LightSetID(ReceiveData.value[0]);
					vTaskDelay(100);
					v_LightReadID();
//					if(lightDMX_data.id==ReceiveData.value[0])
//						vResponseIR(CMD_Write_Add_To_Light,1);
//					else
//						vResponseIR(CMD_Write_Add_To_Light,0);
			break;
					
			case CMD_Read_Add_From_Light:{
					v_LightReadID();
					vResponseIR(CMD_Read_Add_From_Light,lightDMX_data.id);
			}break;

			case CMD_Test_Red:{
					lightDMX_data.isdebug=1;
					vResponseIR(CMD_Test_Red,1);
					vLightDMXDebug(DEBUG_R70,4,5);
					lightDMX_data.isdebug=0;
			}break;
			
			case CMD_Test_Green:{
					lightDMX_data.isdebug=1;
					vResponseIR(CMD_Test_Green,1);
					vLightDMXDebug(DEBUG_G70,4,5);
					lightDMX_data.isdebug=0;
			}break;
			
			case CMD_Test_Blue:{
					lightDMX_data.isdebug=1;
					vResponseIR(CMD_Test_Blue,1);
					vLightDMXDebug(DEBUG_B70,4,5);
					lightDMX_data.isdebug=0;
			}break;
			
			case CMD_Test_All:{
					lightDMX_data.isdebug=1;
					vResponseIR(CMD_Test_All,1);
					vLightDMXDebug(DEBUG_ALL,2,5);
					lightDMX_data.isdebug=0;
			}break;
			
			case CMD_Trans_Limit_Power:{
				vSetMaxValue(ReceiveData.value[0],ReceiveData.value[0],ReceiveData.value[0]);
				I2C_ReadFn(RCALIB,(uint8_t*)lightDMX_data.max,3);
				if(lightDMX_data.max[0]==ReceiveData.value[0])
						vResponseIR(CMD_Trans_Limit_Power,1);
					else
						vResponseIR(CMD_Trans_Limit_Power,0);
			}break;
			
			default:
				
			break;
		}
	}else{
		
	}
		
}