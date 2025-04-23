#ifdef __cplusplus //而这一部分就是告诉编译器，如果定义了__cplusplus(即如果是cpp文件，
extern "C" { //因为cpp文件默认定义了该宏),则采用C语言方式进行编译
#endif

#include "main.h"

#ifdef __cplusplus
}
#endif

void *Clib_memcpy(void *s1, const void *s2, int n) {
  char *su1 = (char *)s1;
  const char *su2 = (const char *)s2;

  for (; 0 < n; ++su1, ++su2, --n) {
    *su1 = *su2;
  }
  return s1;
}

float AxImuData;
float AyImuData;
float AzImuData;
float GxImuData;
float GyImuData;
float GzImuData;

//////////////////////////////////////////////////////////////////

#if EN_USART1_RX //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误

uint8_t UartImuData[IMULEN];
uint8_t UartGpsData[GPSLEN];
uint8_t UartBaroData[BAROLEN];
uint8_t UartBaroDataClear[BAROLEN] = {0};
uint8_t SensorsData[SENSOR_LEN] = {0};

uint8_t RawData_Received = 0;
uint8_t BaroData_Received = 0;

uint32_t ImuData_Index = 0;
uint32_t BaroData_Index = 0;
uint32_t GpsData_Index = 0;
uint32_t SensorsData_Index = 0;

void USART1_IRQHandler(void) {
  uint8_t res;
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
  OSIntEnter();
#endif

  if (USART1->ISR & (1 << 5)) //接收到数据
  {
    res = USART1->RDR;

    SensorsData[SensorsData_Index++] = res;

    if (SensorsData_Index == 1 && SensorsData[0] != 0xFE) {
      SensorsData_Index = 0;
    }
    if (SensorsData_Index == 2 && SensorsData[1] != 0xEF) {
      SensorsData_Index = 0;
      //下面四行，是为了避免收尾两个FE FE相接时无法正确解析添加的
      SensorsData[SensorsData_Index++] = res;
      if (SensorsData_Index == 1 && SensorsData[0] != 0xFE) {
        SensorsData_Index = 0;
      }
    }
    if (SensorsData_Index > 5) {
      if (SensorsData[SensorsData_Index - 1] == 0xFE &&
          SensorsData[SensorsData_Index - 2] == 0xEF) {

        RawData_Received = 1;
        SensorsData_Index = 0; /*???????*/
      }
    }
  }

  USART1->ICR |= 1 << 3; //清除溢出错误,否则可能会卡死在串口中断服务函数里面
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
  OSIntExit();
#endif
}

void USART2_IRQHandler(void) //用于接收GPS的UBX数据
{
  uint8_t res;
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
  OSIntEnter();
#endif
  if (USART2->ISR & (1 << 5)) //接收到数据
  {
    res = USART2->RDR;

    //    UartGpsData[GpsData_Index++] = res;		//测试用，待删除
    //    不删除容易在几个周期后越界，造成硬件错误。
    AnalysisGPSdata(res);
    //		if(GpsData_Index > 180)		//测试用，待删除
    //		{
    //			GpsData_Index =0;
    //		}
  }
  USART2->ICR |= 1 << 3; //清除溢出错误,否则可能会卡死在串口中断服务函数里面
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
  OSIntExit();
#endif
}

void USART3_IRQHandler(void) {
  uint8_t res;
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
  OSIntEnter();
#endif
  if (USART3->ISR & (1 << 5)) //接收到数据
  {
    res = USART3->RDR;
    //		res = USART3->RDR;

    UartBaroData[BaroData_Index++] = res;

    // if(BaroData_Index > 10)
    //{
    //	BaroData_Index = BaroData_Index;
    // }

    if (BaroData_Index == 1 && UartBaroData[0] != 0xFE) {
      BaroData_Index = 0;
    }
    if (BaroData_Index == 2 && UartBaroData[1] != 0xEF) {
      BaroData_Index = 0;
      //下面四行，是为了避免收尾两个FE FE相接时无法正确解析添加的
      UartBaroData[BaroData_Index++] = res;
      if (BaroData_Index == 1 && UartBaroData[0] != 0xFE) {
        BaroData_Index = 0;
      }
    }
    if (BaroData_Index > 12) {
      if (UartBaroData[BaroData_Index - 1] == 0xFE &&
          UartBaroData[BaroData_Index - 2] == 0xEF) {
        Clib_memcpy(&BaropackData, &UartBaroData[2], 12);
        BaroData_Received = 1;
        BaroData_Index = 0; /*???????*/
      }
    }
  }
  USART3->ICR |= 1 << 3; //清除溢出错误,否则可能会卡死在串口中断服务函数里面
#if SYSTEM_SUPPORT_OS //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
  OSIntExit();
#endif
}

#endif

//初始化IO 串口1
// pclk2:PCLK2时钟频率(Mhz)
//注意:USART1的时钟,是可以通过RCC_D2CCIP2R选择的
//但是我们一般用默认的选择即可(默认选择rcc_pclk2作为串口1时钟)
// pclk2即APB2的时钟频率,一般为100Mhz
// bound:波特率
void uart1_init(uint32_t pclk2, uint32_t bound) {
  uint32_t temp;
  temp = (pclk2 * 1000000 + bound / 2) /
         bound;           //得到USARTDIV@OVER8=0,采用四舍五入计算
  RCC->AHB4ENR |= 1 << 0; //使能PORTA口时钟
  RCC->APB2ENR |= 1 << 4; //使能串口1时钟

  // GPIO配置
  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_9 | GPIO_PIN_10; // TX/RX
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_MEDIUM;
  hspi.Alternate = GPIO_AF7_USART1;
  // GPIO配置
  HAL_GPIO_Init(GPIOA, &hspi);

  //波特率设置
  USART1->BRR = temp;     //波特率设置@OVER8=0
  USART1->CR1 = 0;        //清零CR1寄存器
  USART1->CR1 |= 0 << 28; //设置M1=0
  USART1->CR1 |= 0 << 12; //设置M0=0&M1=0,选择8位字长
  USART1->CR1 |= 0 << 15; //设置OVER8=0,16倍过采样
  USART1->CR1 |= 1 << 3;  //串口发送使能
#if EN_USART1_RX          //如果使能了接收
  //使能接收中断
  USART1->CR1 |= 1 << 2; //串口接收使能
  USART1->CR1 |= 1 << 5; //接收缓冲区非空中断使能

#endif
  USART1->CR1 |= 1 << 0; //串口使能
}

void uart2_init(uint32_t pclk2, uint32_t bound) {
  uint32_t temp;
  temp = (pclk2 * 1000000 + bound / 2) /
         bound;             //得到USARTDIV@OVER8=0,采用四舍五入计算
  RCC->AHB4ENR |= 1 << 0;   //使能PORTA口时钟
  RCC->APB1LENR |= 1 << 17; //使能串口1时钟

  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_MEDIUM;
  hspi.Alternate = GPIO_AF7_USART2;
  // GPIO配置
  HAL_GPIO_Init(GPIOA, &hspi);

  //波特率设置
  USART2->BRR = temp;     //波特率设置@OVER8=0
  USART2->CR1 = 0;        //清零CR1寄存器
  USART2->CR1 |= 0 << 28; //设置M1=0
  USART2->CR1 |= 0 << 12; //设置M0=0&M1=0,选择8位字长
  USART2->CR1 |= 0 << 15; //设置OVER8=0,16倍过采样
  USART2->CR1 |= 1 << 3;  //串口发送使能
#if EN_USART2_RX          //如果使能了接收
  //使能接收中断
  USART2->CR1 |= 1 << 2;              //串口接收使能
  USART2->CR1 |= 1 << 5;              //接收缓冲区非空中断使能
  MY_NVIC_Init(3, 4, USART2_IRQn, 2); //组2，最低优先级
#endif
  USART2->CR1 |= 1 << 0; //串口使能
}

void uart3_init(uint32_t pclk2, uint32_t bound) {
  uint32_t temp;
  temp = (pclk2 * 1000000 + bound / 2) /
         bound;             //得到USARTDIV@OVER8=0,采用四舍五入计算
  RCC->AHB4ENR |= 1 << 1;   //使能PORTB口时钟
  RCC->APB1LENR |= 1 << 18; //使能串口1时钟

  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_10 | GPIO_PIN_11;
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_MEDIUM;
  hspi.Alternate = GPIO_AF7_USART3;
  // GPIO配置
  HAL_GPIO_Init(GPIOB, &hspi);

  //波特率设置
  USART3->BRR = temp;     //波特率设置@OVER8=0
  USART3->CR1 = 0;        //清零CR1寄存器
  USART3->CR1 |= 0 << 28; //设置M1=0
  USART3->CR1 |= 0 << 12; //设置M0=0&M1=0,选择8位字长
  USART3->CR1 |= 0 << 15; //设置OVER8=0,16倍过采样
  USART3->CR1 |= 1 << 3;  //串口发送使能
#if EN_USART3_RX          //如果使能了接收
  //使能接收中断
  USART3->CR1 |= 1 << 2;              //串口接收使能
  USART3->CR1 |= 1 << 5;              //接收缓冲区非空中断使能
  MY_NVIC_Init(3, 5, USART3_IRQn, 2); //组2，最低优先级
#endif
  USART3->CR1 |= 1 << 0; //串口使能
}

//串口1发送数据 函数
void Uart1_SendByte(const uint8_t *pData, const uint16_t Datalen) {
  uint8_t t;
  uint8_t temp;
  for (t = 0; t < Datalen; t++) {
    temp = pData[t];
    // USARTx->DR = (Data & (uint16_t)0x01FF);
    USART1->TDR = temp;
    while ((USART1->ISR & 0X40) == 0)
      ; //等待发送结束
  }
}
//串口2发送数据 函数
void Uart2_SendByte(const uint8_t *pData, const uint16_t Datalen) {
  uint8_t t;
  uint8_t temp;
  for (t = 0; t < Datalen; t++) {
    temp = pData[t];
    // USARTx->DR = (Data & (uint16_t)0x01FF);
    USART2->TDR = temp;
    while ((USART2->ISR & 0X40) == 0)
      ; //等待发送结束
  }
}

//串口3发送数据 函数
void Uart3_SendByte(const uint8_t *pData, const uint16_t Datalen) {
  uint8_t t;
  uint8_t temp;
  for (t = 0; t < Datalen; t++) {
    temp = pData[t];
    // USARTx->DR = (Data & (uint16_t)0x01FF);
    USART3->TDR = temp;
    while ((USART3->ISR & 0X40) == 0)
      ; //等待发送结束
  }
}
