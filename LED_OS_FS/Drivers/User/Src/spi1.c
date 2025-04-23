/**
 * 这一版是基于寄存器操作的FIFO直写，暂时不适用DMA，中断暂时似乎也不用，但是我配置了中断，然后后面再看吧
 * 补充：
 * 1.
 * 在开发过程中，我似乎发现FIFO直写不太适合使用TSER，因为TSER是在CTSIZE的时候才会触发重载，而我才用FIFO直写只需要考虑FIFO中是否有空闲的空间提供给我写入待传输的数据，所以我暂时不需要考虑TSER，在每次传输的过程中直接写入当前数据长度到TSIZE即可。考虑到如果最终成品是一个嵌入式实时系统，那么后续还是要考虑我们的文件数据来源频率和我们传输效率之间的协调性
 * 2.
 * 直到快下班才发现，SPI这个全双工必须要主机发送数据才能驱动SCK的产生，简直逆天。所以除开主机自己发命令和写数据过去，其余时间都要发送0xFF，这样才能保证SCK的产生，然后SD卡才能正常工作。
 */

#include "main.h"

/**
 * 以下是SPI模块的初始化代码，配置成主机模式
 * SPI口初始化
 * 这里是针对SPI1的初始化
 * PA5->SPI1_SCK PA6->SPI1_MISO PA7->SPI1->MOSI
 * 这里配置为主从一对一，所以不需要NSS，如果需要NSS，则添加PA4即可
 */
// void SPI1_Init(void) {
//   uint32_t tempreg = 0;
//   RCC->AHB4ENR |= 1 << 0;  // 使能PORTA时钟   	//20241119  Bit0 GPIOAEN
//   RCC->APB2ENR |= 1 << 12; // SPI1时钟使能  		//20241119   RCC_APB2ENR
//                            // Bit12 SPI1EN  Bit13 SPI4EN

//   //配置SPI的时钟源
//   RCC->D2CCIP1R &= ~(7 << 12); // SPI123SEL[2:0]=0,清除原来的设置
//   RCC->D2CCIP1R |=
//       0x01
//       << 12; // SPI123SEL[2:0]=1,选择pll1_q_ck作为SPI1/2/3的时钟源,我将其设置为了32MHz

//   GPIO_InitTypeDef hspi;
//   hspi.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_4;
//   hspi.Mode = GPIO_MODE_AF_PP;
//   hspi.Pull = GPIO_PULLUP;
//   hspi.Speed = GPIO_SPEED_FREQ_HIGH;
//   hspi.Alternate = GPIO_AF5_SPI1;
//   // GPIO配置
//   HAL_GPIO_Init(GPIOA, &hspi);

//   //这里只针对SPI口初始化
//   RCC->APB2RSTR |=
//       1 << 13;                 // 复位SPI1			//20241119  Bit13 SPI1RST
//   RCC->APB2RSTR &= ~(1 << 13); // 停止复位SPI1  //20241119  Bit13 SPI1RST

//   SPI1->CFG1 = 6 << 28; // MBR[2:0]=0,128分频.
//   SPI1->CFG1 |= 7 << 0; // DSIZE[4:0]=7,设置SPI帧格式为8位,即字节传输
//   SPI1->CFG1 |=
//       0x00
//       << 5; // FTHVL[3:0]=0,设置SPI单个数据包的数据帧数为1，一个数据包包含一个字节
//   tempreg = (uint32_t)1 << 31; // AFCNTR=1,SPI保持对IO口的控制
//   tempreg |= 0 << 29;          // SSOE=0,禁止硬件NSS输出
//   tempreg |= 1 << 26;          // SSM=1,软件管理NSS脚
//   tempreg |= 1 << 25;          // CPOL=1,空闲状态下,SCK为高电平
//   tempreg |= 1 << 24;      // CPHA=1,数据采样从第2个时间边沿开始
//   tempreg |= 0 << 23;      // LSBFRST=0,MSB先传输
//   tempreg |= 1 << 22;      // MASTER=1,主机模式
//   tempreg |= 0 << 19;      // SP[2:0]=0,摩托罗拉格式
//   tempreg |= 0 << 17;      // COMM[1:0]=0,全双工通信
//   SPI1->CFG2 = tempreg;    // 设置CFG2寄存器
//   SPI1->CR2 &= 0xFFFF0000; // 设置TSIZE=0，配置传输的长度未知
//   SPI1->CR1 |=
//       0x01
//       << 12; // 内部SS信号输入电平，此时忽略SS引脚的I/O值。但是我们只对SD卡进行操作，也没必要用这个配置，暂时写在这里吧。
//   SPI1->CFG1 &= 0x00 << 14;   // Tx/Rx DMA数据流不使能
//   SPI1->I2SCFGR &= ~(1 << 0); // 选择SPI模式
//   SPI1->CR1 |= 1 << 0;
//   // SPE=1,使能SPI
//   /* 中断配置 */
//   // HAL_NVIC_EnableIRQ(SPI1_IRQn);
//   // HAL_NVIC_SetPriority(SPI1_IRQn, 14, 14);

//   // SPI1->IER |= 0x03 << 0; // RXP TXP
//   // SPI1->IER |= 0x03 << 3; // EOT/SUSP/TXC TXTF
// }

// /**
//  * SPI1速度设置函数
//  * SpeedSet:0~7
//  * SPI速度=spi_ker_ck/2^(SpeedSet+1)
//  * spi_ker_ck我们选择来自pll1_q_ck,为32Mhz
//  */
// void SPI1_SetSpeed(uint8_t SpeedSet) {
//   SpeedSet &= 0X07;         // 限制范围
//   SPI1->CR1 &= ~(1 << 0);   // SPE=0,SPI设备失能
//   SPI1->CFG1 &= ~(7 << 28); // MBR[2:0]=0,清除原来的分频设置
//   SPI1->CFG1 |= (uint32_t)SpeedSet << 28; // MBR[2:0]=SpeedSet,设置SPI1速度
//   SPI1->CR1 |= 1 << 0;                    // SPE=1,SPI设备使能
// }

// /**
//  * @brief
//  * 这是关于SPI1特用于SD卡的中断处理函数，由于涉及到多个中断处理信号，所以需要分多种情况进行处理
//  */
// void SPI1_IRQHandler(void) {}

// /**
//  * @brief 此函数用于发送单个字节数据
//  * @param data: 待发送的数据
//  */
// void SPI1_WriteByte(uint8_t data) {
//   while ((SPI1->SR & 1 << 1) == 0)
//     ; // 等待发送区空
//   *(volatile uint8_t *)&SPI1->TXDR =
//       data; //发送一个byte,以传输长度访问TXDR寄存器
//   while ((SPI1->SR & 1 << 0) == 0)
//     ; // 等待接收完一个byte
//   uint8_t dummy =
//       *(volatile uint8_t *)&SPI1->RXDR; //接收一个byte,以传输长度访问RXDR寄存器
// }

// /* SD卡配置 */
// int8_t SD_Init(void) {
//   uint32_t response;

//   GPIOA->BSRR = 1 << 4; // SPI1 NSS拉高
//   /* 1.发送至少74个空闲时钟 */
//   for (int i = 0; i < 10; i++) {
//     SPI1_WriteByte(0xFF);
//   }
//   GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS拉低

//   /* 2. 发送CMD0复位 */
//   SPI1_SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);
//   if (SPI1_SD_ReceiveResponse() != 0x01) {
//     return CMD0_ERROR;
//   }
//   GPIOA->BSRR = 1 << 4;       // SPI1 NSS拉高
//   SPI1_WriteByte(0xFF);       // 发送空字节延时
//   GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS拉低

//   /* 3. 发送CMD8检测协议版本 */
//   SPI1_SD_SendCmd(SD_CMD_CHECK_VERSION, 0x000001AA, 0x87);
//   // V1卡
//   if (SPI1_SD_ReceiveResponse() != 0x01) {
//     GPIOA->BSRR = 1 << 4;       // SPI1 NSS拉高
//     SPI1_WriteByte(0xFF);       // 发送空字节延时
//     GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS拉低
//     SPI1_SD_SendCmd(SD_CMD_SEND_OP_COND, 0, 0x01);
//     while (SPI1_SD_ReceiveResponse()) {
//       ;
//     }
//   }
//   // 如果是V2卡
//   else {
//     for (int i = 0; i < 4; i++) {
//       response = (response << 8 * i) | SPI1_SD_ReceiveResponse();
//     }
//     GPIOA->BSRR = 1 << 4;       // SPI1 NSS拉高
//     SPI1_WriteByte(0xFF);       // 发送空字节延时
//     GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS拉低
//     if (response != 0x000001AA) {
//       return CMD8_ERROR;
//     }
//     do {
//       SPI1_SD_SendCmd(SD_CMD_APP_CMD, 0, 0x01);
//       response = SPI1_SD_ReceiveResponse();
//       SPI1_SD_SendCmd(SD_ACMD_SD_SEND_OP_COND, 0x40000000, 0x01);
//       response = SPI1_SD_ReceiveResponse();
//     } while (response != 0x00);

//     /* 4. 发送CMD58读取OCR */
//     SPI1_SD_SendCmd(SD_CMD_READ_OCR, 0, 0x01);
//     GPIOA->BSRR = 1 << 4;       // SPI1 NSS拉高
//     SPI1_WriteByte(0xFF);       // 发送空字节延时
//     GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS拉低
//     response = 0;
//     for (int i = 0; i < 4; i++) {
//       response = (response << 8 * i) | SPI1_SD_ReceiveResponse();
//     }
//     if (response & 0x40000000) {
//       // SDHC/SDXC卡(块固定为512字节)
//     } else {
//       // SDSC卡
//       SPI1_SD_SendCmd(SD_CMD_SET_BLOCKLEN, SD_BLOCK_SIZE, 0x01);
//       if (SPI1_SD_ReceiveResponse() != 0x00) {
//         return CMD58_ERROR;
//       }
//     }
//   }

//   GPIOA->BSRR = 1 << 4; // SPI1 NSS拉高
//   SPI1_WriteByte(0xFF); // 发送空字节延时
//   return 0;
// }

// /**
//  * @brief 此函数用于处理连续发送多个帧数据的情况
//  * @note
//  * 此函数考虑前一组数据的传输情况，传输完成后再开启当前这一组，所以放在操作系统中可以将其挂起，等待前一组传输完成后再开启。但是这样似乎只能检测EOT，不然可能会导致意外的时序错误或者数据错误，导致无效数据占用SD卡的内存空间
//  */
// void SPI1_WriteSD(uint8_t *TxData) {
//   /* 判断上一次数据组是否已经完成传输，检查了EOT标志 */
//   // Version 1--with OS
//   if ((SPI1->SR & 1 << 3) == 0) {
//     // suspend，EOT中断恢复运行态
//   }

//   // Version 2--without OS
//   while ((SPI1->SR & 1 << 3) == 0) {
//     // 轮询占用CPU
//   }

//   /* 开始传输 */
//   SPI1->CR2 |= SD_BLOCK_SIZE + 3; // 设置传输长度
//   SPI1->CR1 |= 1 << 9;            // CSTART=1,启动传输
//   for (uint32_t i = 0; i < SD_BLOCK_SIZE; i++) {
//     while ((SPI1->SR & 1 << 1) == 0)
//       ; // 等待发送区空
//     *(volatile uint8_t *)&SPI1->TXDR =
//         TxData[i]; //发送一个byte,以传输长度访问TXDR寄存器
//     while ((SPI1->SR & 1 << 0) == 0)
//       ; // 等待接收完一个byte
//     uint8_t dummy = *(volatile uint8_t *)&SPI1
//                          ->RXDR; //接收一个byte,以传输长度访问RXDR寄存器
//   }
//   return;
// }

// /**
//  * @brief 此函数用于处理连续接收多个帧数据的情况
//  * @note 这个得考虑前一组数据的接收情况，接收完成前一组数据再接收下一组数据，
//  */
// void SPI1_ReadSD(uint8_t *RxData) {
//   /* 判断上一次数据组是否已经完成传输，检查了EOT标志 */
//   // Version 1--with OS
//   if ((SPI1->SR & 1 << 3) == 0) {
//     // suspend，EOT中断恢复运行态
//   }

//   // Version 2--without OS
//   while ((SPI1->SR & 1 << 3) == 0) {
//     // 轮询占用CPU
//   }

//   /* 开始传输 */
//   SPI1->CR2 |= SD_BLOCK_SIZE + 3; // 设置传输长度
//   SPI1->CR1 |= 1 << 9;            // CSTART=1,启动传输
//   for (uint32_t i = 0; i < SD_BLOCK_SIZE; i++) {
//     while ((SPI1->SR & 1 << 1) == 0)
//       ;
//     *(volatile uint8_t *)&SPI1->TXDR = 0xFF;
//     while ((SPI1->SR & 1 << 0) == 0)
//       ; // 等待接收完一个byte
//     RxData[i] = *(volatile uint8_t *)&SPI1
//                      ->RXDR; //接收一个byte,以传输长度访问RXDR寄存器
//   }
//   return;
// }

// /**
//  * @brief 此函数用于终止多块写操作
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
//  * @brief 此函数用于发送SD卡命令
//  * @param cmd: 命令
//  * @param addr: 地址
//  * @param crc: CRC校验码
//  */
// void SPI1_SD_SendCmd(uint8_t cmd, uint32_t addr, uint8_t crc) {
//   GPIOA->BSRR = 1 << 4 << 16; // SPI1 NSS拉低

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
//  * @brief 此函数用于接收SD卡回应
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