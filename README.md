# Bosch-BME280
Humidity and temperature sensor with stm32f103 / Weight cell measurements
Keil Uvision IDE
I2C HAL library
The sensors sends the result to serial monitor with USART2

The Weight acquisition system measures the differential signal provided by the strain gauge sensor (based on full bridge).

The HX711 module is a 24-bit ADC designed for weigh
scales and industrial control applications , it can be interfaced directly with a bridge sensor using the differential inputs.

The system is able to send the data from the sensor to a remote server using a wi-fi connectivity

Power supply :
Battery or external +5V DC powered through USB port.

The charge management controller monitors the charging process for the battery
The MCP738312 enhance the safety and efficiency of the battery, 
 and provides selectable voltage regulation of 4.2v , and constant charge current of 500mA .

The battery protection circuit protects lithium ion/polymer battery from damage or degrading the lifetime due to overcharge,
overdischarge, and/or overcurrent.
DW01-P is an IC used for a 4.2 battery voltage , it protects the battery according to voltages thresholds : Vocp( over charge protection) , Vodp ( over discharge protection).

The system is running on Stm32F1 series , ARM Cortex-M3 processor.

The system is also able to measure the local humidity and temperature , using the BME280 from BOSCH .

The firmware is tested on the developpment board.

![Results_BME280](https://user-images.githubusercontent.com/58118111/126770247-2c6644a7-4345-4d8a-94c5-44c2c682afb1.png)
![HX711_BME280_3D](https://user-images.githubusercontent.com/58118111/128508178-af894f91-28c6-4ad7-a88d-68c139002927.png)
