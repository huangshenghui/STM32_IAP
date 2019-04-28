#ifndef _CRC_H_
#define _CRC_H_

#include "main.h"




void crc_init(void);
uint16_t CRC16( uint8_t * pucFrame, uint32_t usLen );


#endif /*_CRC_H*/

