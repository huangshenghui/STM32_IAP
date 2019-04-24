#include "flash.h"
#include "iap.h"


volatile uint32_t FlashData[FLASH_BUFF];



//读取指定地址的字(32位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t FLASH_ReadWord(uint32_t faddr)
{
	return *(volatile uint32_t*)faddr; 
}


//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(32位)数
void FLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   	
{
	uint32_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=FLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}




//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
uint32_t STMFLASH_GetFlashSector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return ADDR_FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return ADDR_FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return ADDR_FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return ADDR_FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return ADDR_FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return ADDR_FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return ADDR_FLASH_SECTOR_6;
	else if(addr<0x08008000)return ADDR_FLASH_SECTOR_7;

	return ADDR_FLASH_SECTOR_7;	
}





//从指定地址开始写入指定长度的数据
//特别注意:因为STM32的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F(注意：最后16字节，用于OTP数据块锁定，别乱写！！)
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
void FLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
{ 
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	uint32_t SectorError=0;
	uint32_t addrx=0;
	uint32_t endaddr=0;	
	//if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
    
	HAL_FLASH_Unlock();             //解锁	
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
    
	//if(addrx<0X08008000)
	//{
		/*while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			 if(FLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				FlashEraseInit.TypeErase= FLASH_TYPEERASE_PAGES;       //擦除类型，扇区擦除 
				FlashEraseInit.PageAddress=0x08007000;//STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
				FlashEraseInit.NbPages=1;                             //一次只擦除一个扇区
				
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//发生错误了	
				}
				
			}else
			addrx+=4;
			FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);                //等待上次操作完成
		}
		*/
			FlashEraseInit.TypeErase= FLASH_TYPEERASE_PAGES;       //擦除类型，扇区擦除 
			FlashEraseInit.PageAddress=0x08007000;//STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
			FlashEraseInit.NbPages=1;                             //一次只擦除一个扇区
			
			if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
			{
				//break;//发生错误了	
			}
			FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);                //等待上次操作完成
//	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE );            //等待上次操作完成
	if(FlashStatus==HAL_OK)
	{
		 while(WriteAddr<endaddr)//写数据
		 {
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=4;
			pBuffer++;
		}  
	}
	HAL_FLASH_Lock();           //上锁
} 



void read_par(void)
{
	FLASH_Read(FLASH_PAR_ADDR,(uint32_t *)FlashData,FLASH_BUFF);  
	if((FlashData[FLASH_BOOT_ADDR]<=10)&&(0!=FlashData[FLASH_BOOT_ADDR]))
	{
		my_system.boot=FlashData[FLASH_BOOT_ADDR];
	
	}
	else
	{
		my_system.boot=3;
	}
	if(0!=FlashData[FLASH_TAG_ADDR])
	{
		//my_system.tag=FlashData[FLASH_TAG_ADDR];
	
	}
	else
	{
	//	my_system.tag=0;
	}
	
	

}




