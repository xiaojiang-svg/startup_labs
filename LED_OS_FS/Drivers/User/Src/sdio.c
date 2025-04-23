#include "sdio.h"
#include "main.h"
#include "stm32h743xx.h"
#include "stm32h7xx_ll_sdmmc.h"

SD_HandleTypeDef hsd1;

uint32_t sd_rca = 0;   // RCA相对地址--在CMD3之后会被赋值
uint32_t csd[4] = {0}; // CSD寄存器
uint32_t sd_type = -1; // SD卡类型
uint32_t ocr = -1;     // OCR寄存器

void SDMMC1_Init(void) {
  RCC->AHB4ENR |= 0x03 << 2; //使能GPIOC和GPIOD
  RCC->D1CCIPR |= 1 << 16;   // 选择PLL2_R_CK作为时钟源
  RCC->AHB3ENR |= 1 << 16;   //使能SDMMC1和SDMMC1延迟时钟

  /* 配置GPIOC和GPIOD */
  GPIO_InitTypeDef hspi;

  hspi.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_NOPULL;
  hspi.Speed = GPIO_SPEED_FREQ_HIGH;
  hspi.Alternate = GPIO_AF12_SDMMC1;
  HAL_GPIO_Init(GPIOC, &hspi);

  hspi.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &hspi);

  /* 配置SDMMC1 */
  hsd1.Instance = SDMMC1;

  RCC->AHB3RSTR |= RCC_AHB3RSTR_SDMMC1RST; // 复位SDMMC1
  RCC->AHB3RSTR &= ~RCC_AHB3RSTR_SDMMC1RST;

  SDMMC1->POWER = (3 << SDMMC_POWER_PWRCTRL_Pos); // 电源控制--上电
  SDMMC1->CLKCR =
      (0 << SDMMC_CLKCR_HWFC_EN_Pos) | (0 << SDMMC_CLKCR_NEGEDGE_Pos) |
      (1 << SDMMC_CLKCR_WIDBUS_Pos) | (0 << SDMMC_CLKCR_PWRSAV_Pos) |
      (125 << SDMMC_CLKCR_CLKDIV_Pos); // 时钟控制寄存器
  SDMMC1->DTIMER = 0xFFFFFFFF;         // 最大超时时间
  SDMMC1->DLEN = 0;                    // 关闭数据长度
  SDMMC1->DCTRL = 0;                   // 禁用数据通道

  /* 配置中断 */
  HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 1, 1);
}

int SDMMC1_SendCMD(uint8_t cmd, uint32_t arg, uint8_t is_trans,
                   uint8_t wait_resp) {
  // 清除之前的标志位
  SDMMC1->ICR = 0xFFFFFFFF;

  // 设置命令参数
  SDMMC1->ARG = arg;
  __DSB();

  // 设置命令寄存器
  SDMMC1->CMD = (cmd & 0x3F) | (wait_resp << SDMMC_CMD_WAITRESP_Pos) |
                (is_trans << SDMMC_CMD_CMDTRANS_Pos) |
                (1 << SDMMC_CMD_CPSMEN_Pos);
  volatile uint32_t timeout = 0x1000000;
  // 等待命令完成
  if (!wait_resp) {
    do {
      if (SDMMC1->STA & SDMMC_STA_CMDSENT)
        break;
      if (SDMMC1->STA & (SDMMC_STA_CTIMEOUT | SDMMC_STA_CCRCFAIL)) {
        return -2;
      }
    } while (timeout--);
    if (timeout == 0)
      return -1;
  }

  // 等待响应完成--响应超时为固定值64个SDMMC_CK时钟周期
  if (wait_resp) {
    timeout = 0x1000000;
    while (!(SDMMC1->STA & SDMMC_STA_CMDREND) && timeout--)
      ;
    if (timeout == 0)
      return -1;
  }

  return 0;
}

uint32_t SDMMC1_GetResponse(uint8_t response) { return SDMMC1->RESP1; }

void SDMMC1_IdleClock(void) {
  for (int i = 0; i < 1e5; ++i) {
    __NOP();
  }
}

int8_t SD_Init(void) {
  uint32_t response = 0x00;
  uint32_t sta = 0x00;

  // IdleClock
  SDMMC1_IdleClock();

  // 1. 复位SD卡
  if (SDMMC1_SendCMD(SD_CMD_GO_IDLE_STATE, 0, 0, SDMMC_WAITRESP_NONE) != 0) {
    return CMD0_ERROR;
  }

  // 2. 发送CMD8命令（检查电压范围）
  if (SDMMC1_SendCMD(SD_CMD_SEND_IF_COND, 0x1AA, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD8_ERROR;
  }
  response = SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);

  // 3. 发送ACMD41命令
  uint32_t timeout = 0x100000;
  while (timeout--) {
    SDMMC1_SendCMD(SD_CMD_APP_CMD, 0, 0, SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
    if ((SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) & (1 << 5)) ==
        0)
      continue;
    SDMMC1_SendCMD(SD_ACMD_SD_SEND_OP_COND, 0xC0300000, 0,
                   SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
    sta = SDMMC1->STA;
    ocr = SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
    if (ocr & (1 << 31)) {
      break;
    }
  }
  if (timeout == 0) {
    return ACMD41_ERROR;
  }

  // 读取OCR寄存器
  if (ocr & (1 << 30))
    sd_type = SDHC_XC;
  else
    sd_type = SDSC;

  // 是否支持1.8V以切换到4bit高速模式
  if (ocr & (1 << 24)) {
    uint32_t timeout = 0x100000;
    SDMMC1->POWER |= SDMMC_POWER_VSWITCHEN;
    SDMMC1_SendCMD(SD_CMD_VOLTAGE_SWITCH, 0, 0,
                   SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
    while (!(SDMMC1->CLKCR & SDMMC_STA_CKSTOP) && timeout--)
      ;
    // BUSYD0为低电平
    if (!(SDMMC1->STA & SDMMC_STA_BUSYD0)) {
      SDMMC1->POWER |= SDMMC_POWER_VSWITCH;
    } else
      return VOLTAGE_SWITCH_ERROR;
    // 等待VSWEND标志
    while (!(SDMMC1->POWER & SDMMC_STA_VSWEND))
      ;
    // 检查BUSYD0
    if (!(SDMMC1->STA & SDMMC_STA_BUSYD0))
      return VOLTAGE_SWITCH_ERROR;
  }

  // 4. 发送CMD2命令--获取CID
  if (SDMMC1_SendCMD(SD_CMD_ALL_SEND_CID, 0, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD2_ERROR;
  }
  response = SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);

  // 5. 发送CMD3命令--获取RCA
  if (SDMMC1_SendCMD(SD_CMD_SET_RELATIVE_ADDR, 0, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD3_ERROR;
  }
  response = SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
  sd_rca = response >> 16;

  // 发送CMD9命令--获取CSD
  if (SDMMC1_SendCMD(SD_CMD_SEND_CSD, sd_rca << 16, 0,
                     SDMMC_WAITRESP_LONG) != 0) {
    return CMD9_ERROR;
  }
  sta = SDMMC1->STA;

  // 6.
  // 发送CMD7命令--选择卡(根据调试结果来看，似乎在单卡的情况下，它直接默认选择了，我使用CMD7后反而取消了选择)
  timeout = 0x10000;
  // do {
  SDMMC1_SendCMD(SD_CMD_SEL_DES_CARD, sd_rca << 16, 0,
                 SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
  response = SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
  // } while(!((response & (0x0F<<9)) == (0x04<<9)) && timeout--);
  sta = SDMMC1->STA;

  // CMD13查了一下Card Status
  SDMMC1_SendCMD(SD_CMD_SEND_STATUS, sd_rca << 16, 0,
                 SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
  response = SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
  sta = SDMMC1->STA;

  // 7. 发送ACMD6--使能4bit模式
  if (SDMMC1_SendCMD(SD_CMD_APP_CMD, sd_rca << 16, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD55_ERROR;
  }
  if (SDMMC1_SendCMD(SD_ACMD_ENABLE_4BIT, 0x2, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return ACMD6_ERROR;
  }
  response = SDMMC1->RESP1;
  sta = SDMMC1->STA;

  // 回调时钟为25MHz，最大传输频率达到50MB/s，发送一个块的时间为10us
  SDMMC1->CLKCR &= ~(0x3FF);
  SDMMC1->CLKCR |= (2 << SDMMC_CLKCR_CLKDIV_Pos);
  SDMMC1_IdleClock();
  return 0;
}

int SD_ReadSingleBlock(uint32_t block_addr, uint32_t *buffer) {
  // 设置DLEN\DTIMER\DCTRL
  SDMMC1->DLEN = SD_SECTOR_SIZE;
  SDMMC1->DTIMER = 0x1000000;
  SDMMC1->DCTRL =
      (1 << SDMMC_DCTRL_DTDIR_Pos) | (9 << SDMMC_DCTRL_DBLOCKSIZE_Pos);

  // 1. 发送CMD17命令--读取单块数据
  if (SDMMC1_SendCMD(SD_CMD_READ_SINGLE_BLOCK, block_addr, 1,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD17_ERROR;
  }
  uint32_t response = SDMMC1->RESP1;

  // 2. 等待数据准备
  uint32_t timeout = 0x1000000;
  while (timeout--) {
    if (SDMMC1->STA & SDMMC_STA_RXOVERR) {
      return SDMMC_RXOVERR_ERROR;
    }
    if (SDMMC1->STA & SDMMC_STA_RXFIFOHF) {
      break;
    }
  }
  if (timeout == 0)
    return TIMEOUT_ERROR;

  // 3. 读取数据
  if (SDMMC1_ReadFIFO(buffer, SD_SECTOR_SIZE / 4)) {
    return READ_FIFO_ERROR;
  }

  // 4. 等待数据传输完成
  timeout = 0x1000000;
  while (!(SDMMC1->STA & SDMMC_STA_DATAEND) && timeout--)
    ;
  if (timeout == 0)
    return TIMEOUT_ERROR;
  return 0;
}

int SDMMC1_ReadFIFO(uint32_t *buffer, uint32_t length) {
  uint32_t i;
  for (i = 0; i < length;) {
    // while (!(SDMMC1->STA & (SDMMC_STA_RXFIFOHF | SDMMC_STA_DBCKEND)))
    //   ;
    while (i < length && !(SDMMC1->STA & SDMMC_STA_RXFIFOE)) {
      buffer[i++] = SDMMC1->FIFO;
    }
  }
  return 0;
}

int SD_WriteSingleBlock(uint32_t block_addr, uint32_t *buffer) {
  uint32_t sta = 0;
  uint32_t response = 0;

  // 设置DLEN\DTIMER\DCTRL
  SDMMC1->DLEN = SD_SECTOR_SIZE;
  SDMMC1->DTIMER = 0x1000000;
  SDMMC1->DCTRL =
      (0 << SDMMC_DCTRL_DTDIR_Pos) | (9 << SDMMC_DCTRL_DBLOCKSIZE_Pos);

  // 1. 发送CMD24命令--写入单块数据
  if (SDMMC1_SendCMD(SD_CMD_WRITE_SINGLE_BLOCK, block_addr, 1,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD24_ERROR;
  }

  response = SDMMC1->RESP1;
  sta = SDMMC1->STA;

  // 2. 等待数据准备
  uint32_t timeout = 0x1000000;
  while (timeout--) {
    if (SDMMC1->STA & SDMMC_STA_TXUNDERR) {
      return SDMMC_UNDERR_ERROR;
    }
    if (SDMMC1->STA & SDMMC_STA_TXFIFOE) {
      break;
    }
  }
  if (timeout == 0)
    return TIMEOUT_ERROR;
  // 至少两个时钟周期SDMMC_CK
  for (int i = 0; i < 40; ++i) {
    __NOP();
  }

  uint32_t count = SDMMC1->DCOUNT;
  // 3. 写入数据
  if (SDMMC1_WriteFIFO(buffer, SD_SECTOR_SIZE / 4)) {
    return WRITE_FIFO_ERROR;
  }

  // 4. 等待数据传输完成
  timeout = 0x1000000;
  while (!(SDMMC1->STA & SDMMC_STA_DATAEND) && timeout--)
    ;
  uint32_t stack = SDMMC1->STA;
  if (timeout == 0)
    return TIMEOUT_ERROR;

  return 0;
}

int SDMMC1_WriteFIFO(uint32_t *buffer, uint32_t length) {
  uint32_t i;
  for (i = 0; i < length;) {
    while (i < length && !(SDMMC1->STA & SDMMC_STA_TXFIFOF)) {
      SDMMC1->FIFO = buffer[i++];
    }
  }
  return 0;
}

int SD_Erase(uint32_t start_addr, uint32_t end_addr) {
  uint32_t response = 0;
  uint32_t sta = 0;

  // 等待 DPSM 停止
  while (SDMMC1->STA & SDMMC_STA_DPSMACT)
    ;

  // 等待 D0 不再忙
  // while (!(SDMMC1->STA & SDMMC_STA_BUSYD0END))
  //   ;
  // SDMMC1->ICR = SDMMC_ICR_BUSYD0ENDC;

  // 可选：确认卡状态为 TRAN
  do {
    SDMMC1_SendCMD(SD_CMD_SEND_STATUS, sd_rca << 16, 0,
                 SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
    response = SDMMC1->RESP1;
    sta = SDMMC1->STA;
  } while (((response >> 9) & 0x0F) != 4); // TRAN

  // 1. 发送CMD32命令--设置起始擦除地址
  if (SDMMC1_SendCMD(SD_CMD_ERASE_GRP_START, start_addr, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD32_ERROR;
  }
  response = SDMMC1->RESP1;
  sta = SDMMC1->STA;

  // 2. 发送CMD33命令--设置结束擦除地址
  if (SDMMC1_SendCMD(SD_CMD_ERASE_GRP_END, end_addr, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD33_ERROR;
  }
  response = SDMMC1->RESP1;
  sta = SDMMC1->STA;

  // 3. 发送CMD38命令--擦除数据
  if (SDMMC1_SendCMD(SD_CMD_ERASE, 0x00, 0,
                     SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
    return CMD38_ERROR;
  }
  response = SDMMC1->RESP1;
  sta = SDMMC1->STA;
  

  return SDMMC1_WaitBusy();
}

int SDMMC1_WaitBusy(void) {
  uint32_t timeout = 0x1000000;
  while (timeout--) {
    // 发送CMD13命令--查询卡状态
    if (SDMMC1_SendCMD(SD_CMD_SEND_STATUS, sd_rca, 0,
                       SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
      return CMD13_ERROR;
    }
    if (!(SDMMC1_GetResponse(SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) &
          SD_R1_BUSY)) {
      return 0;
    }
  }
  if (timeout == 0)
    return -1;
  return 0;
}

/****************************************************** IDMA VERSION
 * ***********************************************************/
uint32_t buffer0[128]; // 512字节
uint32_t buffer1[128]; // 512字节
void IDMA_Config(uint8_t IDMAMODE, uint32_t buffer0, uint32_t buffer1) {
  SDMMC1->IDMACTRL = (SDMMC_IDMA_IDMAEN) | (IDMAMODE << SDMMC_IDMA_IDMABMODE_Pos);
  SDMMC1->IDMABSIZE = (0x10 << 5); // 一个缓冲区128字
  SDMMC1->IDMABASE0 = (uint32_t)buffer0;
  SDMMC1->IDMABASE1 = (uint32_t)buffer1;

  return;
}

int16_t SDMMC_WriteBlocks_IDMA(uint32_t block_addr, uint32_t block_nums) {
  // 清除标志位
  SDMMC1->ICR = 0xFFFFFFFF;

  uint32_t sta = 0;
  uint32_t response = 0;
  // 设置DLEN\DTIMER\DCTRL
  SDMMC1->DLEN = SD_SECTOR_SIZE * block_nums;
  SDMMC1->DTIMER = 0xFFFFFFFF;
  SDMMC1->DCTRL =
      (0 << SDMMC_DCTRL_DTDIR_Pos) | (9 << SDMMC_DCTRL_DBLOCKSIZE_Pos);

  // 使能IDMA，只能在DPSM未启动时使能IDMA
  IDMA_Config(1, (uint32_t)buffer0, (uint32_t)buffer1);

  // 1. 发送写命令
  if (block_nums > 1) {
    SDMMC1->DCTRL |= (3 << SDMMC_DCTRL_DTMODE_Pos);
    if (SDMMC1_SendCMD(SD_CMD_WRITE_MULT_BLOCK, block_addr, 1,
                       SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
      return CMD25_ERROR;
    }
  } else {
    if (SDMMC1_SendCMD(SD_CMD_WRITE_SINGLE_BLOCK, block_addr, 1,
                       SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
      return CMD24_ERROR;
    }
  }
  sta = SDMMC1->STA;
  response = SDMMC1->RESP1;

  // 2. 等待数据准备
  uint32_t timeout = 0x1000000;
  while (timeout--) {
    if (SDMMC1->STA & SDMMC_STA_TXUNDERR) {
      return SDMMC_UNDERR_ERROR;
    }
    if (SDMMC1->STA & SDMMC_STA_TXFIFOE) {
      break;
    }
  }
  if (timeout == 0)
    return TIMEOUT_ERROR;
  // 至少两个时钟周期SDMMC_CK
  for (int i = 0; i < 40; ++i) {
    __NOP();
  }

  // 3. 写入数据

  // 4. 等待数据传输完成
  timeout = 0x1000000;
  while (!(SDMMC1->STA & SDMMC_STA_DATAEND) && --timeout)
    ;
  sta = SDMMC1->STA;
  uint32_t left = SDMMC1->DCOUNT;
  uint32_t mdma_left_units = MDMA_Channel0->CBNDTR >> MDMA_CBNDTR_BRC_Pos;
  uint32_t mdma_left_bytes = MDMA_Channel0->CBNDTR & 0xFFFF;
  if (timeout == 0)
    return TIMEOUT_ERROR;

  // 5. 停止传输
  if(block_nums > 1) {
    do {
      SDMMC1_SendCMD(SD_CMD_STOP_TRANSMISSION, 0, 0,
                     SDMMC_WAITRESP_SHORT_REND_NONE_CRCFAIL);
      response = SDMMC1->RESP1;
      sta = SDMMC1->STA;
    } while (sta & SDMMC_STA_BUSYD0);
  }

  // // 6. 等待SD卡准备好
  // do {
  //   SDMMC1_SendCMD(SD_CMD_SEND_STATUS, sd_rca << 16, 0,
  //                  SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL);
  //   response = SDMMC1->RESP1;
  //   sta = SDMMC1->STA;
  // } while(((response >> 9) & 0x0F) == 0x06);

  // // 清除标志位
  // SDMMC1->ICR = 0xFFFFFFFF;
  // 关闭IDMA
  SDMMC1->IDMACTRL &= ~SDMMC_IDMA_IDMAEN;
  return 0;
}

int16_t SDMMC_ReadBlocks_IDMA(uint32_t block_addr, uint32_t block_nums) {
  // 清除标志位
  SDMMC1->ICR = 0xFFFFFFFF;

  uint32_t response = 0;
  uint32_t sta = 0;
  sta = SDMMC1->STA;

  // 设置DLEN\DTIMER\DCTRL
  SDMMC1->DLEN = SD_SECTOR_SIZE * block_nums;
  SDMMC1->DTIMER = 0x1000000;
  SDMMC1->DCTRL =
      (1 << SDMMC_DCTRL_DTDIR_Pos) | (9 << SDMMC_DCTRL_DBLOCKSIZE_Pos);

  // 使能IDMA，只能在DPSM未启动时使能IDMA
  IDMA_Config(1, (uint32_t)buffer0, (uint32_t)buffer1);
  // 1. 发送读命令
  if (block_nums > 1) {
    if (SDMMC1_SendCMD(SD_CMD_READ_MULT_BLOCK, block_addr, 1,
                       SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
      return CMD18_ERROR;
    }
  } else {
    if (SDMMC1_SendCMD(SD_CMD_READ_SINGLE_BLOCK, block_addr, 1,
                       SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL) != 0) {
      return CMD17_ERROR;
    }
  }

  sta = SDMMC1->STA;
  response = SDMMC1->RESP1;

  uint32_t timeout = 0x1000000;
  // 2. 等待数据准备
  while (!(SDMMC1->STA & SDMMC_STA_RXFIFOHF) && timeout--) {
    __NOP();
  }

  // 3. 读取数据

  // 4. 等待数据传输完成
  timeout = 0x1000000;
  while (!(SDMMC1->STA & SDMMC_STA_DATAEND) && --timeout)
    ;
  sta = SDMMC1->STA;
  uint32_t left = SDMMC1->DCOUNT;

  if (timeout == 0)
    return TIMEOUT_ERROR;

  // 5. 停止传输
  if (block_nums > 1) {
    do {
      SDMMC1_SendCMD(SD_CMD_STOP_TRANSMISSION, 0, 0,
                     SDMMC_WAITRESP_SHORT_REND_NONE_CRCFAIL);
      response = SDMMC1->RESP1;
      sta = SDMMC1->STA;
    } while (sta & SDMMC_STA_BUSYD0);
  }

  if(SDMMC1->STA & SDMMC_STA_BUSYD0END) {
    sta = SDMMC1->STA;
    SDMMC1->ICR = SDMMC_ICR_BUSYD0ENDC;
  }

  // 关闭IDMA
  SDMMC1->IDMACTRL &= ~SDMMC_IDMA_IDMAEN;
  return 0;
}

uint32_t SD_GetSectorCount(void) {
  return ((((csd[2] & 0x3F) << 16) | ((csd[3]>>16) & 0xFFFF)) + 1) * 1024;
}

void SD_Test(void) {
  uint32_t block_addr = 0;
  uint32_t block_nums = 2;

  int write_return;
  int read_return;
  int sdmmc_sta;

  uint32_t response;

  SDMMC1_Init();
  SD_Init();
  for (int i = 0; i < 128; ++i) {
    buffer0[i] = i;
    buffer1[i] = 2 * i;
  }
  write_return = SDMMC_WriteBlocks_IDMA(block_addr, block_nums);
  // write_return = SD_WriteSingleBlock(0, buffer0);

  sdmmc_sta = SDMMC1->STA;
  for (int i = 0; i < 128; ++i) {
    buffer0[i] = 0;
    buffer1[i] = 0;
  }

  // read_return = SD_ReadSingleBlock(1, buffer0);
  read_return = SDMMC_ReadBlocks_IDMA(block_addr, block_nums);
  sdmmc_sta = SDMMC1->STA;

  // 擦除块
  SD_Erase(0, 1);

  read_return = SDMMC_ReadBlocks_IDMA(block_addr, block_nums);
  sdmmc_sta = SDMMC1->STA;

  return;
}
