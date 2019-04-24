#ifndef __FLASH_H
#define __FLASH_H


#include "main.h"



/* Private define ------------------------------------------------------------*/
#define STM32_FLASH_SIZE 128				//定义STM32的FLASH大小(单位K)0x08000000-0x0801ffff
#define STM32_FLASH_BASE 0x08000000		//STM32 基址

//每页2K，每个扇区2页就是4K


//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 4 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08001000) 	//扇区1起始地址, 4 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08002000) 	//扇区2起始地址, 4 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08003000) 	//扇区3起始地址, 4 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08004000) 	//扇区4起始地址, 4 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08005000) 	//扇区5起始地址, 4 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08006000) 	//扇区6起始地址, 4 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08007000) 	//扇区7起始地址, 4 Kbytes  


#define  FLASH_BUFF   10
#define  FLASH_PAR_ADDR  0x0801e000

#define  FLASH_BOOT_ADDR   0
#define  FLASH_TAG_ADDR    1


#define  FLASH_IAP_ADDR  0x08000000  //24k
#define  FLASH_APP1_ADDR  0x08006000	//48k
#define  FLASH_APP2_ADDR  0x08012000	//48k





extern volatile uint32_t FlashData[FLASH_BUFF];

void read_par(void);
void FLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);
void FLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);  

#endif
