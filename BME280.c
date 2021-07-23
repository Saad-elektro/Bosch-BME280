

#include "BME280.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "printf.h"


extern I2C_HandleTypeDef hi2c1;  

#define SLAVE_ADDRESS_LCD 0x76 

void USART2_Init(void){
	
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRH &=~ GPIO_CRH_CNF9_0;
  GPIOA->CRH |= GPIO_CRH_CNF9_1;
  GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);
	
  USART1->BRR = 0XEA6;

  USART1->CR1 |= USART_CR1_UE;
  USART1->CR1 |= USART_CR1_TE;
  USART1->CR1 |= USART_CR1_RE ;

}

void USART_write( int ch){

	while(!(USART2->SR & USART_SR_TXE)){}  // we check if the transmit buffer is empty before sending the data
		
	USART2->DR = (ch);    // contains the received or transmitted data 
		              //we put the data which we will send in DR register of the microcontroller
	}

	
	
void BME_280_Init(){
	
uint8_t Reg_Value;
uint8_t Reg_ID_ADDR = 0xD0;
uint8_t Reg2_ADDR   = 0xF2;
uint8_t Reg4_ADDR   = 0xF4;
uint8_t Reg5_ADDR   = 0xF5;	


uint8_t Reg2_config = 0x01;
uint8_t Reg4_config = 0x27;

uint8_t Reg5_config = (0x11<<2|0>>7);

uint8_t Reg4_config_Value ;
uint8_t Reg2_config_Value ;
uint8_t Reg5_config_Value ;

// Test communication by reading Id register

HAL_I2C_Master_Transmit(&hi2c1, (0x76<<1), &Reg_ID_ADDR, 1,50);	
		
HAL_Delay(100);

HAL_I2C_Master_Receive(&hi2c1, (0x76<<1)|0x01 , &Reg_Value, 1, 50);

HAL_Delay(100);

printf_( "0x%02x \n" ,Reg_Value);	
		
HAL_Delay(2000);	

// Configure register ctrl_hum(0xF2) and Read configuration value

HAL_I2C_Mem_Write(&hi2c1, 0x76<<1, 0xF2, 1, &Reg2_config, 1, 50); 	
		
HAL_Delay(100);

HAL_I2C_Mem_Read(&hi2c1, (0x76<<1)|0x01 , 0xF2, 1, &Reg2_config_Value, 1, 50);		
			
HAL_Delay(100);

printf_( "0x%02x \n" ,Reg2_config_Value);	

HAL_Delay(2000);

// Configure register ctrl_meas(0xF4) and Read configuration value

HAL_I2C_Mem_Write(&hi2c1, 0x76<<1, 0xF4, 1, &Reg4_config, 1, 50); 	
		
HAL_Delay(100);

HAL_I2C_Mem_Read(&hi2c1, (0x76<<1)|0x01 , 0xF4, 1, &Reg4_config_Value, 1, 50);		
			
HAL_Delay(100);

printf_( "0x%02x \n" ,Reg4_config_Value);	

HAL_Delay(2000); 	




// Configure register config(0xF5) and Read configuration value

HAL_I2C_Mem_Write(&hi2c1, 0x76<<1, 0xF5, 1, &Reg5_config, 1, 50); 	
		
HAL_Delay(100);

HAL_I2C_Mem_Read(&hi2c1, (0x76<<1)|0x01 , 0xF5, 1, &Reg5_config_Value, 1, 50);		
			
HAL_Delay(100);

printf_( "0x%02x \n" ,Reg5_config_Value);	

HAL_Delay(2000);



}
//------- Global Variables------------


uint8_t reg_data[8];  // Pressure (20bits - Temperature(bits) - Humidity(16bits); 
uint8_t Press_regADDR = 0xF7;

int32_t t_fine;	// t_fine carries a fine resolution temperature value over pressure and humidity compensation formula
int8_t dig_H1;
	
	
int32_t Press_adc_values(){
		
	int32_t raw_press_values;

		
    HAL_I2C_Master_Transmit(&hi2c1, (0x76<<1), &Press_regADDR, 1,50);	// 0xF7 start adress Press (MSB) register
		
    HAL_Delay(100);

    HAL_I2C_Master_Receive(&hi2c1, (0x76<<1)|0x01 , reg_data, 8, 50);

    HAL_Delay(100);	
		
    raw_press_values = reg_data[0];
    raw_press_values = (raw_press_values<<8) | reg_data[1];
    raw_press_values = (raw_press_values<<4) | reg_data[2];
	
		
     return raw_press_values;
	}

int32_t Temp_adc_values(){
	

int32_t raw_temp_values;

HAL_Delay(100);	

 raw_temp_values = reg_data[3];
 raw_temp_values = (raw_temp_values<<8) | reg_data[4];
 raw_temp_values = (raw_temp_values<<4) | reg_data[5];
	
 
	return raw_temp_values;
	
	}

int32_t Hum_adc_values(){

int32_t raw_hum_values;
		
raw_hum_values = (int32_t)(reg_data[6]<<8 | reg_data[7]);

 
return raw_hum_values;


}	
	

 float BME280_compensate_T_int32(uint32_t temp_value){
	
	//int32_t t_fine; --> Declared as global variable
  int32_t var1, var2;
	float T;
	uint8_t calib_data[6];
	uint16_t dig_T1, dig_T2, dig_T3;
	 
	
  uint8_t  calib00_ADDR = 0x88;
  uint8_t  calib01_ADDR = 0x89;
  uint8_t  calib02_ADDR = 0x8A;
  uint8_t  calib03_ADDR = 0x8B;
  uint8_t  calib04_ADDR = 0x8C;
  uint8_t  calib05_ADDR = 0x8D;
		
// Read Temperature calibration data	
	
	HAL_I2C_Master_Transmit(&hi2c1, (0x76<<1), &calib00_ADDR, 1, 50);
	HAL_Delay(50);
	HAL_I2C_Master_Receive(&hi2c1, (0x76<<1)|0x01, calib_data, 6, 50); // not need use &calib_data, it is already array
	HAL_Delay(50);

	dig_T1 = ((uint16_t) calib_data[1] << 8 | (uint16_t) calib_data[0]);
	dig_T2 = (int16_t) ((uint16_t)calib_data[3] << 8 | (uint16_t) calib_data[2]);
	dig_T3 = (int16_t) ((uint16_t)calib_data[5] << 8 | (uint16_t) calib_data[4]);
	
	HAL_Delay(200);

    var1 = (int32_t)((temp_value / 8) - ((int32_t)dig_T1 * 2));
    var1 = (var1 * ((int32_t)dig_T2)) / 2048;
    var2 = (int32_t)((temp_value / 16) - ((int32_t)dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)dig_T3)) / 16384;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) / 256;

 return T/100;
 

 
}	


uint32_t BME280_compensate_P_int32(int32_t Press_value){
	
	
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    uint32_t var5;
    uint32_t P;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

uint8_t calib_p_data[19];
uint16_t dig_P1, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
	 
	
        uint8_t  calib06_ADDR = 0x8E;
        uint8_t  calib07_ADDR = 0x8F;
        uint8_t  calib08_ADDR = 0x90;
        uint8_t  calib09_ADDR = 0x91;
        uint8_t  calib10_ADDR = 0x92;
        uint8_t  calib11_ADDR = 0x93;
	uint8_t  calib12_ADDR = 0x94;
	uint8_t  calib13_ADDR = 0x95;
	uint8_t  calib14_ADDR = 0x96;
	uint8_t  calib15_ADDR = 0x97;
	uint8_t  calib16_ADDR = 0x98;
	uint8_t  calib17_ADDR = 0x99;
	uint8_t  calib18_ADDR = 0x9A;
	uint8_t  calib19_ADDR = 0x9B;
	uint8_t  calib20_ADDR = 0x9C;
	uint8_t  calib21_ADDR = 0x9D;
	uint8_t  calib22_ADDR = 0x9E;
	uint8_t  calib23_ADDR = 0x9F;
	uint8_t  calib25_ADDR = 0xA1;  // P27/55 datasheet, we should read dig_H1 with the Pressure register (from 0x88 to 0xA1)
	
	

// Read calibration data	
	
	HAL_I2C_Master_Transmit(&hi2c1, (0x76<<1), &calib06_ADDR, 1, 50);
	HAL_Delay(50);
	HAL_I2C_Master_Receive(&hi2c1, (0x76<<1)|0x01, calib_p_data, 19, 50); // not need use &calib_data, it is already array
	HAL_Delay(50);

	dig_P1 = ((uint16_t) calib_p_data[1] << 8 | (uint16_t) calib_p_data[0]);
	dig_P2 = (int16_t) ((uint16_t)calib_p_data[3] << 8 | (uint16_t) calib_p_data[2]);
	dig_P3 = (int16_t) ((uint16_t)calib_p_data[5] << 8 | (uint16_t) calib_p_data[4]);
	dig_P4 = (int16_t) ((uint16_t)calib_p_data[7] << 8 | (uint16_t) calib_p_data[6]);
	dig_P5 = (int16_t) ((uint16_t)calib_p_data[9] << 8 | (uint16_t) calib_p_data[8]);
	dig_P6 = (int16_t) ((uint16_t)calib_p_data[11] << 8 | (uint16_t) calib_p_data[10]);
	dig_P7 = (int16_t) ((uint16_t)calib_p_data[13] << 8 | (uint16_t) calib_p_data[12]);
	dig_P8 = (int16_t) ((uint16_t)calib_p_data[15] << 8 | (uint16_t) calib_p_data[14]);
	dig_P9 = (int16_t) ((uint16_t)calib_p_data[17] << 8 | (uint16_t) calib_p_data[16]);
	
	dig_H1 = calib_p_data[18];
	
	HAL_Delay(200);

	
	#if (BME280_USE_INT64)	
		int64_t v1,v2,p;

	v1 = (int64_t)t_fine - 128000;
	v2 = v1 * v1 * (int64_t)dig_P6;
	v2 = v2 + ((v1 * (int64_t)dig_P5) << 17);
	v2 = v2 + ((int64_t)dig_P4 << 35);
	v1 = ((v1 * v1 * (int64_t)dig_P3) >> 8) + ((v1 * (int64_t)dig_P2) << 12);
	v1 = (((((int64_t)1) << 47) + v1)) * ((int64_t)dig_P1) >> 33;
	if (v1 == 0) return 0; // avoid exception caused by division by zero
	p = 1048576 - Press_value;
	p = (((p << 31) - v2) * 3125) / v1;
	v1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	v2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + v1 + v2) >> 8) + ((int64_t)dig_P7 << 4);
#else 

	int32_t v1,v2;
	uint32_t p;

	v1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
	v2 = (((v1 >> 2) * (v1 >> 2)) >> 11 ) * ((int32_t)dig_P6);
	v2 = v2 + ((v1 * ((int32_t)dig_P5)) << 1);
	v2 = (v2 >> 2) + (((int32_t)dig_P4) << 16);
	v1 = (((dig_P3 * (((v1 >> 2) * (v1 >> 2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * v1) >> 1)) >> 18;
	v1 = (((32768 + v1)) * ((int32_t)dig_P1)) >> 15;
	if (v1 == 0) return 0; // avoid exception caused by division by zero
	p = (((uint32_t)(((int32_t)1048576) - Press_value) - (v2 >> 12))) * 3125;
	if (p < 0x80000000) {
		p = (p << 1) / ((uint32_t)v1);
	} else {
		p = (p / (uint32_t)v1) << 1;
	}
	v1 = (((int32_t)dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
	v2 = (((int32_t)(p >> 2)) * ((int32_t)dig_P8)) >> 13;
	p = (uint32_t)((int32_t)p + ((v1 + v2 + dig_P7) >> 4));

	
	p <<= 8;
#endif 

	return (uint32_t)p/256;
		
		
} 

// Calcul Humidity compensated 


uint32_t BME280_compensate_H(uint32_t humidity_value){
//  humidity_value = raw ADC humidity output value
uint32_t VAR_H;
	
uint8_t calib_H_data[7];
uint16_t  dig_H2,  dig_H4, dig_H5;
int8_t  dig_H3, dig_H6;
	
	int16_t dig_h4_lsb;
    int16_t dig_h4_msb;
    int16_t dig_h5_lsb;
    int16_t dig_h5_msb;
	

  uint8_t  calib26_ADDR = 0xE1;  // calib_H_Data[0]
  uint8_t  calib27_ADDR = 0xE2;  // calib_H_Data[1]
  uint8_t  calib28_ADDR = 0xE3;  // calib_H_Data[2]
  uint8_t  calib29_ADDR = 0xE4;  // calib_H_Data[3]
  uint8_t  calib30_ADDR = 0xE5;  // calib_H_Data[4]
  uint8_t  calib31_ADDR = 0xE6;  // calib_H_Data[5]
  uint8_t  calib32_ADDR = 0xE7;  // calib_H_Data[6]

// Read calibration data	

	
	HAL_I2C_Master_Transmit(&hi2c1, (0x76<<1), &calib26_ADDR, 1, 50);
	HAL_Delay(50);
	HAL_I2C_Master_Receive(&hi2c1, (0x76<<1)|0x01, calib_H_data, 7, 50); // not need use &calib_data, it is already array
	HAL_Delay(200);
	
	

uint8_t E5_LSB = calib_H_data[4]&0x0f;  // 0xE5 LSB  (MSB bit masking)
uint8_t E5_MSB = calib_H_data[4];       // 0xE5 MSB

         dig_H2 = (int16_t)(calib_H_data[1]<<8) |(int16_t) calib_H_data[0]; // 1 0
	 dig_H3 =  calib_H_data[2]; 
	 dig_H4 =  (calib_H_data[3] << 4) | (E5_LSB );	
	 dig_H5 =  (calib_H_data[5] << 4) | (E5_MSB >> 4);
	 dig_H6 =   (int8_t)calib_H_data[6];

	
	HAL_Delay(200);
	

    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = t_fine - ((int32_t)76800);
    var2 = (int32_t)(humidity_value * 16384);
    var3 = (int32_t)(((int32_t)dig_H4) * 1048576);
    var4 = ((int32_t)dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);

    return humidity;
}
