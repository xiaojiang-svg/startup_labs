/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* Declarations of disk functions */
#include "ff.h"     /* Obtains integer types */
#include "sdio.h"   /* 存储器的头文件 */

/* Definitions of physical drive number for each drive */
#define DEV_SD 0    // SD卡
#define SPI_FLASH 1 // SPI Flash

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive number to identify the drive */
) {
  DSTATUS status = STA_NOINIT; // 设备当前状态
  // 设备号判断
  switch (pdrv) {
  case DEV_SD: /* SD Card */
    status = STA_READY; // 设备就绪
    break;
  case SPI_FLASH: /* SPI Flash */
    break;
  default:
    status = STA_NOINIT;
  }

  return status;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive number to identify the drive */
) {
  DSTATUS status = STA_NOINIT; // 设备当前状态

  switch (pdrv) {
  case DEV_SD: /* SD Card */
    if (SD_Init() == SD_OK) {
      status = STA_READY; // 设备就绪
    } else {
      status = STA_NOINIT;
    }
    break;
  case SPI_FLASH: /* SPI_FLASH */
    break;

  default:
    status = STA_NOINIT;
  }

  return status;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv,  /* Physical drive number to identify the drive */
                  BYTE *buff, /* DDEV_SD buffer to store read dDEV_SD */
                  LBA_t sector, /* Start sector in LBA */
                  UINT count    /* Number of sectors to read */
) {
  DRESULT res = RES_PARERR;
  int result  = 0;
  switch (pdrv) {
  case DEV_SD:
    if ((result = SDMMC_ReadBlocks_IDMA(sector, count)) == 0) {
      res = RES_OK;
    } else {
      res = RES_ERROR;
    }
    break;
  case SPI_FLASH:
    break;
  default:
    res = RES_PARERR;
  }

  return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, /* Physical drive nmuber to identify the drive */
                   const BYTE *buff, /* DDEV_SD to be written */
                   LBA_t sector,     /* Start sector in LBA */
                   UINT count        /* Number of sectors to write */
) {
  DRESULT res;
  int result;

  switch (pdrv) {
  case DEV_SD:
    if((result = SDMMC_WriteBlocks_IDMA(sector, count)) == 0) {
      res = RES_OK;
    } else {
      res = RES_ERROR;
    }
    break;
  case SPI_FLASH:
    break;
  default:
    res = RES_PARERR;
  }

  return res;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
                   BYTE cmd,  /* Control code */
                   void *buff /* Buffer to send/receive control dDEV_SD */
) {
  DRESULT res = RES_PARERR;
  int result;

  switch (pdrv) {
  case DEV_SD:
    switch (cmd) {
    case CTRL_SYNC: /* Make sure that no pending write process */
      res = RES_OK;
      break;
    case GET_SECTOR_COUNT: /* Get number of sectors on the disk (DWORD) */
      *(DWORD *)buff = SD_GetSectorCount();
      res            = RES_OK;
      break;
    case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
      *(DWORD *)buff = SD_BLOCK_SIZE;
      res            = RES_OK;
      break;
    case GET_SECTOR_SIZE: /* Get sector size (BYTE) */
      *(WORD *)buff = SD_SECTOR_SIZE;
      res           = RES_OK;
      break;
    default:
      res = RES_PARERR;
    }
    break;
  case SPI_FLASH:
    res = RES_PARERR;
    break;
  default:
    res = RES_PARERR;
  }

  return res;
}
