#ifdef __cplusplus //����һ���־��Ǹ��߱����������������__cplusplus(�������cpp�ļ���
extern "C" { //��Ϊcpp�ļ�Ĭ�϶����˸ú�),�����C���Է�ʽ���б���
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

#if EN_USART1_RX //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���

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
#if SYSTEM_SUPPORT_OS //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
  OSIntEnter();
#endif

  if (USART1->ISR & (1 << 5)) //���յ�����
  {
    res = USART1->RDR;

    SensorsData[SensorsData_Index++] = res;

    if (SensorsData_Index == 1 && SensorsData[0] != 0xFE) {
      SensorsData_Index = 0;
    }
    if (SensorsData_Index == 2 && SensorsData[1] != 0xEF) {
      SensorsData_Index = 0;
      //�������У���Ϊ�˱�����β����FE FE���ʱ�޷���ȷ������ӵ�
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

  USART1->ICR |= 1 << 3; //����������,������ܻῨ���ڴ����жϷ���������
#if SYSTEM_SUPPORT_OS //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
  OSIntExit();
#endif
}

void USART2_IRQHandler(void) //���ڽ���GPS��UBX����
{
  uint8_t res;
#if SYSTEM_SUPPORT_OS //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
  OSIntEnter();
#endif
  if (USART2->ISR & (1 << 5)) //���յ�����
  {
    res = USART2->RDR;

    //    UartGpsData[GpsData_Index++] = res;		//�����ã���ɾ��
    //    ��ɾ�������ڼ������ں�Խ�磬���Ӳ������
    AnalysisGPSdata(res);
    //		if(GpsData_Index > 180)		//�����ã���ɾ��
    //		{
    //			GpsData_Index =0;
    //		}
  }
  USART2->ICR |= 1 << 3; //����������,������ܻῨ���ڴ����жϷ���������
#if SYSTEM_SUPPORT_OS //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
  OSIntExit();
#endif
}

void USART3_IRQHandler(void) {
  uint8_t res;
#if SYSTEM_SUPPORT_OS //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
  OSIntEnter();
#endif
  if (USART3->ISR & (1 << 5)) //���յ�����
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
      //�������У���Ϊ�˱�����β����FE FE���ʱ�޷���ȷ������ӵ�
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
  USART3->ICR |= 1 << 3; //����������,������ܻῨ���ڴ����жϷ���������
#if SYSTEM_SUPPORT_OS //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
  OSIntExit();
#endif
}

#endif

//��ʼ��IO ����1
// pclk2:PCLK2ʱ��Ƶ��(Mhz)
//ע��:USART1��ʱ��,�ǿ���ͨ��RCC_D2CCIP2Rѡ���
//��������һ����Ĭ�ϵ�ѡ�񼴿�(Ĭ��ѡ��rcc_pclk2��Ϊ����1ʱ��)
// pclk2��APB2��ʱ��Ƶ��,һ��Ϊ100Mhz
// bound:������
void uart1_init(uint32_t pclk2, uint32_t bound) {
  uint32_t temp;
  temp = (pclk2 * 1000000 + bound / 2) /
         bound;           //�õ�USARTDIV@OVER8=0,���������������
  RCC->AHB4ENR |= 1 << 0; //ʹ��PORTA��ʱ��
  RCC->APB2ENR |= 1 << 4; //ʹ�ܴ���1ʱ��

  // GPIO����
  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_9 | GPIO_PIN_10; // TX/RX
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_MEDIUM;
  hspi.Alternate = GPIO_AF7_USART1;
  // GPIO����
  HAL_GPIO_Init(GPIOA, &hspi);

  //����������
  USART1->BRR = temp;     //����������@OVER8=0
  USART1->CR1 = 0;        //����CR1�Ĵ���
  USART1->CR1 |= 0 << 28; //����M1=0
  USART1->CR1 |= 0 << 12; //����M0=0&M1=0,ѡ��8λ�ֳ�
  USART1->CR1 |= 0 << 15; //����OVER8=0,16��������
  USART1->CR1 |= 1 << 3;  //���ڷ���ʹ��
#if EN_USART1_RX          //���ʹ���˽���
  //ʹ�ܽ����ж�
  USART1->CR1 |= 1 << 2; //���ڽ���ʹ��
  USART1->CR1 |= 1 << 5; //���ջ������ǿ��ж�ʹ��

#endif
  USART1->CR1 |= 1 << 0; //����ʹ��
}

void uart2_init(uint32_t pclk2, uint32_t bound) {
  uint32_t temp;
  temp = (pclk2 * 1000000 + bound / 2) /
         bound;             //�õ�USARTDIV@OVER8=0,���������������
  RCC->AHB4ENR |= 1 << 0;   //ʹ��PORTA��ʱ��
  RCC->APB1LENR |= 1 << 17; //ʹ�ܴ���1ʱ��

  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_MEDIUM;
  hspi.Alternate = GPIO_AF7_USART2;
  // GPIO����
  HAL_GPIO_Init(GPIOA, &hspi);

  //����������
  USART2->BRR = temp;     //����������@OVER8=0
  USART2->CR1 = 0;        //����CR1�Ĵ���
  USART2->CR1 |= 0 << 28; //����M1=0
  USART2->CR1 |= 0 << 12; //����M0=0&M1=0,ѡ��8λ�ֳ�
  USART2->CR1 |= 0 << 15; //����OVER8=0,16��������
  USART2->CR1 |= 1 << 3;  //���ڷ���ʹ��
#if EN_USART2_RX          //���ʹ���˽���
  //ʹ�ܽ����ж�
  USART2->CR1 |= 1 << 2;              //���ڽ���ʹ��
  USART2->CR1 |= 1 << 5;              //���ջ������ǿ��ж�ʹ��
  MY_NVIC_Init(3, 4, USART2_IRQn, 2); //��2��������ȼ�
#endif
  USART2->CR1 |= 1 << 0; //����ʹ��
}

void uart3_init(uint32_t pclk2, uint32_t bound) {
  uint32_t temp;
  temp = (pclk2 * 1000000 + bound / 2) /
         bound;             //�õ�USARTDIV@OVER8=0,���������������
  RCC->AHB4ENR |= 1 << 1;   //ʹ��PORTB��ʱ��
  RCC->APB1LENR |= 1 << 18; //ʹ�ܴ���1ʱ��

  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_10 | GPIO_PIN_11;
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_MEDIUM;
  hspi.Alternate = GPIO_AF7_USART3;
  // GPIO����
  HAL_GPIO_Init(GPIOB, &hspi);

  //����������
  USART3->BRR = temp;     //����������@OVER8=0
  USART3->CR1 = 0;        //����CR1�Ĵ���
  USART3->CR1 |= 0 << 28; //����M1=0
  USART3->CR1 |= 0 << 12; //����M0=0&M1=0,ѡ��8λ�ֳ�
  USART3->CR1 |= 0 << 15; //����OVER8=0,16��������
  USART3->CR1 |= 1 << 3;  //���ڷ���ʹ��
#if EN_USART3_RX          //���ʹ���˽���
  //ʹ�ܽ����ж�
  USART3->CR1 |= 1 << 2;              //���ڽ���ʹ��
  USART3->CR1 |= 1 << 5;              //���ջ������ǿ��ж�ʹ��
  MY_NVIC_Init(3, 5, USART3_IRQn, 2); //��2��������ȼ�
#endif
  USART3->CR1 |= 1 << 0; //����ʹ��
}

//����1�������� ����
void Uart1_SendByte(const uint8_t *pData, const uint16_t Datalen) {
  uint8_t t;
  uint8_t temp;
  for (t = 0; t < Datalen; t++) {
    temp = pData[t];
    // USARTx->DR = (Data & (uint16_t)0x01FF);
    USART1->TDR = temp;
    while ((USART1->ISR & 0X40) == 0)
      ; //�ȴ����ͽ���
  }
}
//����2�������� ����
void Uart2_SendByte(const uint8_t *pData, const uint16_t Datalen) {
  uint8_t t;
  uint8_t temp;
  for (t = 0; t < Datalen; t++) {
    temp = pData[t];
    // USARTx->DR = (Data & (uint16_t)0x01FF);
    USART2->TDR = temp;
    while ((USART2->ISR & 0X40) == 0)
      ; //�ȴ����ͽ���
  }
}

//����3�������� ����
void Uart3_SendByte(const uint8_t *pData, const uint16_t Datalen) {
  uint8_t t;
  uint8_t temp;
  for (t = 0; t < Datalen; t++) {
    temp = pData[t];
    // USARTx->DR = (Data & (uint16_t)0x01FF);
    USART3->TDR = temp;
    while ((USART3->ISR & 0X40) == 0)
      ; //�ȴ����ͽ���
  }
}
