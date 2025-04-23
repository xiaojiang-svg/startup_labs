/**
 * ��һ���ǻ��ڼĴ���������FIFOֱд����ʱ������DMA���ж���ʱ�ƺ�Ҳ���ã��������������жϣ�Ȼ������ٿ���
 * ���䣺
 * 1.
 * �ڿ��������У����ƺ�����FIFOֱд��̫�ʺ�ʹ��TSER����ΪTSER����CTSIZE��ʱ��Żᴥ�����أ����Ҳ���FIFOֱдֻ��Ҫ����FIFO���Ƿ��п��еĿռ��ṩ����д�����������ݣ���������ʱ����Ҫ����TSER����ÿ�δ���Ĺ�����ֱ��д�뵱ǰ���ݳ��ȵ�TSIZE���ɡ����ǵ�������ճ�Ʒ��һ��Ƕ��ʽʵʱϵͳ����ô��������Ҫ�������ǵ��ļ�������ԴƵ�ʺ����Ǵ���Ч��֮���Э����
 * 2.
 * ֱ�����°�ŷ��֣�SPI���ȫ˫������Ҫ�����������ݲ�������SCK�Ĳ�������ֱ���졣���Գ��������Լ��������д���ݹ�ȥ������ʱ�䶼Ҫ����0xFF���������ܱ�֤SCK�Ĳ�����Ȼ��SD����������������
 */

#include "main.h"

/**
 * ������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ
 * SPI�ڳ�ʼ��
 * ���������SPI1�ĳ�ʼ��
 * PA5->SPI1_SCK PA6->SPI1_MISO PA7->SPI1->MOSI
 * ��������Ϊ����һ��һ�����Բ���ҪNSS�������ҪNSS�������PA4����
 */
// void SPI1_Init(void) {
//   uint32_t tempreg = 0;
//   RCC->AHB4ENR |= 1 << 0;  // ʹ��PORTAʱ��   	//20241119  Bit0 GPIOAEN
//   RCC->APB2ENR |= 1 << 12; // SPI1ʱ��ʹ��  		//20241119   RCC_APB2ENR
//                            // Bit12 SPI1EN  Bit13 SPI4EN

//   //����SPI��ʱ��Դ
//   RCC->D2CCIP1R &= ~(7 << 12); // SPI123SEL[2:0]=0,���ԭ��������
//   RCC->D2CCIP1R |=
//       0x01
//       << 12; // SPI123SEL[2:0]=1,ѡ��pll1_q_ck��ΪSPI1/2/3��ʱ��Դ,�ҽ�������Ϊ��32MHz

//   GPIO_InitTypeDef hspi;
//   hspi.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_4;
//   hspi.Mode = GPIO_MODE_AF_PP;
//   hspi.Pull = GPIO_PULLUP;
//   hspi.Speed = GPIO_SPEED_FREQ_HIGH;
//   hspi.Alternate = GPIO_AF5_SPI1;
//   // GPIO����
//   HAL_GPIO_Init(GPIOA, &hspi);

//   //����ֻ���SPI�ڳ�ʼ��
//   RCC->APB2RSTR |=
//       1 << 13;                 // ��λSPI1			//20241119  Bit13 SPI1RST
//   RCC->APB2RSTR &= ~(1 << 13); // ֹͣ��λSPI1  //20241119  Bit13 SPI1RST

//   SPI1->CFG1 = 6 << 28; // MBR[2:0]=0,128��Ƶ.
//   SPI1->CFG1 |= 7 << 0; // DSIZE[4:0]=7,����SPI֡��ʽΪ8λ,���ֽڴ���
//   SPI1->CFG1 |=
//       0x00
//       << 5; // FTHVL[3:0]=0,����SPI�������ݰ�������֡��Ϊ1��һ�����ݰ�����һ���ֽ�
//   tempreg = (uint32_t)1 << 31; // AFCNTR=1,SPI���ֶ�IO�ڵĿ���
//   tempreg |= 0 << 29;          // SSOE=0,��ֹӲ��NSS���
//   tempreg |= 1 << 26;          // SSM=1,�������NSS��
//   tempreg |= 1 << 25;          // CPOL=1,����״̬��,SCKΪ�ߵ�ƽ
//   tempreg |= 1 << 24;      // CPHA=1,���ݲ����ӵ�2��ʱ����ؿ�ʼ
//   tempreg |= 0 << 23;      // LSBFRST=0,MSB�ȴ���
//   tempreg |= 1 << 22;      // MASTER=1,����ģʽ
//   tempreg |= 0 << 19;      // SP[2:0]=0,Ħ��������ʽ
//   tempreg |= 0 << 17;      // COMM[1:0]=0,ȫ˫��ͨ��
//   SPI1->CFG2 = tempreg;    // ����CFG2�Ĵ���
//   SPI1->CR2 &= 0xFFFF0000; // ����TSIZE=0�����ô���ĳ���δ֪
//   SPI1->CR1 |=
//       0x01
//       << 12; // �ڲ�SS�ź������ƽ����ʱ����SS���ŵ�I/Oֵ����������ֻ��SD�����в�����Ҳû��Ҫ��������ã���ʱд������ɡ�
//   SPI1->CFG1 &= 0x00 << 14;   // Tx/Rx DMA��������ʹ��
//   SPI1->I2SCFGR &= ~(1 << 0); // ѡ��SPIģʽ
//   SPI1->CR1 |= 1 << 0;
//   // SPE=1,ʹ��SPI
//   /* �ж����� */
//   // HAL_NVIC_EnableIRQ(SPI1_IRQn);
//   // HAL_NVIC_SetPriority(SPI1_IRQn, 14, 14);

//   // SPI1->IER |= 0x03 << 0; // RXP TXP
//   // SPI1->IER |= 0x03 << 3; // EOT/SUSP/TXC TXTF
// }

// /**
//  * SPI1�ٶ����ú���
//  * SpeedSet:0~7
//  * SPI�ٶ�=spi_ker_ck/2^(SpeedSet+1)
//  * spi_ker_ck����ѡ������pll1_q_ck,Ϊ32Mhz
//  */
// void SPI1_SetSpeed(uint8_t SpeedSet) {
//   SpeedSet &= 0X07;         // ���Ʒ�Χ
//   SPI1->CR1 &= ~(1 << 0);   // SPE=0,SPI�豸ʧ��
//   SPI1->CFG1 &= ~(7 << 28); // MBR[2:0]=0,���ԭ���ķ�Ƶ����
//   SPI1->CFG1 |= (uint32_t)SpeedSet << 28; // MBR[2:0]=SpeedSet,����SPI1�ٶ�
//   SPI1->CR1 |= 1 << 0;                    // SPE=1,SPI�豸ʹ��
// }

// /**
//  * @brief
//  * ���ǹ���SPI1������SD�����жϴ������������漰������жϴ����źţ�������Ҫ�ֶ���������д���
//  */
// void SPI1_IRQHandler(void) {}

// /**
//  * @brief �˺������ڷ��͵����ֽ�����
//  * @param data: �����͵�����
//  */
// void SPI1_WriteByte(uint8_t data) {
//   while ((SPI1->SR & 1 << 1) == 0)
//     ; // �ȴ���������
//   *(volatile uint8_t *)&SPI1->TXDR =
//       data; //����һ��byte,�Դ��䳤�ȷ���TXDR�Ĵ���
//   while ((SPI1->SR & 1 << 0) == 0)
//     ; // �ȴ�������һ��byte
//   uint8_t dummy =
//       *(volatile uint8_t *)&SPI1->RXDR; //����һ��byte,�Դ��䳤�ȷ���RXDR�Ĵ���
// }

// /* SD������ */
// int8_t SD_Init(void) {
//   uint32_t response;

//   GPIOA->BSRR = 1 << 4; // SPI1 NSS����
//   /* 1.��������74������ʱ�� */
//   for (int i = 0; i < 10; i++) {
//     SPI1_WriteByte(0xFF);
//   }
//   GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS����

//   /* 2. ����CMD0��λ */
//   SPI1_SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);
//   if (SPI1_SD_ReceiveResponse() != 0x01) {
//     return CMD0_ERROR;
//   }
//   GPIOA->BSRR = 1 << 4;       // SPI1 NSS����
//   SPI1_WriteByte(0xFF);       // ���Ϳ��ֽ���ʱ
//   GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS����

//   /* 3. ����CMD8���Э��汾 */
//   SPI1_SD_SendCmd(SD_CMD_CHECK_VERSION, 0x000001AA, 0x87);
//   // V1��
//   if (SPI1_SD_ReceiveResponse() != 0x01) {
//     GPIOA->BSRR = 1 << 4;       // SPI1 NSS����
//     SPI1_WriteByte(0xFF);       // ���Ϳ��ֽ���ʱ
//     GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS����
//     SPI1_SD_SendCmd(SD_CMD_SEND_OP_COND, 0, 0x01);
//     while (SPI1_SD_ReceiveResponse()) {
//       ;
//     }
//   }
//   // �����V2��
//   else {
//     for (int i = 0; i < 4; i++) {
//       response = (response << 8 * i) | SPI1_SD_ReceiveResponse();
//     }
//     GPIOA->BSRR = 1 << 4;       // SPI1 NSS����
//     SPI1_WriteByte(0xFF);       // ���Ϳ��ֽ���ʱ
//     GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS����
//     if (response != 0x000001AA) {
//       return CMD8_ERROR;
//     }
//     do {
//       SPI1_SD_SendCmd(SD_CMD_APP_CMD, 0, 0x01);
//       response = SPI1_SD_ReceiveResponse();
//       SPI1_SD_SendCmd(SD_ACMD_SD_SEND_OP_COND, 0x40000000, 0x01);
//       response = SPI1_SD_ReceiveResponse();
//     } while (response != 0x00);

//     /* 4. ����CMD58��ȡOCR */
//     SPI1_SD_SendCmd(SD_CMD_READ_OCR, 0, 0x01);
//     GPIOA->BSRR = 1 << 4;       // SPI1 NSS����
//     SPI1_WriteByte(0xFF);       // ���Ϳ��ֽ���ʱ
//     GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS����
//     response = 0;
//     for (int i = 0; i < 4; i++) {
//       response = (response << 8 * i) | SPI1_SD_ReceiveResponse();
//     }
//     if (response & 0x40000000) {
//       // SDHC/SDXC��(��̶�Ϊ512�ֽ�)
//     } else {
//       // SDSC��
//       SPI1_SD_SendCmd(SD_CMD_SET_BLOCKLEN, SD_BLOCK_SIZE, 0x01);
//       if (SPI1_SD_ReceiveResponse() != 0x00) {
//         return CMD58_ERROR;
//       }
//     }
//   }

//   GPIOA->BSRR = 1 << 4; // SPI1 NSS����
//   SPI1_WriteByte(0xFF); // ���Ϳ��ֽ���ʱ
//   return 0;
// }

// /**
//  * @brief �˺������ڴ����������Ͷ��֡���ݵ����
//  * @note
//  * �˺�������ǰһ�����ݵĴ��������������ɺ��ٿ�����ǰ��һ�飬���Է��ڲ���ϵͳ�п��Խ�����𣬵ȴ�ǰһ�鴫����ɺ��ٿ��������������ƺ�ֻ�ܼ��EOT����Ȼ���ܻᵼ�������ʱ�����������ݴ��󣬵�����Ч����ռ��SD�����ڴ�ռ�
//  */
// void SPI1_WriteSD(uint8_t *TxData) {
//   /* �ж���һ���������Ƿ��Ѿ���ɴ��䣬�����EOT��־ */
//   // Version 1--with OS
//   if ((SPI1->SR & 1 << 3) == 0) {
//     // suspend��EOT�жϻָ�����̬
//   }

//   // Version 2--without OS
//   while ((SPI1->SR & 1 << 3) == 0) {
//     // ��ѯռ��CPU
//   }

//   /* ��ʼ���� */
//   SPI1->CR2 |= SD_BLOCK_SIZE + 3; // ���ô��䳤��
//   SPI1->CR1 |= 1 << 9;            // CSTART=1,��������
//   for (uint32_t i = 0; i < SD_BLOCK_SIZE; i++) {
//     while ((SPI1->SR & 1 << 1) == 0)
//       ; // �ȴ���������
//     *(volatile uint8_t *)&SPI1->TXDR =
//         TxData[i]; //����һ��byte,�Դ��䳤�ȷ���TXDR�Ĵ���
//     while ((SPI1->SR & 1 << 0) == 0)
//       ; // �ȴ�������һ��byte
//     uint8_t dummy = *(volatile uint8_t *)&SPI1
//                          ->RXDR; //����һ��byte,�Դ��䳤�ȷ���RXDR�Ĵ���
//   }
//   return;
// }

// /**
//  * @brief �˺������ڴ����������ն��֡���ݵ����
//  * @note ����ÿ���ǰһ�����ݵĽ���������������ǰһ�������ٽ�����һ�����ݣ�
//  */
// void SPI1_ReadSD(uint8_t *RxData) {
//   /* �ж���һ���������Ƿ��Ѿ���ɴ��䣬�����EOT��־ */
//   // Version 1--with OS
//   if ((SPI1->SR & 1 << 3) == 0) {
//     // suspend��EOT�жϻָ�����̬
//   }

//   // Version 2--without OS
//   while ((SPI1->SR & 1 << 3) == 0) {
//     // ��ѯռ��CPU
//   }

//   /* ��ʼ���� */
//   SPI1->CR2 |= SD_BLOCK_SIZE + 3; // ���ô��䳤��
//   SPI1->CR1 |= 1 << 9;            // CSTART=1,��������
//   for (uint32_t i = 0; i < SD_BLOCK_SIZE; i++) {
//     while ((SPI1->SR & 1 << 1) == 0)
//       ;
//     *(volatile uint8_t *)&SPI1->TXDR = 0xFF;
//     while ((SPI1->SR & 1 << 0) == 0)
//       ; // �ȴ�������һ��byte
//     RxData[i] = *(volatile uint8_t *)&SPI1
//                      ->RXDR; //����һ��byte,�Դ��䳤�ȷ���RXDR�Ĵ���
//   }
//   return;
// }

// /**
//  * @brief �˺���������ֹ���д����
//  */
// void SPI1_SD_StopWrite() {
//   while ((SPI1->SR & 1 << 1) == 0)
//     ;
//   *(volatile uint8_t *)&SPI1->TXDR = SD_STOP_DATA_MULTIPLE_BLOCK_WRITE;
//   while ((SPI1->SR & 1 << 0) == 0)
//     ;
//   uint8_t dummy = *(volatile uint8_t *)&SPI1->RXDR;
//   return;
// }

// /**
//  * @brief �˺������ڷ���SD������
//  * @param cmd: ����
//  * @param addr: ��ַ
//  * @param crc: CRCУ����
//  */
// void SPI1_SD_SendCmd(uint8_t cmd, uint32_t addr, uint8_t crc) {
//   GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS����

//   uint8_t frame[6];
//   frame[0] = 0x40 | cmd;
//   frame[1] = addr >> 24;
//   frame[2] = addr >> 16;
//   frame[3] = addr >> 8;
//   frame[4] = addr;
//   frame[5] = crc;
//   for (int i = 0; i < 6; i++) {
//     while ((SPI1->SR & 1 << 1) == 0)
//       ;
//     *(volatile uint8_t *)&SPI1->TXDR = frame[i];
//     while ((SPI1->SR & 1 << 0) == 0)
//       ;
//     uint8_t dummy = *(volatile uint8_t *)&SPI1->RXDR;
//   }
// }

// /**
//  * @brief �˺������ڽ���SD����Ӧ
//  */
// uint8_t SPI1_SD_ReceiveResponse() {
//   uint8_t response;
//   do {
//     while ((SPI1->SR & 1 << 1) == 0)
//       ;
//     *(volatile uint8_t *)&SPI1->TXDR = 0xFF;
//     while ((SPI1->SR & 1 << 0) == 0)
//       ;
//     response = *(volatile uint8_t *)&SPI1->RXDR;
//   } while (response == 0xFF);
//   return response;
// }