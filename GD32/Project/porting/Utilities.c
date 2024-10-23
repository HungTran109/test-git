/******************************************************************************
 * @file    	Utilities.c
 * @author  	Phinht
 * @version 	V1.0.0
 * @date    	15/01/2014
 * @brief   	
 ******************************************************************************/


/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
 #include <string.h>
#include "Utilities.h"

/******************************************************************************
                                   GLOBAL VARIABLES					    			 
 ******************************************************************************/
 
 /******************************************************************************
                                   GLOBAL FUNCTIONS					    			 
 ******************************************************************************/


/******************************************************************************
                                   DATA TYPE DEFINE					    			 
 ******************************************************************************/

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/

/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/


/*****************************************************************************/
/**
 * @brief	:  	Get Hexa number from string
 * @param	:   
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
uint32_t GetHexNumberFromString(uint16_t BeginAddress, char* Buffer)
{
    uint32_t Value = 0;
    uint16_t tmpCount = 0;

    tmpCount = BeginAddress;
    Value = 0;
    while(Buffer[tmpCount] && tmpCount < 1024)
    {
        if((Buffer[tmpCount] >= '0' && Buffer[tmpCount] <= '9') || (Buffer[tmpCount] >= 'A' && Buffer[tmpCount] <= 'F') || (Buffer[tmpCount] >= 'a' && Buffer[tmpCount] <= 'f'))
        {
            Value *= 16;

            if(Buffer[tmpCount] == '1') Value += 1;
            if(Buffer[tmpCount] == '2') Value += 2;
            if(Buffer[tmpCount] == '3') Value += 3;
            if(Buffer[tmpCount] == '4') Value += 4;
            if(Buffer[tmpCount] == '5') Value += 5;
            if(Buffer[tmpCount] == '6') Value += 6;
            if(Buffer[tmpCount] == '7') Value += 7;
            if(Buffer[tmpCount] == '8') Value += 8;
            if(Buffer[tmpCount] == '9') Value += 9;

            if(Buffer[tmpCount] == 'A' || Buffer[tmpCount] == 'a') Value += 10;
            if(Buffer[tmpCount] == 'B' || Buffer[tmpCount] == 'b') Value += 11;
            if(Buffer[tmpCount] == 'C' || Buffer[tmpCount] == 'c') Value += 12;
            if(Buffer[tmpCount] == 'D' || Buffer[tmpCount] == 'd') Value += 13;
            if(Buffer[tmpCount] == 'E' || Buffer[tmpCount] == 'e') Value += 14;
            if(Buffer[tmpCount] == 'F' || Buffer[tmpCount] == 'f') Value += 15;
        }
        else break;

        tmpCount++;
    }

    return Value;
}
/*****************************************************************************/
/**
 * @brief	:  	Tinh check sum trong mot mang buffer
 * @param	:   
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
uint16_t CalculateCheckSum(uint8_t* Buffer, uint16_t BeginAddress, uint16_t Length)
{
    uint32_t tempChecksum = 0;
    uint16_t i = 0;

    for(i = BeginAddress; i < BeginAddress + Length; i++)
        tempChecksum += Buffer[i];
	
    return(uint16_t) (tempChecksum);
}

/***************************************************************************************************************************/
/*
 * 	Tinh check sum CRC 16 
 *
 */
#define ISO15693_PRELOADCRC16	0xFFFF 
#define ISO15693_POLYCRC16      0x8408 
#define ISO15693_MASKCRC16      0x0001
#define ISO15693_RESIDUECRC16   0xF0B8
 
uint16_t CalculateCRC16(uint8_t *DataIn, uint8_t NbByte)
{
	int16_t   i,j; 
	int32_t ResCrc = ISO15693_PRELOADCRC16;
            
	for (i = 0; i < NbByte; i++) 
	{ 
		ResCrc = ResCrc ^ DataIn[i];
		for (j = 8; j > 0; j--) 
		{
			ResCrc = (ResCrc & ISO15693_MASKCRC16) ? (ResCrc>>1) ^ ISO15693_POLYCRC16 : (ResCrc>>1); 
		}
	} 
 
	return ((~ResCrc) & 0xFFFF);
}

///* Table of CRC values for high�order byte */
//uint8_t const auchCRCHi[] = { 
//	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
//	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
//	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
//	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
//	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
//	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
//	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
//	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
//	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
//	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
//	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
//	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
//	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
//	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
//	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
//	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
//	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
//	0x40
//} ;

/* Table of CRC values for low�order byte */ 
//uint8_t const auchCRCLo[] = { 
//	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
//	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
//	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
//	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
//	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
//	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
//	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
//	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
//	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
//	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
//	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
//	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
//	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
//	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
//	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
//	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
//	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
//	0x40
//}; 

//uint16_t CRC16Custom(uint8_t *Buff, uint16_t DataLen)  /* The function returns the CRC as a unsigned short type */ 
//{
//	uint8_t CRCHi = 0xFF ;  /* high byte of CRC initialized  */ 
//	uint8_t CRCLo = 0xFF ;  /* low byte of CRC initialized  */ 
//	
//	uint16_t uIndex ;  /* will index into CRC lookup table  */ 
//	while (DataLen--)  /* pass through message buffer  */ 
//	{ 
//		uIndex = CRCLo ^ (*Buff++) ;   /* calculate the CRC  */ 
//		CRCLo = CRCHi ^ auchCRCHi[uIndex]; 
//		CRCHi = auchCRCHi[uIndex]/2; 
//	}
//	
//	return (CRCHi << 8 | CRCLo);
//}

/*****************************************************************************/
/**
 * @brief	:  	
 * @param	:   
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */
int16_t FindIndexOfChar(char CharToFind, char *BufferToFind)
{
    uint8_t tmpCount = 0;
    uint16_t DoDai = 0;

    /* Do dai du lieu */
    DoDai = strlen(BufferToFind);

    for(tmpCount = 0; tmpCount < DoDai; tmpCount++)
    {
        if(BufferToFind[tmpCount] == CharToFind) return tmpCount;
    }
    return -1;
}
/*****************************************************************************/
/**
 * @brief	:  	Copy parameters
 * @param	:   
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */

uint8_t CopyParameter(char* BufferSource, char* BufferDes, char FindCharBegin, char FindCharEnd)
{
    int16_t ViTriBatDau = FindIndexOfChar(FindCharBegin, BufferSource);
    int16_t ViTriKetThuc = FindIndexOfChar(FindCharEnd, BufferSource);
    int16_t tmpCount, i = 0;

    /* Kiem tra dinh dang du lieu */
    if(ViTriBatDau == -1 || ViTriKetThuc == -1) return 0;
    if(ViTriKetThuc - ViTriBatDau <= 1) return 0;

    for(tmpCount = ViTriBatDau + 1; tmpCount < ViTriKetThuc; tmpCount++)
    {
        BufferDes[i++] = BufferSource[tmpCount];
    }

    BufferDes[i] = 0;

    return 1;
}
/*****************************************************************************/
/**
 * @brief	:  	Ham doc mot so trong chuoi bat dau tu dia chi nao do
 *				Buffer = abc124mff thi GetNumberFromString(3,Buffer) = 123
 * @param	:   
 * @retval	:
 * @author	:	Phinht
 * @created	:	15/01/2014
 * @version	:
 * @reviewer:	
 */

uint32_t GetNumberFromString(uint16_t BeginAddress, char* Buffer)
{
    uint32_t Value = 0;
    uint16_t tmpCount = 0;

    tmpCount = BeginAddress;
    Value = 0;
    while(Buffer[tmpCount] && tmpCount < 1024)
    {
        if(Buffer[tmpCount] >= '0' && Buffer[tmpCount] <= '9')
        {
            Value *= 10;
            Value += Buffer[tmpCount] - 48;
        }
        else break;

        tmpCount++;
    }

    return Value;
}
