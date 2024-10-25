#ifndef __DATA_DEFINE_H__
#define __DATA_DEFINE_H__

// #include "stm32f0xx.h"
#include "gd32e23x.h"
#include "gd32e23x_i2c.h"

#include "Hardware.h"
#include "DriverUART.h"
	
#include "SEGGER_RTT.h"    
#include "app_debug.h"

#define	SMALL_BUFFER_SIZE	1024

#define	TACT_START	1
#define	TACT_PAUSE	2

#define	STATUS_OK    1
#define	STATUS_FAIL  0

#define	PTT_PRESSED	0
#define	PTT_RELEASED	1

typedef enum
{
	LED_BRIGHTNESS_100 = 1,
	LED_BRIGHTNESS_80,
	LED_BRIGHTNESS_60,
	LED_BRIGHTNESS_40,
	LED_BRIGHTNESS_20
} LEDBrightness_t;

typedef enum
{
	MODE_IDLE = 0,
	MODE_NONE = 1,
	MODE_INTERNET,
	MODE_FM_REMOTE,
	MODE_FM_LOCAL,
	MODE_MIC,
	MODE_LINE_IN
} OperationMode_t;

typedef struct
{
	uint8_t Buffer[SMALL_BUFFER_SIZE];
	uint16_t BufferIndex;
	uint8_t State;
} SmallBuffer_t;

typedef union
{
	float value;
	uint8_t bytes[4];
} Float_t;

typedef union
{
	uint32_t value;
	uint8_t bytes[4];
} Long_t;

typedef union
{
	uint16_t value;
	uint8_t bytes[2];
} Int_t;

typedef struct
{
	Float_t KinhDo;
	Float_t ViDo;
} PointStruct_t;

typedef struct
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
} DateTime_t;

typedef struct _DriverUART_t {	
	void		(*Init)		(void* USARTx, uint32_t BaudRate);
	void 		(*Putc)		(void* USARTx, uint8_t c) ;	
	void	 	(*Puts)		(void* USARTx, const char *str);
	void 		(*Putb)		(void* USARTx, uint8_t *Data, uint16_t Length);	
	void	 	(*Printf)	(void* USARTx,const char* str, ...);		
} const DriverUART_t;

typedef struct _Debug_t {	
	void		(*Init) 	(void);
	void		(*Tick) 	(void);
	void		(*AddData) 	(uint8_t Data);
}const Debug_t;

typedef struct _Driver_InternalFlash_t {
	void 		(*Init)         (void);
	uint8_t     (*Prepare) 		(uint32_t Address, uint16_t Length);
	uint8_t		(*WriteBytes)   (uint32_t Address, uint8_t *Buffer, uint16_t Length);
	void        (*ReadBytes)    (uint32_t Address, uint8_t *Buffer, uint32_t Length);
	uint8_t     (*CopyData)     (uint32_t SrcAddress, uint32_t DstAddress, uint32_t Length);
	uint8_t     (*WriteByte)    (uint32_t Address, uint8_t Data);
	uint8_t     (*WriteInt)     (uint32_t Address, uint16_t Data);
	uint8_t     (*WriteLong)    (uint32_t Address, uint32_t Data);
} const Driver_InternalFlash_t;

typedef struct _Driver_RTC_t
{
	void (*Init) (void);
	void (*SetDateTime) (DateTime_t Input, int32_t GMT_Adjust);
	DateTime_t(*GetDateTime) (void);
	void (*UpdateTimeFromServer) (uint8_t *Buffer);
	uint32_t(*GetCounter) (void);
	void (*Tick) (void);
	uint8_t(*ValidDateTime) (DateTime_t ThoiGian);
} const Driver_RTC_t;
	
typedef struct
{
	DriverUART_t *UART;
	Driver_InternalFlash_t *InternalFlash;
}Driver_t;

typedef struct
{
	uint8_t DiaChi;
	uint8_t HardwareVersion;
	uint32_t ResetReasion;
	uint8_t SelfResetReasion;
	uint32_t SoLanReset;	
	DateTime_t ThoiGianXuatXuong;
	uint8_t UID[15];
	char UniqueID[35];
} HardwareInfo_t;

typedef struct {
	uint8_t State;
	uint8_t SNR;
	uint8_t Volume;
	uint32_t Frequency;
	uint8_t RSSI;
	int dBm;
} FM_Status_t;

typedef struct {
	uint8_t SystemInitDone;
	uint8_t TestInternalWDT;
	int32_t TactTimeCounter;
	int32_t DelayTimeSum;
	uint32_t VehicleCountInDay;
	
	uint8_t AudioLevel;
	uint16_t DelaySleepLED7;
	uint8_t isSetupState;

	FM_Status_t FM;
	
	//Trạng thái điều khiển ESP_EN
	uint8_t isESP32Enable;
} Status_t;

typedef struct {
	uint8_t Mode;		/* Internet - FM - MIC -> chon qua lai */
	uint8_t RemoteVolume;	/* Volume dieu khien tu mach main */
	uint8_t LocalVolume;		/* Volume dieu khien bang num xoay */

	uint8_t FreqIndex;
	uint16_t Frequency[5];	/* Tan so FM duoc cai dat */
	uint16_t DispDelay;
	
	/* Cac thong so dang hoat dong */
	uint16_t FreqRun;	/* Tan so dang thu */
	uint8_t ModeLocal;	/* Che do chon tay duoi local */
		
	uint8_t LEDBrightness;
    uint8_t enter_test_mode;
} Parameters_t;

typedef struct
{
	Driver_t Driver;
	Driver_RTC_t *Rtc;
	Debug_t	*Debug;
	Status_t Status;
	Parameters_t Parameter;
} System_t;

#endif // __DATA_DEFINE_H__

