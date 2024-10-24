/******************************************************************************
 * @file    	Transfer.c
 * @author  	Phinht
 * @version 	V1.0.0
 * @date    	03/03/2016
 * @brief   	Dung de trao doi du lieu voi Blackbox
 ******************************************************************************/

/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "Hardware.h"
#include "DataDefine.h"
#include "Transfer.h"
#include "Utilities.h"
#include "MeasureInput.h"
#include "HardwareManager.h"
#include "Version.h"
#include "InternalFlash.h"
#include "Main.h"
#include "DriverUART.h"
#include "I2CFM.h"
#include "LED7.h"
#include "GPS.h"

/******************************************************************************
                                   GLOBAL VARIABLES					    			 
 ******************************************************************************/
extern System_t xSystem;
//FWUpdate_t FWUpdate;

extern KT_Write_List_t KT_Write_List;
extern KT_Read_List_t KT_Read_List;

/******************************************************************************
                                   GLOBAL FUNCTIONS					    			 
 ******************************************************************************/

/******************************************************************************
                                   DATA TYPE DEFINE					    			 
 ******************************************************************************/

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/
BlackBoxManager_t BlackBoxManager;
SmallBuffer_t BlackBoxBuffer;
uint8_t BLB_TxBuffer[45];

uint8_t esp32RxMessageTimeout = 0;
uint8_t esp32RebootTimeout = 0;

//static uint8_t DataOutTimeOut = 0;
//static uint16_t TimeOut1s = 0;
//static uint16_t TimeOut5s = 0;
//static uint16_t PingTimeOut = 0;

/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/
static void ProcessNewBlackboxData(void);
//static void XuLyBanTin(uint8_t *BanTinBLB, uint8_t Length);
//static void XuLyBanTinYeuCauDuLieu(uint8_t *Buffer);
//static void XuLyBanTinCauHinh(uint8_t *Buffer);
//static void XuLyBanTinBatDauUpdate(uint8_t *Buffer);
//static void XuLyBanTinDuLieuUpdate(uint8_t *Buffer);
//static void XuLyBanTinKetThucUpdate(uint8_t *Buffer);
//void GuiBanTinTrangThaiGhe(void);
//static void GuiBanTinThongSo(void);

/*****************************************************************************/
/**
 * @brief	:  	Tick every 1s
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	04/03/2016
 * @version	:
 * @reviewer:	
 */
char espMessage[200];
uint32_t esp_die_cnt = 0;
void ESP32_ManagerTick(void)
{
    /** Gui cac thong tin dinh ky len mach master
	* FM,LOCAL_MODE=<FM/MIC/IDLE>,FREQ=<FREQ>,VOL=<>,SNR=<>,RSSI=<>,GPS=<VD,KD>,REG12=65,VERSION=4,CRC=12345#
	*/

    uint8_t index = 0;
    uint8_t localVolume = xSystem.Parameter.LocalVolume;

    memset(espMessage, 0, sizeof(espMessage));

    /* Local volume */
    index = sprintf(espMessage, "FM|SNR=%u|RSSI=%u|dBm=%d|",
                    xSystem.Status.FM.SNR, xSystem.Status.FM.RSSI, xSystem.Status.FM.dBm);

    /* Local/remote mode */
    if (xSystem.Parameter.Mode == MODE_IDLE)
    {
        switch (xSystem.Parameter.ModeLocal)
        {
        case MODE_FM_LOCAL:
            index += sprintf(&espMessage[index], "MODE=LOCAL_FM|FREQ=%u|", xSystem.Status.FM.Frequency / 1000); /* don vi KHz */
            localVolume = xSystem.Status.FM.Volume;
            break;
        case MODE_MIC:
            index += sprintf(&espMessage[index], "%s", "MODE=LOCAL_MIC|");
            break;
        case MODE_LINE_IN:
            index += sprintf(&espMessage[index], "%s", "MODE=LOCAL_LINE|");
            break;
        case MODE_IDLE:
            index += sprintf(&espMessage[index], "%s", "MODE=LOCAL_IDLE|");
            break;
        default:
            break;
        }
    }
    else
    {
        index += sprintf(&espMessage[index], "MODE=REMOTE|");

        /* Tan so FM remote dang phat & FM volume */
        if (xSystem.Parameter.Mode == MODE_FM_REMOTE)
            index += sprintf(&espMessage[index], "FREQ=%u|", xSystem.Status.FM.Frequency / 1000); /* FM Freq don vi KHz */
    }

    /* Volume */
    index += sprintf(&espMessage[index], "VOL=%u|", localVolume);

    /* GPS */
    char *gps_pos = &espMessage[index];
    if (GPS_IsValid())
    {
        index += sprintf(&espMessage[index], "GPS=%.6f,%.6f|",
                         GPS_GetPosition().ViDo.value, GPS_GetPosition().KinhDo.value);
    }
    else
    {
        index += sprintf(&espMessage[index], "%s", "GPS=0.0000,0.0000|");
    }
//    DEBUG_INFO("%s\r\n", gps_pos);

    /* Giá trị Read thanh ghi KT0935 */
    for (uint8_t i = 0; i < KT_REG_LIST_MAX; i++)
    {
        if (KT_Read_List.List[i].isReadDone)
        {
            index += sprintf(&espMessage[index], "REG%u=%u|", KT_Read_List.List[i].reg, KT_Read_List.List[i].value);

            KT_Read_List.List[i].isReadDone = 0;
            KT_Read_List.List[i].reg = 0;
            KT_Read_List.List[i].value = 0;

            //Nếu có nhiều thanh ghi -> chỉ gửi 5 thanh ghi 1 lần
            if (i > 5 || index > 160)
                break;
        }
    }

    /* Firmware Version */
    index += sprintf(&espMessage[index], "VERSION=%u|", FIRMWARE_VERSION_CODE);

    /* Add checksum */
    uint16_t crc16 = CalculateCRC16((uint8_t *)espMessage, index);
    index += sprintf(&espMessage[index], "CRC=%05u#", crc16);

    /* Send to ESP32 */
    UART_Puts(ESP32_UART, espMessage);

    /** Giam sat trang thai cua ESP32, truong hop bi treo khi bat nguon -> reset */
    if (xSystem.Status.isESP32Enable)
    {
        if (esp32RxMessageTimeout++ >= 30)
        {
            esp32RxMessageTimeout = 0;
            DEBUG_INFO("\r\t!!! WARNING: ESP32 has died, reboot him....!!!\r\n");

//            GPIO_ResetBits(GPS_UART_GPIO, GPS_RX_PIN);
            xSystem.Status.isESP32Enable = 0;
            esp32RebootTimeout = 3; //cho 3s sau thi Enable lai esp32
            if (esp_die_cnt++ > 400)
            {
                esp_die_cnt = 0;
//                NVIC_SystemReset();
            }
        }
    }
    else
    {
        if (esp32RebootTimeout)
        {
            esp32RebootTimeout--;
            if (esp32RebootTimeout == 0)
            {
                DEBUG_INFO("\r\t\t ------ START ESP32 again -------\r\n");
//                GPIO_SetBits(GPS_UART_GPIO, GPS_RX_PIN);
                xSystem.Status.isESP32Enable = 1;
                esp32RxMessageTimeout = 0;
            }
        }
    }

#if 0
	DEBUG_INFO("\rSend state: %s", espMessage);
#endif
}

/*****************************************************************************/
/**
 * @brief	:  	Tick every 10ms
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	04/03/2016
 * @version	:
 * @reviewer:	
 */
void ESP32_UART_Tick(void)
{
    if (BlackBoxBuffer.State)
    {
        BlackBoxBuffer.State--;
        if (BlackBoxBuffer.State == 0)
        {
            ProcessNewBlackboxData();
            BlackBoxBuffer.BufferIndex = 0;
        }
    }
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	05/03/2016
 * @version	:
 * @reviewer:	
 */
//static void BlackBoxInit(void)
//{
//	BlackBoxManager.TestLEDTime = 0;
//	BlackBoxManager.SendDataDelay = 0;
//}

uint8_t isValidFreq(uint16_t Freq)
{
    for (uint8_t i = 0; i < sizeof(xSystem.Parameter.Frequency); i++)
    {
        if (Freq == xSystem.Parameter.Frequency[i])
            return 1;
    }
    return 0xFF;
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	03/03/2016
 * @version	:
 * @reviewer:	
 */
//static uint8_t ESP32Message[250];
static void ProcessNewBlackboxData(void)
{
    esp32RxMessageTimeout = 0;
    // DEBUG_INFO("ESP32 message size %d: %s\r\n", BlackBoxBuffer.BufferIndex, BlackBoxBuffer.Buffer);

    /** Xu ly ban tin tu ESP32 */
    /** 4G,MODE=<FM/4G/MIC/NONE/IDLE>,FREQ_RUN=<10270>,FREQ1=<>,FREQ2=<>,FREQ3=<>,VOL=<70>,THRESHOLD=<90>,CRC=12345#
		* Dinh dang gia tri tan so: = Freq (Hz) /10000, vi du: 102.7MHz = 102700000 /10000 = 10270 
	* Lệnh ghi/đọc trực tiếp thanh ghi của KT0935R: KT_WRITE<xxx>=<yy> trong đó: xxx là địa chỉ thanh ghi, 3 digit, yy: giá trị cần ghi
	* KT_READ<xxx>
	*/
    
    if (strstr((char *)BlackBoxBuffer.Buffer, "TEST=1"))
    {
        xSystem.Parameter.enter_test_mode = 1;
    }
    
    if (xSystem.Parameter.enter_test_mode)
    {
        static uint32_t delay = 5;
        if (delay)
        {
            delay--;
            FM_SetVolume(100);
        }
    }
            
    if (strstr((char *)BlackBoxBuffer.Buffer, "4G,") && BlackBoxBuffer.BufferIndex > 10)
    {
        /* Check CRC: ,CRC=12345# */
        char *crc = strstr((char *)BlackBoxBuffer.Buffer, "CRC=");
        if (crc)
        {
            esp_die_cnt = 0;
            uint16_t crc16 = GetNumberFromString(4, crc);

            /* Tinh CRC thuc cua chuoi: Tru 10 ki tu cuoi CRC=12345# */
            uint16_t crcCal = CalculateCRC16(BlackBoxBuffer.Buffer, BlackBoxBuffer.BufferIndex - 10);

            if (crc16 != crcCal)
            {
                DEBUG_INFO("\rCRC failed: %u - %u", crc16, crcCal);
                return;
            }
        }
        else
            return;

        /* So config can luu */
        uint8_t configNum = 0;

        /* Split các truong du lieu */
        char *mToken = strtok((char *)BlackBoxBuffer.Buffer, ",");

        /** Duyet cac truong */
        while (mToken != NULL)
        {
            //			DEBUG_INFO("\rToken: %s", mToken);

            /** Lệnh reset */
            if (strstr(mToken, "RESET=1"))
            {
                NVIC_SystemReset();
            }

            /** Check Master Mode */
            if (strstr(mToken, "MODE=FM"))
            {
                xSystem.Parameter.Mode = MODE_FM_REMOTE;
                goto NEXT_TOKEN;
            }
            if (strstr(mToken, "MODE=INTERNET"))
            {
                xSystem.Parameter.Mode = MODE_INTERNET;
                goto NEXT_TOKEN;
            }
            if (strstr(mToken, "MODE=MIC"))
            {
                xSystem.Parameter.Mode = MODE_MIC;
                goto NEXT_TOKEN;
            }
            if (strstr(mToken, "MODE=NONE"))
            { /* Khong cho phep hoat dong */
                if (xSystem.Parameter.Mode != MODE_NONE)
                {
                    xSystem.Parameter.Mode = MODE_NONE;
                    configNum++;
                }
                goto NEXT_TOKEN;
            }
            if (strstr(mToken, "MODE=IDLE"))
            {
                xSystem.Parameter.Mode = MODE_IDLE;
                goto NEXT_TOKEN;
            }

            /** Check tan so cai dat: FREQ1,...,FREQ5 */
            if (strstr(mToken, "FREQ1="))
            {
                uint16_t freq = GetNumberFromString(6, mToken);
                if (freq != xSystem.Parameter.Frequency[0])
                {
                    xSystem.Parameter.Frequency[0] = freq;
                    configNum++;
                }
                goto NEXT_TOKEN;
            }
            if (strstr(mToken, "FREQ2="))
            {
                uint16_t freq = GetNumberFromString(6, mToken);
                if (freq != xSystem.Parameter.Frequency[1])
                {
                    xSystem.Parameter.Frequency[1] = freq;
                    configNum++;
                }
                goto NEXT_TOKEN;
            }
            if (strstr(mToken, "FREQ3="))
            {
                uint16_t freq = GetNumberFromString(6, mToken);
                if (freq != xSystem.Parameter.Frequency[2])
                {
                    xSystem.Parameter.Frequency[2] = freq;
                    configNum++;
                }
                goto NEXT_TOKEN;
            }

            /** Check tan so lam viec: FREQ */
            if (strstr(mToken, "FREQ_RUN="))
            {
                uint16_t Freq = GetNumberFromString(9, mToken);
                uint8_t isFreqValid = isValidFreq(Freq);
                if (isFreqValid != 0xFF && Freq != xSystem.Parameter.FreqRun)
                {
                    xSystem.Parameter.FreqRun = Freq;
                    xSystem.Parameter.FreqIndex = isFreqValid;

                    /* Dieu khien module thu tan so moi */
                    if (xSystem.Parameter.Mode == MODE_FM_REMOTE)
                    {
                        /* Setup Freq & volume */
                        KT093x_User_FMTune(xSystem.Parameter.FreqRun); //MHz x100
                        FM_SetVolume(xSystem.Parameter.RemoteVolume);

                        /* Hien thi tan so dang phat */
                        Led7_DispFrequency(Freq);
                    }
                }
                goto NEXT_TOKEN;
            }

            /** Check Volume cai dat cho FM */
            if (strstr(mToken, "VOL="))
            {
                uint8_t volume = GetNumberFromString(4, mToken);
                if (xSystem.Parameter.RemoteVolume != volume)
                {
                    xSystem.Parameter.RemoteVolume = volume;

                    /* Thiet lap volume cho module FM */
                    FM_SetVolume(xSystem.Parameter.RemoteVolume);
                }
                goto NEXT_TOKEN;
            }

            /** Điều khiển local mode về IDLE ngay lập tức */
            if (strstr(mToken, "LOCAL_MODE=IDLE"))
            {
                xSystem.Parameter.ModeLocal = MODE_IDLE;
                goto NEXT_TOKEN;
            }

            /** Lệnh Read/Write thanh ghi KT0935 : KT_WRITE<xxx>=yy */
            if (strstr(mToken, "KT_WRITE"))
            {
                uint8_t reg = GetNumberFromString(8, mToken);
                uint8_t value = GetNumberFromString(12, mToken);

                //Add to write list
                if (KT_Write_List.total == 0)
                {
                    KT_Write_List.List[0].reg = reg;
                    KT_Write_List.List[0].value = value;
                    KT_Write_List.List[0].isWrite = 1;
                    KT_Write_List.total++;
                }
                else
                {
                    //Tìm vị trí trống
                    for (uint8_t i = 0; i < KT_REG_LIST_MAX; i++)
                    {
                        if (KT_Write_List.List[i].isWrite == 0)
                        {
                            KT_Write_List.List[i].reg = reg;
                            KT_Write_List.List[i].value = value;
                            KT_Write_List.List[i].isWrite = 1;
                            KT_Write_List.total++;
                            break;
                        }
                    }
                }
                goto NEXT_TOKEN;
            }

            if (strstr(mToken, "KT_READ"))
            {
                uint8_t reg = GetNumberFromString(7, mToken);

                //Add to write list
                if (KT_Read_List.total == 0)
                {
                    KT_Read_List.List[0].reg = reg;
                    KT_Read_List.List[0].value = 0;
                    KT_Read_List.List[0].isRead = 1;
                    KT_Read_List.List[0].isReadDone = 0;
                    KT_Read_List.total++;
                }
                else
                {
                    //Tìm vị trí trống
                    for (uint8_t i = 0; i < KT_REG_LIST_MAX; i++)
                    {
                        if (KT_Read_List.List[i].isRead == 0 && KT_Read_List.List[i].reg == 0 &&
                            KT_Read_List.List[i].value == 0)
                        {
                            KT_Read_List.List[i].reg = reg;
                            KT_Read_List.List[i].value = 0;
                            KT_Read_List.List[i].isRead = 1;
                            KT_Read_List.List[i].isReadDone = 0;
                            KT_Read_List.total++;
                            break;
                        }
                    }
                }
                goto NEXT_TOKEN;
            }

            /** Lệnh tắt màn hình LED sau N giây không sử dụng */
            /** Điều khiển local mode về IDLE ngay lập tức */
            if (strstr(mToken, "DISP_DELAY="))
            {
                uint16_t delay = GetNumberFromString(11, mToken);
                if (xSystem.Parameter.DispDelay != delay)
                {
                    xSystem.Parameter.DispDelay = delay;
                    configNum++;
                }
                goto NEXT_TOKEN;
            }
            
            /* Next */
        NEXT_TOKEN:
            mToken = strtok(NULL, ",");
        }

        /** Luu cau hinh */
        if (configNum)
        {
            if (xSystem.Parameter.enter_test_mode == 0)
            {
                InternalFlash_WriteConfig();
            }
        }
    }

    //	uint8_t Index = 0;
    //	uint8_t SoLanXuatHien = 0;
    //	uint16_t ViTriXuatHien[5] = {0};
    //	uint8_t MessageLength = 0;

    //	//Xac dinh vi tri cac ban tin
    //	for(Index = 0; Index < BlackBoxBuffer.BufferIndex; Index++)
    //	{
    //		if(BlackBoxBuffer.Buffer[Index] == '$' && BlackBoxBuffer.Buffer[Index + 1] == 'B' &&
    //			 BlackBoxBuffer.Buffer[Index + 2] == 'A' && BlackBoxBuffer.Buffer[Index + 3] == 'P' &&
    //			 BlackBoxBuffer.Buffer[Index + 4] == 'C')
    //		{
    //			ViTriXuatHien[SoLanXuatHien] = Index;
    //			SoLanXuatHien++;
    //			ViTriXuatHien[SoLanXuatHien] = 0;
    //		}
    //	}
    //
    //	if(SoLanXuatHien == 0) return;
    //
    //	//Xu ly ban tin Blackbox (tach ban tin trong truong hop bi dinh lien)
    //	for(Index = 0; Index < SoLanXuatHien; Index++)
    //	{
    //		if(ViTriXuatHien[Index + 1] > ViTriXuatHien[Index])
    //		{
    //			MessageLength = ViTriXuatHien[Index + 1] - ViTriXuatHien[Index];
    //			memcpy(BanTinBLB, &BlackBoxBuffer.Buffer[ViTriXuatHien[Index]], MessageLength);
    //		}
    //		else
    //		{
    //			MessageLength = BlackBoxBuffer.BufferIndex - ViTriXuatHien[Index];
    //			memcpy(BanTinBLB, &BlackBoxBuffer.Buffer[ViTriXuatHien[Index]], MessageLength);
    //		}
    //		XuLyBanTin(BanTinBLB, MessageLength);
    //		memset(BanTinBLB, 0, MessageLength);
    //	}
}

#if 0
static void XuLyBanTin(uint8_t *BanTinBLB, uint8_t Length)
{
	uint8_t i;
	if(xSystem.DebugMode)
	{
		DEBUG_INFO("\rRECV:%d", Length);
		for(i = 0; i < Length; i++)
			DEBUG_INFO("%X ", BanTinBLB[i]);
	}
	
	i = BLK_ValidMessage(BanTinBLB, Length);
	if( i != 0) 
	{
		DEBUG_INFO("\rInvalid: %d", i);
		return;	
	}

	switch(BanTinBLB[5])	//Ma ban tin
	{
		case BT_YEU_CAU_DU_LIEU:
			XuLyBanTinYeuCauDuLieu(BanTinBLB);
			break;
		case BT_BAT_DAU_UPDATE:
			XuLyBanTinBatDauUpdate(BanTinBLB);
			break;
		case BT_DU_LIEU_UPDATE:
			XuLyBanTinDuLieuUpdate(BanTinBLB);
			break;
		case BT_KET_THUC_UPDATE:
			XuLyBanTinKetThucUpdate(BanTinBLB);
			break;
		case BT_CAU_HINH:
			XuLyBanTinCauHinh(BanTinBLB);
			break;
		default:
			break;
	}
		
	//TEST lay du lieu tu USB-RS485 PC
//	if(strstr((char *)BanTinBLB, "GETDATA:")) 
//	{
//		uint8_t DiaChi = GetNumberFromString(8, (char *)BanTinBLB);
//		if(DiaChi == xSystem.HardwareInfo.DiaChi)
//			GuiBanTinTrangThaiGhe();
//	}
}

/*****************************************************************************/
/**
 * @brief	:  Xu ly ban tin yeu cau du lieu tu hop den
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	03/03/2016
 * @version	:
 * @reviewer:	
 */
static void XuLyBanTinYeuCauDuLieu(uint8_t *Buffer)
{
	uint8_t Index = 14;
	uint8_t Valid;
	
	//Kiem tra dia chi Slave
	if(Buffer[Index] != xSystem.HardwareInfo.DiaChi) 
	{
		return;
	}
	
	//Kiem tra ma bao mat
	Valid = BLK_ValidSecureCode(Buffer);
	if(Valid != 0) 
	{
		DEBUG_INFO("Valid code fail: %d", Valid);
		return;
	}
	
	//Xu ly ban tin
	switch(Buffer[Index+1])	//Loai du lieu yeu cau
	{
		case BT_THONG_SO:
			GuiBanTinThongSo();
			break;
		case BT_TRANG_THAI_GHE:
			GuiBanTinTrangThaiGhe();
			break;
		default:
			DEBUG_INFO("\rLoai du lieu yeu cau sai: %d", Buffer[Index+1]);
			break;
	}
}


/*****************************************************************************/
/**
 * @brief	:  Xu ly ban tin cau hinh tu hop den
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	03/03/2016
 * @version	:
 * @reviewer:	
 */
static void XuLyBanTinCauHinh(uint8_t *Buffer)
{
	Int_t Len;
	uint8_t MaLenh = 0;
	uint8_t tmpBuff[20];
	
	//Kiem tra ma bao mat
	if(BLK_ValidSecureCode(Buffer) != 0) return;
	
	Len.bytes[1] = Buffer[12];
	Len.bytes[0] = Buffer[13];
	
	memcpy(tmpBuff, &Buffer[14], Len.value);

	if(strstr((char*)tmpBuff, "SCFG"))
	{
		MaLenh = GetNumberFromString(5, (char*)tmpBuff);
		
		if(MaLenh == 1 && strstr((char*)tmpBuff, "OK"))
		{
			//Lenh reset mach
			SystemReset(7);
		}
		if(MaLenh == 2 && strstr((char*)tmpBuff, "OK"))
		{
			//Test LED trong 3s
			BlackBoxManager.TestLEDTime = 30;
		}
	}
	if(strstr((char*)tmpBuff, "GCFG"))
	{}
}
	

/*****************************************************************************/
/**
 * @brief	:  Xu ly ban tin Update FW tu hop den
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	03/03/2016
 * @version	:
 * @reviewer:	
 */
void XuLyBanTinBatDauUpdate(uint8_t *Buffer)
{
	uint8_t Index = 14, Result, i;
	
	//Kiem tra ma bao mat
	if(BLK_ValidSecureCode(Buffer) != 0) return;
	
	FWUpdate.FWLength.bytes[3] = Buffer[Index++];
	FWUpdate.FWLength.bytes[2] = Buffer[Index++];
	FWUpdate.FWLength.bytes[1] = Buffer[Index++];
	FWUpdate.FWLength.bytes[0] = Buffer[Index++];

	FWUpdate.FWCRC.bytes[3] = Buffer[Index++];
	FWUpdate.FWCRC.bytes[2] = Buffer[Index++];
	FWUpdate.FWCRC.bytes[1] = Buffer[Index++];
	FWUpdate.FWCRC.bytes[0] = Buffer[Index++];
	
	//Xoa vung nho luu FW data
	for(i = 0; i < 3; i++)
	{
		Result = InternalFlash_Prepare(FW_UPDATE_ADDR, FWUpdate.FWLength.value);
		if(Result == 0) break;
	}
	
	if(Result != 0)	//Khong erase duoc vung nho
	{
		DEBUG_INFO("\r[UDFW] Erase Flash [Fail]");
		return;
	}
	
	//Ghi cac gia tri thong so update
	FLASH_Unlock();
	FLASH_ProgramWord(FWLength_InFlash, FWUpdate.FWLength.value);
	FLASH_ProgramWord(FWCRC_InFlash, FWUpdate.FWCRC.value);
	FLASH_Lock();
	
	DEBUG_INFO("\r[UDFW] Memory prepare [Done]");
	FWUpdate.PrepareDone = 0xAA;
	FWUpdate.DataWritten = 0;
}
	
void XuLyBanTinDuLieuUpdate(uint8_t *Buffer)
{
	Int_t Leng;
	Long_t AddID, TotalLeng;
	uint8_t Index = 14;
	
	if(FWUpdate.PrepareDone != 0xAA) return;
	
	FWUpdate.Type = Buffer[Index++];
	AddID.bytes[3] = Buffer[Index++];
	AddID.bytes[2] = Buffer[Index++];
	AddID.bytes[1] = Buffer[Index++];
	AddID.bytes[0] = Buffer[Index++];
	TotalLeng.bytes[3] = Buffer[Index++];
	TotalLeng.bytes[2] = Buffer[Index++];
	TotalLeng.bytes[1] = Buffer[Index++];
	TotalLeng.bytes[0] = Buffer[Index++];
	
	//Lay du lieu FW va ghi vao Flash
	Leng.bytes[1] = Buffer[12];
	Leng.bytes[0] = Buffer[13];
	Leng.value -= 9;	//Bo 9 bytes
	
	Buffer[Index + Leng.value] = 0;
	Buffer[Index + Leng.value + 1] = 0;
	if(InternalFlash_WriteBytes(FW_UPDATE_DATA_ADDR + FWUpdate.DataWritten, &Buffer[Index], Leng.value))
	{
		DEBUG_INFO("\r[UDFW] Write to Flash [Fail], MessageID: %d", AddID.value);
		FWUpdate.PrepareDone = 0;
		FWUpdate.DataWritten = 0;
		return;
	}
	FWUpdate.DataWritten += Leng.value;
	DEBUG_INFO("\r[UDFW] ID: %d, Len: %d, Total: %d, Written: %d", AddID.value, Leng.value, TotalLeng.value, FWUpdate.DataWritten);
}
	

void XuLyBanTinKetThucUpdate(uint8_t *Buffer)
{
	if(FWUpdate.PrepareDone != 0xAA) return;

	if(FWUpdate.DataWritten != FWUpdate.FWLength.value)
	{
		DEBUG_INFO("\r[UDFW] Data written: %d != %d FWLength. Update discard!", FWUpdate.DataWritten, FWUpdate.FWLength.value);
		FWUpdate.PrepareDone = 0;
		FWUpdate.DataWritten = 0;
		return;
	}
	
	//Neu du lieu ghi thanh cong -> cho phep update
	if(FWUpdate.Type == UDFW_MAIN)
	{
		FLASH_Unlock();
		FLASH_ProgramWord(NewFWFlag_InFlash, NewFWFlagValue);
		FLASH_Lock();
		DEBUG_INFO("\rUDFW: !!!Reset to update FW!!!");
		SystemReset(8);
	}
	else if(FWUpdate.Type == UDFW_BOOTLOADER)
	{
		DEBUG_INFO("\rUDFW: Dang update FW Bootloader...");
		
		if(CheckCRCInternalFlash(FWUpdate.FWLength.value) == 0)
		{
			DEBUG_INFO("\rUDFW: Check CRC [FAIL]. Huy Update!");
			return;
		}
	
		if(InternalFlash_Prepare(BOOTLOADER_FW_ADDR, FWUpdate.FWLength.value) == 1)
		{
			DEBUG_INFO("\rUDFW: Khong xoa duoc Flash. Huy update!");
			return;
		}
		DEBUG_INFO("\rUDFW: Xoa Flash [OK]. Copy du lieu...");
		
		if(CopyDataInternalFlash(BOOTLOADER_FW_ADDR, FW_UPDATE_DATA_ADDR, FWUpdate.FWLength.value) == 0)
		{
			DEBUG_INFO("\rUDFW: Khong copy duoc du lieu. Update [FAIL]");	//Co the cho update lai khi reset!!
			return;
		}
		DEBUG_INFO("\rUDFW: Update thanh cong! Reset mach!!");
		SystemReset(10);
	}
}


/*****************************************************************************/
/**
 * @brief	:  Gui ban tin Thong so len hop den
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	03/03/2016
 * @version	:
 * @reviewer:	
 */
static void GuiBanTinThongSo(void)
{
	uint8_t Index = 0;
	Int_t Checksum;
	uint8_t i;
	
	//Header
	Index = BAPC_AddHeader();
	
	//Loai ban tin
	BLB_TxBuffer[Index++] = BT_THONG_SO;
	
	//Ma Bao mat
	Index = BAPC_AddSecureCode(Index);
	
	//Do dai
	BLB_TxBuffer[Index++] = 0;
	BLB_TxBuffer[Index++] = 0;	
	
	//Noi dung
	BLB_TxBuffer[Index++] = xSystem.HardwareInfo.DiaChi;	//Device ID
	BLB_TxBuffer[Index++] = FIRMWARE_VERSION_CODE;	//FW code
	BLB_TxBuffer[Index++] = xSystem.HardwareInfo.HardwareVersion;	//HW version
	BLB_TxBuffer[Index++] = PROTOCOL_VERSION_CODE;	//Protocol code
	memcpy(&BLB_TxBuffer[Index], xSystem.HardwareInfo.UID, 12);
	Index += 12;
	
	//Tinh lai do dai truong noi dung
	BLB_TxBuffer[12] = (Index - 14)>>8;
	BLB_TxBuffer[13] = (Index - 14) & 0xFF;	

	//Checksum
	Checksum.value = CalculateCheckSum(BLB_TxBuffer, 0, Index);
	BLB_TxBuffer[Index++] = Checksum.bytes[1];
	BLB_TxBuffer[Index++] = Checksum.bytes[0];
	
	//Tail
	BLB_TxBuffer[Index++] = '#';
	
	//Gui ban tin
	DATAMODE(OUTPUT);
	xSystem.Driver.UART->Putb(RS485_UART, BLB_TxBuffer, Index);
	
	if(xSystem.HardwareInfo.HardwareVersion == __XEKHACH_VERSION__)
		DataOutTimeOut = 5;	//10ms

	//TEST
	if(xSystem.DebugMode)
	{
		DEBUG_INFO("\rSENS: Para,%d", Index);
		for(i=0; i <Index; i++)
			DEBUG_INFO("%X ", BLB_TxBuffer[i]);
	}
}

/*****************************************************************************/
/**
 * @brief	:  Gui ban tin Trang thai ghe len hop den
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	03/03/2016
 * @version	:
 * @reviewer:	
 */
void GuiBanTinTrangThaiGhe(void)
{
	uint8_t Index = 0, i;
	Int_t Checksum;
	
	ResetWatchdog();
	
	//Header
	Index = BAPC_AddHeader();
	
	//Loai ban tin
	BLB_TxBuffer[Index++] = BT_TRANG_THAI_GHE;
	
	//Ma bao mat - 6
	Index = BAPC_AddSecureCode(Index);

	//Do dai
	BLB_TxBuffer[Index++] = 0;
	BLB_TxBuffer[Index++] = 0;	
	
	//Noi dung (Dia chi + trang thai cac ghe)
	BLB_TxBuffer[Index++] = xSystem.HardwareInfo.DiaChi;	
	for(i = 0; i < 8; i++)
	{
		BLB_TxBuffer[Index + i] = Measure.TrangThaiGhe[i];
	}
	Index += 8;
	
	//Tinh lai do dai truong noi dung
	BLB_TxBuffer[12] = (Index - 14)>>8;
	BLB_TxBuffer[13] = (Index - 14) & 0xFF;
	
	//Checksum
	Checksum.value = CalculateCheckSum(BLB_TxBuffer, 0, Index);
	BLB_TxBuffer[Index++] = Checksum.bytes[1];
	BLB_TxBuffer[Index++] = Checksum.bytes[0];
	
	//Tail
	BLB_TxBuffer[Index++] = '#';
	
	//Gui ban tin
	DATAMODE(OUTPUT);
	xSystem.Driver.UART->Putb(RS485_UART, BLB_TxBuffer, Index);
	
	if(xSystem.HardwareInfo.HardwareVersion == __XEKHACH_VERSION__)
		DataOutTimeOut = 5;	//10ms
	
	//TEST
	if(xSystem.DebugMode)
	{
		DEBUG_INFO("\rSENS: State,%d", Index);
		for(i=0; i <Index; i++)
			DEBUG_INFO("%X ", BLB_TxBuffer[i]);
	}
}
#endif

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	Phinht
 * @created	:	03/03/2016
 * @version	:
 * @reviewer:	
 */
static void BlackBoxAddData(uint8_t Data)
{
    BlackBoxBuffer.Buffer[BlackBoxBuffer.BufferIndex++] = Data;
    BlackBoxBuffer.Buffer[BlackBoxBuffer.BufferIndex] = 0;
    BlackBoxBuffer.State = 7;
}

/*******************************************************************************
 * Function Name  	: USART2_IRQHandler 
 * Return         	: None
 * Parameters 		: None
 * Created by		: Phinht
 * Date created	: 02/03/2016
 * Description		: This function handles USART1 global interrupt request. 
 * Notes			: 
 *******************************************************************************/
extern size_t driver_uart_get_new_data_to_send(uint8_t *c);
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t rx = USART_ReceiveData(USART1);
        BlackBoxAddData(rx);
    }
    
    uint32_t tmp = USART_CTL0(USART1);
        
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_TBE)
        && (tmp & USART_CTL0_TBEIE))
    {
        /* transmit data */
        uint8_t c;
        if (driver_uart_get_new_data_to_send(&c))
        {
            usart_data_transmit(USART1, c);
        }
        else
        {
            usart_interrupt_disable(USART1, USART_INT_TBE);
        }
    } 

//    if (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_ERR_ORERR))
//    {
//        usart_data_receive(USART1);
//		usart_flag_clear(USART1, USART_FLAG_ORERR);
//    } 	
//    
//    if (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_ERR_FERR))
//    {
//        usart_data_receive(USART1);
//		usart_flag_clear(USART1, USART_FLAG_FERR);
//    }     
    
    if (RESET != usart_flag_get(USART1, USART_FLAG_ORERR))
    {
//        usart_data_receive(USART0);
		usart_flag_clear(USART1, USART_FLAG_ORERR);
    } 	
    
    if (RESET != usart_flag_get(USART1, USART_FLAG_FERR))
    {
        usart_data_receive(USART1);
		usart_flag_clear(USART1, USART_FLAG_FERR);
    }    

    if (RESET != usart_flag_get(USART1, USART_FLAG_NERR))
    {
        usart_data_receive(USART1);
		usart_flag_clear(USART1, USART_FLAG_NERR);
    }
    
}



/********************************* END OF FILE *******************************/
