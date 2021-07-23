#include "stm32f1xx_hal.h"



typedef int32_t BME280_S32_t ;

void USART2_Init(void);

//int32_t  BME280_compensate_T_int32(uint32_t temp_value);
float  BME280_compensate_T_int32(uint32_t temp_value);
uint32_t BME280_compensate_P_int32(int32_t Press_value);
uint32_t BME280_compensate_H(uint32_t humidity_value);


int32_t Hum_adc_values(void);
int32_t Temp_adc_values(void);
int32_t Press_adc_values(void);
 void BME_280_Init(void);
 
 /*
#define  calib25_ADDR (uint8_t) 0xA1;
#define  calib26_ADDR (uint8_t) 0xE1;
#define  calib27_ADDR (uint8_t) 0xE2;

*/