#ifndef __USART_H
#define __USART_H 

#ifdef __cplusplus //而这一部分就是告诉编译器，如果定义了__cplusplus(即如果是cpp文件， 
extern "C"{ //因为cpp文件默认定义了该宏),则采用C语言方式进行编译
#endif
	
#include "stdint.h"

#define USART_REC_LEN 		200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1			//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			1			//使能（1）/禁止（0）串口1接收
#define EN_USART3_RX 			1			//使能（1）/禁止（0）串口1接收

//#pragma  pack(1)
typedef struct {
	float 	Pressure;
	float 	Temperature;	
	float 	Altitude;	
	float 	Velocity;
	uint8_t Flag;	
} __attribute__((packed)) BaroDataPacked;
//#pragma  pack(pop)		//下面的C代码编译并未出现问题，待确认其可实现性
typedef BaroDataPacked __attribute__((aligned(4))) BaroData;


//typedef struct {
//		//	Total 	133 Bytes
//		//	Flag		1 Byte												
//		uint8_t		DataFlag;								//	1	Byte
//		// Imu Data 24 Bytes
//		uint32_t	imainLoop_Imu;					//	4	Bytes
//		uint64_t	timCunt_Imu;						//	8	Bytes
//		int16_t 	ax;											//	2	Bytes
//		int16_t 	ay;											//	2	Bytes
//		int16_t 	az;											//	2	Bytes
//		int16_t 	gx;											//	2	Bytes
//		int16_t 	gy;											//	2	Bytes
//		int16_t 	gz;											//	2	Bytes
//		// Mag Data 18 Bytes
//		uint32_t	imainLoop_Mag;					//	4	Bytes
//		uint64_t	timCunt_Mag;						//	8	Bytes
//		int16_t 	mx;											//	2	Bytes
//		int16_t 	my;											//	2	Bytes
//		int16_t 	mz;											//	2	Bytes
//		// Baro Data 21	Bytes
//		uint32_t	imainLoop_Baro;					//	4	Bytes
//		uint64_t	timCunt_Baro;						//	8	Bytes
//		float  		Baro_Altitude;					//	4	Bytes	
//		float  		Temperature;						//	4	Bytes
//		uint8_t 	Flag;										//	1	Bytes	
//		// GPS Data 69 Bytes
//		uint32_t	imainLoop_Gps;					//	4	Bytes
//		uint64_t	timCunt_Gps;						//	8	Bytes
//    int32_t 	Latitude;								//	4	Bytes
//    int32_t 	Longitude;							//	4	Bytes
//    float 		GPS_Altitude;								//	4	Bytes
//		float 		GeoidSeparation;				//	4	Bytes
//    float			hAcc;										//	4	Bytes
//	  float			vAcc;										//	4	Bytes
//		float			sAcc;										//	4	Bytes 
//		float			vel_m_s;								//	4	Bytes	
//		float			vel_NED[3];							//	12Bytes
//		float			gdop;										//	4	Bytes
//		uint16_t 	PDOP;										//	2	Bytes
//		uint16_t 	HDOP;										//	2	Bytes
//    uint16_t 	VDOP;										//	2	Bytes
//    uint8_t 	Status;									//	1	Bytes
//    uint8_t		Satellites;							//	1	Bytes
//		uint8_t		vel_NED_valid;					//	1	Bytes
//} __attribute__((packed)) AllSensorsData;

//extern AllSensorsData	AllDataToH743;

extern BaroDataPacked BaropackData;

	  	
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART_RX_STA;         				//接收状态标记	


extern void uart1_init(uint32_t pclk2,uint32_t bound); 
extern void uart2_init(uint32_t pclk2,uint32_t bound);
extern void uart3_init(uint32_t pclk2,uint32_t bound);
extern void Uart1_SendByte(const uint8_t* pData, const uint16_t Datalen);
extern void Uart2_SendByte(const uint8_t* pData, const uint16_t Datalen);
extern void Uart3_SendByte(const uint8_t* pData, const uint16_t Datalen);
extern void* Clib_memcpy( void* s1, const void* s2, int n );

#ifdef __cplusplus
}
#endif	

#endif	   
















