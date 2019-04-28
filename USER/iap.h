#ifndef _IAP_H_
#define _IAP_H_

#include "main.h"
#include "string.h"
#include <stdio.h>
#include "math.h"
#include "stdlib.h"

typedef struct
{
	uint32_t boot;
	uint32_t baud;
	uint8_t mode;
	uint32_t counter;
	uint8_t flag_10s;
	uint8_t end_flag;
	
} SYS_PAR;



typedef struct
{
	uint8_t start;
	uint8_t number;
	uint8_t complement;
	uint8_t data[1024];
	uint8_t crc[2];
} FRAME_L_STR;

typedef struct
{
	uint8_t start;
	uint8_t number;
	uint8_t complement;
	uint8_t data[128];
	uint8_t crc[2];
} FRAME_S_STR;


typedef union
{
	uint8_t DATA[1035];
	FRAME_L_STR DATA_L;
	FRAME_S_STR DATA_S;

} IAP_DATA;


typedef union 
{
	uint32_t _int;
	uint16_t _short[2];
	uint8_t _byte[4];
} MyInt;



#define  SOH  0x01//128字节数据包开始
#define  STX  0x02//1024字节数据包开始
#define  EOT  0x04//结束传输
#define  ACK  0x06//回应
#define  NAK  0x15//没有回应
#define  CCC  0x43




#define TX1BUFSIZE 50
#define RX1BUFSIZE 1050






extern volatile unsigned char txbuf_UART1[TX1BUFSIZE];			// transmit buffer for UART1
extern volatile unsigned char rxbuf_UART1[RX1BUFSIZE];	// receive buffer for UART1

// queue management variables
extern volatile unsigned char *push_tx1;					// transmit buffer push pointer
extern volatile unsigned char *pull_tx1;					// transmit buffer pull pointer
extern volatile unsigned char *push_rx1;					// receive buffer push pointer
extern volatile unsigned char *pull_rx1;					// receive buffer pull pointer
extern volatile unsigned int tx1_status;						// queue status for txbuf_UART2
extern volatile unsigned int rx1_status;	

;
extern unsigned char gotCommand;
extern unsigned char gotData;

extern volatile SYS_PAR my_system;


extern FRAME_L_STR *IAP_L;
extern FRAME_S_STR *IAP_S;



void iap_init(void);

void iap_analysis(void);
void iap_load_app(uint32_t appxaddr);
void usart1_Analysis(void);

#endif
