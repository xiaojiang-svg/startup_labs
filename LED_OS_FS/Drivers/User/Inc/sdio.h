#ifndef __SDIO_H
#define __SDIO_H

/**
 * @brief  Commands: CMDxx = CMD-number | 0x40
 */
#include "stdint.h"
#define SD_CMD_GO_IDLE_STATE 0       /*!< CMD0 = 0x40 */
#define SD_CMD_SEND_OP_COND 1        /*!< CMD1 = 0x41 */
#define SD_CMD_ALL_SEND_CID 2        /*!< CMD2 = 0x42 */
#define SD_CMD_SET_RELATIVE_ADDR 3   /*!< CMD3 = 0x43 */
#define SD_CMD_SEL_DES_CARD 7        /*!< CMD7 = 0x47 */
#define SD_CMD_SEND_IF_COND 8        /*!< CMD8 = 0x48 */
#define SD_CMD_SEND_CSD 9            /*!< CMD9 = 0x49 */
#define SD_CMD_SEND_CID 10           /*!< CMD10 = 0x4A */
#define SD_CMD_VOLTAGE_SWITCH 11     /*!< CMD11 = 0x4B */
#define SD_CMD_STOP_TRANSMISSION 12  /*!< CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS 13        /*!< CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN 16       /*!< CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK 17  /*!< CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK 18    /*!< CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT 23    /*!< CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK 24 /*!< CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK 25   /*!< CMD25 = 0x59 */
#define SD_CMD_PROG_CSD 27           /*!< CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT 28     /*!< CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT 29     /*!< CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT 30    /*!< CMD30 = 0x5E */
#define SD_CMD_ERASE_GRP_START 32 /*!< CMD32 = 0x60 */
#define SD_CMD_ERASE_GRP_END 33   /*!< CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR 34       /*!< CMD34 = 0x62 */
#define SD_CMD_UNTAG_ERASE_GROUP 37  /*!< CMD37 = 0x65 */
#define SD_CMD_ERASE 38              /*!< CMD38 = 0x66 */
#define SD_CMD_UNLOCK 42             /*!< CMD42 = 0x6A>*/
#define SD_CMD_READ_OCR 58           /*!< CMD58 */
#define SD_CMD_APP_CMD 55            /*!< CMD55 返回0x01*/
#define SD_ACMD_SD_SEND_OP_COND 41   /*!< ACMD41  返回0x00*/
#define SD_ACMD_ENABLE_4BIT 6       /*!< ACMD6   返回0x00*/

// SD卡回应标志
#define SD_RESPONSE_NO 0x00
#define SD_RESPONSE_R1 0x01
#define SD_RESPONSE_R1b 0x02
#define SD_RESPONSE_R2 0x03
#define SD_RESPONSE_R3 0x04
#define SD_RESPONSE_R7 0x05

// SD卡扇区大小
#define SD_SECTOR_SIZE 512

/*!< Data token start byte, 单块读起始Token */
#define SD_START_DATA_SINGLE_BLOCK_READ 0xFE

/*!< Data token start byte, 多块读起始Token */
#define SD_START_DATA_MULTIPLE_BLOCK_READ 0xFE

/*!< Data token start byte, 单块写起始Token */
#define SD_START_DATA_SINGLE_BLOCK_WRITE 0xFE

/*!< Data token start byte, 多块写起始Token */
#define SD_START_DATA_MULTIPLE_BLOCK_WRITE 0xFC

/*!< Data toke stop byte, 多块写停止Token */
#define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE 0xFD

// Response error
#define CMD0_ERROR -1
#define CMD1_ERROR -2
#define CMD2_ERROR -3
#define CMD3_ERROR -4
#define CMD7_ERROR -7
#define CMD8_ERROR -8
#define CMD9_ERROR -9
#define CMD12_ERROR -12
#define CMD13_ERROR -13
#define CMD16_ERROR -16
#define CMD17_ERROR -17
#define CMD18_ERROR -18
#define CMD24_ERROR -24
#define CMD25_ERROR -25
#define CMD32_ERROR -32
#define CMD33_ERROR -33
#define CMD38_ERROR -38
#define CMD55_ERROR -55
#define CMD58_ERROR -58
#define ACMD6_ERROR -6
#define ACMD41_ERROR -41

#define SD_TRANSFER_ERROR -1
#define WRITE_FIFO_ERROR -1
#define READ_FIFO_ERROR -2
#define VOLTAGE_SWITCH_ERROR -3
#define SDMMC_RXOVERR_ERROR -1
#define SDMMC_UNDERR_ERROR -2
#define TIMEOUT_ERROR -1000

// wait for response type
#define SDMMC_WAITRESP_NONE 0
#define SDMMC_WAITRESP_SHORT_REND_OR_CRCFAIL 1
#define SDMMC_WAITRESP_SHORT_REND_NONE_CRCFAIL 2
#define SDMMC_WAITRESP_LONG 3

// SD卡状态
#define SD_R1_BUSY 0x01
#define SD_DATA_READY (1 << 8)
#define SD_WRITE_OK 0x05
#define SD_OK 0x00

// SD卡类型
#define SDHC_XC 0x01
#define SDSC    0x00

#define SD_MDMA_SINGLE_BUFFER_TRANS_SIZE 128
#define BUFFER_SIZE 512
#define MDMA_STREAM MDMA_Channel0


// 全局变量
extern uint32_t buffer0[128];
extern uint32_t buffer1[128];
extern uint32_t csd[4];
extern uint32_t sd_rca;
extern uint32_t sd_type;

// 固件轮询
int8_t SD_Init(void);
void SDMMC1_Init(void);
int SDMMC1_SendCMD(uint8_t cmd, uint32_t arg, uint8_t is_trans, uint8_t wait);
uint32_t SDMMC1_GetResponse(uint8_t wait);
int SDMMC1_ReadFIFO(uint32_t *buffer, uint32_t length);
int SDMMC1_WriteFIFO(uint32_t *buffer, uint32_t length);
int SD_ReadSingleBlock(uint32_t block_addr, uint32_t *buffer);
int SD_WriteSingleBlock(uint32_t block_addr, uint32_t *buffer);
int SD_Erase(uint32_t start_addr, uint32_t end_addr);
static void SDMMC_MDMA_Config(void);
int SDMMC1_WaitBusy(void);

// IDMA协助
void IDMA_Config(uint8_t IDMAMODE, uint32_t buffer0, uint32_t buffer1);
int16_t SDMMC_WriteBlocks_IDMA(uint32_t block_addr, uint32_t block_nums);
int16_t SDMMC_ReadBlocks_IDMA(uint32_t block_addr, uint32_t block_nums);
uint32_t SD_GetSectorCount(void); 

void SD_Test(void);

#endif
