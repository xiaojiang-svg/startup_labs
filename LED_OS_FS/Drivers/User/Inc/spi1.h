#ifndef __SPI1_H
#define __SPI1_H

#include "stdint.h"
#include <stdint-gcc.h>


// // SPI总线速度设置 
// #define SPI_SPEED_2   		0
// #define SPI_SPEED_4   		1
// #define SPI_SPEED_8   		2
// #define SPI_SPEED_16  		3
// #define SPI_SPEED_32 			4
// #define SPI_SPEED_64 			5
// #define SPI_SPEED_128 		6
// #define SPI_SPEED_256 		7

// /**
//  * @brief  Commands: CMDxx = CMD-number | 0x40
//  */
// #define SD_CMD_GO_IDLE_STATE 0       /*!< CMD0 = 0x40 */
// #define SD_CMD_SEND_OP_COND 1        /*!< CMD1 = 0x41 */
// #define SD_CMD_CHECK_VERSION 8       /*!< CMD8 = 0x48 */
// #define SD_CMD_SEND_CSD 9            /*!< CMD9 = 0x49 */
// #define SD_CMD_SEND_CID 10           /*!< CMD10 = 0x4A */
// #define SD_CMD_STOP_TRANSMISSION 12  /*!< CMD12 = 0x4C */
// #define SD_CMD_SEND_STATUS 13        /*!< CMD13 = 0x4D */
// #define SD_CMD_SET_BLOCKLEN 16       /*!< CMD16 = 0x50 */
// #define SD_CMD_READ_SINGLE_BLOCK 17  /*!< CMD17 = 0x51 */
// #define SD_CMD_READ_MULT_BLOCK 18    /*!< CMD18 = 0x52 */
// #define SD_CMD_SET_BLOCK_COUNT 23    /*!< CMD23 = 0x57 */
// #define SD_CMD_WRITE_SINGLE_BLOCK 24 /*!< CMD24 = 0x58 */
// #define SD_CMD_WRITE_MULT_BLOCK 25   /*!< CMD25 = 0x59 */
// #define SD_CMD_PROG_CSD 27           /*!< CMD27 = 0x5B */
// #define SD_CMD_SET_WRITE_PROT 28     /*!< CMD28 = 0x5C */
// #define SD_CMD_CLR_WRITE_PROT 29     /*!< CMD29 = 0x5D */
// #define SD_CMD_SEND_WRITE_PROT 30    /*!< CMD30 = 0x5E */
// #define SD_CMD_SD_ERASE_GRP_START 32 /*!< CMD32 = 0x60 */
// #define SD_CMD_SD_ERASE_GRP_END 33   /*!< CMD33 = 0x61 */
// #define SD_CMD_UNTAG_SECTOR 34       /*!< CMD34 = 0x62 */
// #define SD_CMD_ERASE_GRP_START 35    /*!< CMD35 = 0x63 */
// #define SD_CMD_ERASE_GRP_END 36      /*!< CMD36 = 0x64 */
// #define SD_CMD_UNTAG_ERASE_GROUP 37  /*!< CMD37 = 0x65 */
// #define SD_CMD_ERASE 38              /*!< CMD38 = 0x66 */
// #define SD_CMD_READ_OCR 58           /*!< CMD58 */
// #define SD_CMD_APP_CMD 55            /*!< CMD55 返回0x01*/
// #define SD_ACMD_SD_SEND_OP_COND 41   /*!< ACMD41  返回0x00*/

// // SD卡回应标志
// #define SD_RESPONSE_NO     0x00
// #define SD_RESPONSE_R1     0x01
// #define SD_RESPONSE_R1b    0x02
// #define SD_RESPONSE_R2     0x03
// #define SD_RESPONSE_R3     0x04
// #define SD_RESPONSE_R7     0x05

// // SD卡块大小
// #define SD_BLOCK_SIZE      512

// /*!< Data token start byte, 单块读起始Token */
// #define SD_START_DATA_SINGLE_BLOCK_READ 0xFE

// /*!< Data token start byte, 多块读起始Token */
// #define SD_START_DATA_MULTIPLE_BLOCK_READ 0xFE

// /*!< Data token start byte, 单块写起始Token */
// #define SD_START_DATA_SINGLE_BLOCK_WRITE 0xFE

// /*!< Data token start byte, 多块写起始Token */
// #define SD_START_DATA_MULTIPLE_BLOCK_WRITE 0xFC

// /*!< Data toke stop byte, 多块写停止Token */
// #define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE 0xFD

// // Response error
// #define CMD0_ERROR -1
// #define CMD1_ERROR -2
// #define CMD8_ERROR -3
// #define ACMD41_ERROR -4
// #define CMD58_ERROR -5

// void SPI1_Init(void);
// void SPI1_SetSpeed(unsigned char SpeedSet);
// void SPI1_WriteByte(uint8_t data);
// void SPI1_SD_SendCmd(uint8_t cmd, uint32_t addr, uint8_t crc);
// uint8_t SPI1_SD_ReceiveResponse(void);
// void SPI1_SD_StopWrite(void);
// int8_t SD_Init(void);
// void SPI1_WriteSD(uint8_t *TxData);
// void SPI1_ReadSD(uint8_t *RxData);

#endif

