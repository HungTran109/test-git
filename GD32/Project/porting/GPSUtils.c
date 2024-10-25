/******************************************************************************
 * @file    	GPSUtils.c
 * @author  	
 * @version 	V1.0.0
 * @date    	10/11/2014
 * @brief   	
 ******************************************************************************/


/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include <stdlib.h>
#include "GPS.h"
#include "Utilities.h"
#include "string.h"

/******************************************************************************
                                   GLOBAL VARIABLES					    			 
 ******************************************************************************/
extern GPS_Manager_t GPS_Manager;
extern System_t xSystem;

/******************************************************************************
                                   GLOBAL FUNCTIONS					    			 
 ******************************************************************************/

/******************************************************************************
                                   DATA TYPE DEFINE					    			 
 ******************************************************************************/
#define	MAX_LONGITUDE 18000
#define	MAX_LATITUDE 9000
#define	KNOTS_TO_KM 1.852
#define	LATITUDE		1
#define	LONGITUDE	2

#define DELIMITER__ "," /* Change this value to the delimiter of your SourceFile*/

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
char GPSConvert[15] = {0};

/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/

/*****************************************************************************/
/**
 * @brief	:   Valid checksum from GPS message
 * @param	:  
 * @retval	:
 * @author	:	
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
uint8_t ValidCheckSum(char *GPSMessageBuffer)
{
    uint8_t GPSCRC = 0;
    uint8_t MessageCRC = 0;
    uint8_t i = 0;

    //Tinh checksum
    MessageCRC = 0;
    for(i = 1; i < strlen(GPSMessageBuffer); i++)
    {
        if(GPSMessageBuffer[i] == '*')
        {
            MessageCRC = 0xFF; //Bien tam
            break;
        }

        GPSCRC ^= GPSMessageBuffer[i];
    }

    //Ban tin khong co dau *
    if(MessageCRC != 0xFF)
    {        
        return 0;
    }

    //Lay gia tri checksum trong ban tin
    MessageCRC = GetHexNumberFromString(i + 1, GPSMessageBuffer) & 0xFF;

    //Dung checksum
    if(MessageCRC == GPSCRC)
    {        
        return 1;
    }
    
    return 0;
}
	
/*****************************************************************************
 * $GPRMC,053740.000,A,2503.6319,N,12136.0099,E,2.69,79.65,100106,,,A*53
 * $GPRMC,120650.000,A,2058.472447,N,10550.829105,E,0.000,0,071012,,,A*7A  //SIM908
 * $GPRMC,044518.0,A,2058.441820,N,10550.844794,E,0.0,119.2,170914,,,A*64 // UC20
 * $GNRMC,040132.100,A,2057.8901,N,10549.5083,E,0.00,269.70,031119,,,A*73	//L26
 * $GPRMC,041902.000,V,,,,,0.02,264.74,031119,,,N*49	//L26
 * $GPRMC,134241.00,A,2057.87608,N,10549.50682,E,0.061,,071119,,,A*78		//Ublox NEO-7N
 * RMC Data Format
 * Name                 Example             Units           Description
 * Message ID           $GPRMC                              RMC protocol header
 * UTC Time             053740.000                          hhmmss.sss
 * Status               A                                   A=data valid or V=data not valid
 * Latitude             2503.6319                           ddmm.mmmm
 * N/S Indicator        N                                   N=north or S=south
 * Longitude            12136.0099                          dddmm.mmmm
 * E/W Indicator        E                                   E=east or W=west
 * Speed over ground    2.69                knots           True
 * Course over ground   79.65               degrees
 * Date                 100106                              ddmmyy
 * Magnetic variation                       degrees
 * Variation sense                                          E=east or W=west (Not shown)
 * Mode                 A                                   A=autonomous, D=DGPS, E=DR
 * Checksum             *53
 * <CR> <LF>                                                End of message termination
 */
void GPS_ProcessNewMessage(void) 
{ 
	uint16_t Pos = 0, Length = 0, i = 0;
	double tempf = 0;
	uint32_t temp = 0;
	GPSInforStruct GPSTemp;
	char GPSConvert[12];
	char *GPS_Message;
	
	//Xu ly GNRMC module L96, L76,L26
	GPS_Message = strstr((char *)&GPS_Manager.DataBuffer.Buffer[Pos],"$GPRMC");
	if(GPS_Message == NULL)
	{
		GPS_Message = strstr((char *)&GPS_Manager.DataBuffer.Buffer[Pos],"$GNRMC");
	}

	if(GPS_Message != NULL)
	{
		if(ValidCheckSum(GPS_Message) == 0) return;
				
		//Bo qua phan $GPRMC dau tien, chay qua day phay thu nhat
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;
		if(Pos >= SMALL_BUFFER_SIZE) return;

		//Thoi gian
		GPSTemp.GPSTime.Hour = (GPS_Message[Pos] - 48) * 10 + (GPS_Message[Pos + 1] - 48);
		Pos += 2;
		GPSTemp.GPSTime.Minute = (GPS_Message[Pos] - 48) * 10 + (GPS_Message[Pos + 1] - 48);
		Pos += 2;
		GPSTemp.GPSTime.Second = (GPS_Message[Pos] - 48) * 10 + (GPS_Message[Pos + 1] - 48);
		Pos += 2;

		//Valid ?
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;
		if(Pos >= SMALL_BUFFER_SIZE) return;

		if(GPS_Message[Pos] != 'A')
		{
			GPS_Manager.Valid = 0;

			//Phan tich xong
			return;
		}
		
		if(GPS_Message[Pos] == 'A') GPSTemp.Valid = 1;

		//Vi do, bo qua den het dau phay
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;

		//Dem do dai du lieu, dem den khi gap dau cham: 2057.8901 -> 2057
		Length = 0;
		while(GPS_Message[Pos + Length] != '.' && Length < 6) Length++;
		if(Length > 5) return;
		
		//Xoa du lieu
		GPSTemp.CurrentPosition.ViDo.value  = 0;

		//2 ky tu phan degree
		Length -= 2;
		for(i = 0; i < Length; i++)
		{
			GPSTemp.CurrentPosition.ViDo.value = GPSTemp.CurrentPosition.ViDo.value * 10;
			GPSTemp.CurrentPosition.ViDo.value += GPS_Message[Pos++] - 48;
		}

		//Chuyen doi phan sau thanh so thap phan
		i = 0;
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE)
			GPSConvert[i++] = GPS_Message[Pos++];
		Pos++;
		if(Pos >= SMALL_BUFFER_SIZE) return;

		GPSConvert[i] = 0;
		tempf = atof(GPSConvert);
		GPSTemp.CurrentPosition.ViDo.value += (double) tempf / 60;

		//Vi do nam
		if(GPS_Message[Pos] == 'S') {
			GPSTemp.CurrentPosition.ViDo.value = GPSTemp.CurrentPosition.ViDo.value * -1;
		}

		//Bo qua phuong
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;
		if(Pos >= SMALL_BUFFER_SIZE) return;

		//Dem do dai du lieu, dem den khi gap dau cham: 10549.5083 -> 10549
		Length = 0;
		while(GPS_Message[Pos + Length] != '.' && Length < 6) Length++;
		if(Length > 5) return;

		//Xoa du lieu
		GPSTemp.CurrentPosition.KinhDo.value = 0;
		
		//2 ky tu phan degree
		Length -= 2;
		for(i = 0; i < Length; i++)
		{
			GPSTemp.CurrentPosition.KinhDo.value = GPSTemp.CurrentPosition.KinhDo.value * 10;
			GPSTemp.CurrentPosition.KinhDo.value += GPS_Message[Pos++] - 48;
		}

		i = 0;
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE)
			GPSConvert[i++] = GPS_Message[Pos++];
		Pos++;

		//Chuyen doi phan kinh do
		GPSConvert[i] = 0;
		tempf = atof(GPSConvert);
		GPSTemp.CurrentPosition.KinhDo.value += (double) tempf / 60;

		//Chuyen doi phuong
		if(GPS_Message[Pos] == 'W') {
			GPSTemp.CurrentPosition.KinhDo.value = GPSTemp.CurrentPosition.KinhDo.value * -1;
		}

		//Bo qua dau ','
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;
		if(Pos >= SMALL_BUFFER_SIZE) return;

		//Tinh van toc tu Knot sang km/h - 1Knot = 1.852 km/h
		temp = GetNumberFromString(Pos, (char*) (GPS_Message));	//Lay phan nguyen cua Knot VD: 15.36 -> 15*100
		temp = temp * 100;
		while(GPS_Message[Pos] != '.' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;
		temp += GetNumberFromString(Pos, (char*) (GPS_Message));	//Cong them phan thap phan cua Knot -> 1500 + 36
		
		//temp = gia tri Knot * 100
		GPSTemp.KnotSpeed = temp;
		
		//convert Km/h =1.852 * Knot 
		temp = temp * 13;
		temp = temp / 702;		//7.02 * 100
		GPSTemp.GPSSpeed = temp & 0xFFFF;
		
		//COG		
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;
		
		i = 0;
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE)
			GPSConvert[i++] = GPS_Message[Pos++];
		
		GPSConvert[i] = 0;
		GPSTemp.Course.value = atof(GPSConvert);
		
		if(Pos >= SMALL_BUFFER_SIZE) return;

		//Ngay thang
		while(GPS_Message[Pos] != ',' && Pos < SMALL_BUFFER_SIZE) Pos++;
		Pos++;
		GPSTemp.GPSTime.Day = (GPS_Message[Pos] - 48) * 10 + (GPS_Message[Pos + 1] - 48);
		Pos += 2;
		GPSTemp.GPSTime.Month = (GPS_Message[Pos] - 48) * 10 + (GPS_Message[Pos + 1] - 48);
		Pos += 2;
		GPSTemp.GPSTime.Year = (GPS_Message[Pos] - 48) * 10 + (GPS_Message[Pos + 1] - 48);
		Pos += 2;

		//Van toc qua cao thi bo luon du lieu
		if(GPSTemp.GPSSpeed >= 180)
		{
			GPSTemp.Valid = 0;
		  
			//Phan tich xong
			return;
		}
		
		//Copy du lieu vao cau truc du lieu chinh
		GPS_Manager.CurrentPosition.KinhDo.value = GPSTemp.CurrentPosition.KinhDo.value;
		GPS_Manager.CurrentPosition.ViDo.value = GPSTemp.CurrentPosition.ViDo.value;
		GPS_Manager.CurrentDateTime.Year = GPSTemp.GPSTime.Year;
		GPS_Manager.CurrentDateTime.Month = GPSTemp.GPSTime.Month;
		GPS_Manager.CurrentDateTime.Day = GPSTemp.GPSTime.Day;
		GPS_Manager.CurrentDateTime.Hour = GPSTemp.GPSTime.Hour;
		GPS_Manager.CurrentDateTime.Minute = GPSTemp.GPSTime.Minute;
		GPS_Manager.CurrentDateTime.Second = GPSTemp.GPSTime.Second;
		
		GPS_Manager.CurrentSpeed = GPSTemp.GPSSpeed;
		GPS_Manager.Valid = 10;
	}
}

/*****************************************************************************/
/**
  * @brief  Update date time for RTC from GPS, call every 1s
  * @param  None
  * @retval None
  */
void UpdateTimeFromGPS (void)
{
	uint8_t UpdateEnable = 0;
	static uint8_t UpdateTick = 0;
	
	UpdateTick++;
	if(UpdateTick >= 30)
	{
		if(GPS_IsValid())
		{
			UpdateEnable = 1;			
		}
		
		UpdateTick = 0;
	}
	
	if(GPS_Manager.Valid && xSystem.Rtc->GetDateTime().Year < 19)
    {
        UpdateEnable = 1;
    }
	
//	if(UpdateEnable)
//	{
//		printf("\rCap nhat thoi gian tu GPS : %u:%u:%u %u/%u/%u ",GPS_Manager.CurrentDateTime.Hour,
//				GPS_Manager.CurrentDateTime.Minute,GPS_Manager.CurrentDateTime.Second,
//				GPS_Manager.CurrentDateTime.Day,GPS_Manager.CurrentDateTime.Month,GPS_Manager.CurrentDateTime.Year);
//		
//        // Convert to GMT +7. 25200 = 7 * 3600
//		xSystem.Rtc->SetDateTime(GPS_Manager.CurrentDateTime, 25200);
//	}	
}

/********************************* END OF FILE *******************************/
