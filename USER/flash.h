#ifndef __FLASH_H
#define __FLASH_H


#include "main.h"



/* Private define ------------------------------------------------------------*/
#define STM32_FLASH_SIZE 128				//����STM32��FLASH��С(��λK)0x08000000-0x0801ffff
#define STM32_FLASH_BASE 0x08000000		//STM32 ��ַ

//ÿҳ2K��ÿ������2ҳ����4K


//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 4 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08001000) 	//����1��ʼ��ַ, 4 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08002000) 	//����2��ʼ��ַ, 4 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08003000) 	//����3��ʼ��ַ, 4 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08004000) 	//����4��ʼ��ַ, 4 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08005000) 	//����5��ʼ��ַ, 4 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08006000) 	//����6��ʼ��ַ, 4 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08007000) 	//����7��ʼ��ַ, 4 Kbytes  


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
