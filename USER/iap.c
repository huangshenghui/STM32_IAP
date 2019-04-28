#include "iap.h"
#include "usart.h"
#include "flash.h"
#include "crc.h"


typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.   
iapfun jump2app; 

volatile unsigned char txbuf_UART1[TX1BUFSIZE]={0};			// transmit buffer for UART1
volatile unsigned char rxbuf_UART1[RX1BUFSIZE]={0};	// receive buffer for UART1

// queue management variables
volatile unsigned char *push_tx1=txbuf_UART1;					// transmit buffer push pointer
volatile unsigned char *pull_tx1=txbuf_UART1;					// transmit buffer pull pointer
volatile unsigned char *push_rx1=rxbuf_UART1;					// receive buffer push pointer
volatile unsigned char *pull_rx1=rxbuf_UART1;					// receive buffer pull pointer
volatile unsigned int tx1_status=0;						// queue status for txbuf_UART2
volatile unsigned int rx1_status=0;	
unsigned char gotCommand;
unsigned char gotData;



volatile SYS_PAR my_system;
//IAP_DATA iap_data;
FRAME_L_STR *IAP_L=(void *)rxbuf_UART1;
FRAME_S_STR *IAP_S=(void *)rxbuf_UART1;



void cmdResetMCU(void);
void cmdBaudrate(void);
void cmdBootToApp(void);

 typedef struct
{
	char *command;
	void (*commandHandler) (void) ;
} HOST_COMMAND ;

const HOST_COMMAND commands[] = {
	{"BOOT:",        cmdBootToApp},//	
	//{"RVER:",       cmdReadVersion},//��ȡ�汾��
	{"BAUD:",      	cmdBaudrate},
	//{"RERY:",       cmdRecovery}
	{"RMCU:\0",      cmdResetMCU}//������Ƭ��	
}; 



void cmdBootToApp(void)
{
	my_system.mode=1;
	printf("C");
}


void cmdResetMCU(void)
{
	
	__set_PRIMASK(1);//�ر����ж�
	NVIC_SystemReset();//��λ��Ƭ��

}


void cmdBaudrate(void)
{
	uint32_t serial;
	if(6 == strlen((void*)rxbuf_UART1))
	{
		printf("BAUD:%d",my_system.baud);
	}
	else
	{
		serial=atoi((void *)(rxbuf_UART1+5));
		if((9600<=serial)&&(serial<=115200))
		{
			my_system.baud=serial;
			FlashPar[FLASH_BAUD_ADDR]=serial;
			FLASH_Write(FLASH_PAR_ADDR,(uint32_t *)FlashPar,FLASH_PAR_BUFF);
			printf("ACK\r\n");
		}
		else
		{
			printf("ERR\r\n");
		}
	}

}



void iap_init(void)
{
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)push_rx1, 1);

}


//���������ļ���FLASH
void bin2flash(uint8_t *serial,uint32_t addr,uint32_t size)
{
	uint32_t i,j;
	
	MyInt data;
	

	  for(i=0,j=0;i<(size-1);i+=4)
	{
		data._byte[0]=*(serial+0);
		data._byte[1]=*(serial+1);
		data._byte[2]=*(serial+2);
		data._byte[3]=*(serial+3);
		FlashApp[j++]= data._int;
		serial+=4;
	 
	}
	FLASH_Write(addr,  (uint32_t *)FlashApp, (size/4));
}


//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(uint32_t appxaddr)
{ 
	
	if(((*(uint32_t *)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
   
		 HAL_RCC_DeInit();
  
		jump2app=(iapfun)*(uint32_t *)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		 __set_MSP(*(uint32_t *)appxaddr);				//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		
		jump2app();									//��ת��APP.
	}
}	


//IAP���ݽ��ս׶εķ���
void iap_analysis(void)
{
	uint16_t crc1,crc2;
	uint8_t temp;
	static uint16_t num=0;
	static uint32_t iap_addr;
	
	if(1==gotData)
	{
		gotData=0;
		switch(rxbuf_UART1[0])
		{
			case SOH://128 byte
				if(my_system.end_flag>=2)//���һ�� Ҳ��128�ֽ�
				{
						num++;
						my_system.end_flag=0;
						temp=ACK;
						printf("%c",temp);
						/**************jump to new app*******************/
						iap_load_app(FLASH_APP1_ADDR);
				}
				if(0==IAP_S->number)//����� �����ݰ�
				{					
						crc1=CRC16(IAP_S->data,128);
						crc2=(IAP_S->crc[0]<<8)+IAP_S->crc[1];
						if(crc1==crc2)
						{
							temp=ACK;
							printf("%c",temp);
							printf("C");
							iap_addr=FLASH_APP1_ADDR;
						}
						else
						{
							temp=NAK;
							printf("%c",temp);
						}
					
				}
				else
				{
					num=IAP_S->number;
					crc1=CRC16(IAP_S->data,1024);
					crc2=(IAP_S->crc[0]<<8)+IAP_S->crc[1];
					if(crc1==crc2)
					{
						/***********do something************/
						//bin2flash(IAP_S->data,(FLASH_APP1_ADDR+(num-1)*128),128);
						bin2flash(IAP_S->data,iap_addr,128);
						temp=ACK;
						printf("%c",temp);
						iap_addr=(FLASH_APP1_ADDR+128);
					}
					else
					{
						temp=NAK;
						printf("%c",temp);
					}
				
				}
				
				push_rx1 = rxbuf_UART1;
				rx1_status=0;
				break;
			case STX://1024 byte
				num=IAP_L->number;
				crc1=CRC16(IAP_L->data,1024);
				crc2=(IAP_L->crc[0]<<8)+IAP_L->crc[1];
				if(crc1==crc2)
				{
					/***********do something************/
					//bin2flash(IAP_L->data,(FLASH_APP1_ADDR+(num-1)*1024),1024);
					bin2flash(IAP_L->data,iap_addr,1024);
					temp=ACK;
					printf("%c",temp);
					iap_addr=(FLASH_APP1_ADDR+1024);
				}
				else
				{
					temp=NAK;
					printf("%c",temp);
				}
				push_rx1 = rxbuf_UART1;
				rx1_status=0;
				break;
			case EOT:
				my_system.end_flag++;
				if(my_system.end_flag>=2)
				{
					temp=ACK;
					printf("%c",temp);
					printf("C");
				}
				else
				{
					temp=NAK;
					printf("%c",temp);
				}
				push_rx1 = rxbuf_UART1;
				rx1_status=0;
				break;
			default://�ط�
					temp=NAK;
					printf("%c",temp);
					push_rx1 = rxbuf_UART1;
					rx1_status=0;
			break;
		
		}
	
	}


}









//�����������
void usart1_Analysis(void)
{
	
	
	uint32_t i;
	
	if (gotCommand == 1 )	//ȷ���յ�����
   {
         	gotCommand = 0;		 

          	 for (i = 0; i < (sizeof(commands) / sizeof(HOST_COMMAND)); i++)//�����ַ�ƥ��
         	  {
            	if (strncmp(commands[i].command, (void *)rxbuf_UART1, strlen(commands[i].command)) == 0)		//
            	{
				        	 (*commands[i].commandHandler)();		//
				          	break;
            	}
         	 } 
			     if (i == (sizeof(commands) / sizeof(HOST_COMMAND)))
				   printf("ERR\r\n");
		     
  }
	 


}


