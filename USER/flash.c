#include "flash.h"
#include "iap.h"


volatile uint32_t FlashData[FLASH_BUFF];



//��ȡָ����ַ����(32λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
uint32_t FLASH_ReadWord(uint32_t faddr)
{
	return *(volatile uint32_t*)faddr; 
}


//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(32λ)��
void FLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   	
{
	uint32_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=FLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}




//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
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





//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F(ע�⣺���16�ֽڣ�����OTP���ݿ�����������д����)
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
void FLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
{ 
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	uint32_t SectorError=0;
	uint32_t addrx=0;
	uint32_t endaddr=0;	
	//if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
    
	HAL_FLASH_Unlock();             //����	
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
    
	//if(addrx<0X08008000)
	//{
		/*while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			 if(FLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				FlashEraseInit.TypeErase= FLASH_TYPEERASE_PAGES;       //�������ͣ��������� 
				FlashEraseInit.PageAddress=0x08007000;//STMFLASH_GetFlashSector(addrx);   //Ҫ����������
				FlashEraseInit.NbPages=1;                             //һ��ֻ����һ������
				
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//����������	
				}
				
			}else
			addrx+=4;
			FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);                //�ȴ��ϴβ������
		}
		*/
			FlashEraseInit.TypeErase= FLASH_TYPEERASE_PAGES;       //�������ͣ��������� 
			FlashEraseInit.PageAddress=0x08007000;//STMFLASH_GetFlashSector(addrx);   //Ҫ����������
			FlashEraseInit.NbPages=1;                             //һ��ֻ����һ������
			
			if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
			{
				//break;//����������	
			}
			FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);                //�ȴ��ϴβ������
//	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE );            //�ȴ��ϴβ������
	if(FlashStatus==HAL_OK)
	{
		 while(WriteAddr<endaddr)//д����
		 {
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		}  
	}
	HAL_FLASH_Lock();           //����
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




