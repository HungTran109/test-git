/************************************************************************************/
//	公司名称：			Beijing KT Micro, Ltd.
//
//	模 块 名：			KT093X驱动模块头文件
//	版    本：V1.4.6
//	创 建 人：ZhouDongfeng				时间：2013-05-30
//	修 改 人：Kanghekai					时间：2013-12-12
//	功能描述：
//	其他说明： Revision History
//	Version		Date			Description
//	V1.0.0		2013-06-03		Initial draft version
//	V1.1		2013-06-18		Add "FM_DELTN"
//	V1.3		2013-07-19		OPEN "BLEND of SNR"
//	V1.4		2013-08-27		Delete useless definition
//	V1.4.1		2013-08-29		1、调节音量使FM75K的音量与AM80%的音量一致
//								2、调节了tune亮灯FM在14dBuVemf亮，9dBuVemf灭
//								3、调节了tune亮灯AM在71dBuVemf亮，68dBuVemf灭
//								4、调节了AM音量曲线
//								5、调节了FM音量曲线
//								6、调节了FM BLEND参数
//	V1.4.2		2013-09-23		修正了AM时DELTN错误的BUG
//	V1.4.3		2013-09-30		修正了FM时DELTN的值
//								修改了FM softmute配置（针对sony输入输出特性要求）
//	V1.4.4		2013-11-05		修正了AM时DELTN错误的BUG
//	V1.4.5		2013-11-27		整理KT_AMFM_Init，增加宏定义
//  							改变变量类型定义方法
//								缩短AM有效台判断的延时时间
//	V1.4.6		2013-12-12		优化AM搜台程序
/************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
//#include "interface.h"

#define FM_DELTN

//#ifdef FM_DELTN
//	void KT_FM_Deltn(uint16_t Deltn);
//#endif

#define KT0933

#ifdef KT0933
	#define KTAMw_address 0x6A
	#define KTAMr_address 0x6B
#endif

//#define GOODCHANNEL

#define LOWFM_MODE
//#define CHN_BAND
//#define EUP_BAND
//#define USA_BAND
//#define JPN_BAND

#define X32P768K					//32.768KHz crystal
//#define R32P768K					//32.768KHz reference clock
//#define X38K						//38KHz crystal
//#define R38K						//38KHz reference clock
//#define R7P6M						//7.6MHz reference clock
//#define R12M						//12MHz reference clock
//#define R13M						//13MHz reference clock
//#define R24M						//24MHz reference clock
//#define R26M						//26MHz reference clock

#ifdef X32P768K
	#define XTAL_FREQ			32768
	#define DIVIDERP			1
	#define DIVIDERN			668
	#define SYS_CFGOK			1
	#define	RCLK_EN				0
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif
	
#ifdef R32P768K
	#define XTAL_FREQ			32768
	#define DIVIDERP			1
	#define DIVIDERN			668
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef X38K
	#define XTAL_FREQ			38000
	#define DIVIDERP			1
	#define DIVIDERN			576
	#define SYS_CFGOK			1
	#define	RCLK_EN				0
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef R38K
	#define XTAL_FREQ			38000
	#define DIVIDERP			1
	#define DIVIDERN			576
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef R7P6M
	#define XTAL_FREQ			7600000
	#define DIVIDERP			200
	#define DIVIDERN			576
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef R12M
	#define XTAL_FREQ			12000000
	#define DIVIDERP			375
	#define DIVIDERN			684
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef R12P288M
	#define XTAL_FREQ			12288000
	#define DIVIDERP			384
	#define DIVIDERN			684
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef R13M
	#define XTAL_FREQ			13000000
	#define DIVIDERP			411
	#define DIVIDERN			692
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef R24M
	#define XTAL_FREQ			24000000
	#define DIVIDERP			750
	#define DIVIDERN			684
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif

#ifdef R26M
	#define XTAL_FREQ			26000000
	#define DIVIDERP			822
	#define DIVIDERN			692
	#define SYS_CFGOK			1
	#define	RCLK_EN				1
	
	#define FPFD				(XTAL_FREQ / DIVIDERP * 16)
	#define DIVIDERP_H			(DIVIDERP >> 8)
	#define DIVIDERP_L			(DIVIDERP % 256)
	#define DIVIDERN_H			(DIVIDERN >> 8)
	#define DIVIDERN_L			(DIVIDERN % 256)
	#define FPFD_H				(FPFD >> 16)
	#define FPFD_M				((FPFD >> 8) & 0x00FF)
	#define FPFD_L				(FPFD % 65536)
#endif


#define FM_50KHz_STEP			5
#define FM_100KHz_STEP			10
#define FM_200KHz_STEP			20
#define FM_1MHz_STEP			100
#define AM_1KHz_STEP			1
#define AM_9KHz_STEP			9
#define AM_10KHz_STEP			10
#define AM_100KHz_STEP			100


#ifdef LOWFM_MODE
	#define DE					0		//0: 75us,		1: 50us

	#define FM_BAND_BOTTOM		8750
	#define FM_BAND_TOP			10800
	#define FM_SEEK_DOWN_LIMIT	8750
	#define FM_SEEK_UP_LIMIT	10800

	#define MW_SEEK_DOWN_LIMIT	504
	#define MW_SEEK_UP_LIMIT	1710
	#define MW_BAND_BOTTOM		504
	#define MW_BAND_TOP			1710

	#define FM_MIN_STEP				FM_100KHz_STEP
	#define FM_MAX_STEP				FM_100KHz_STEP
	#define FM_SEEK_STEP			FM_100KHz_STEP
	#define FM_VALIDSTATION_STEP	FM_100KHz_STEP

	#define MW_MIN_STEP				AM_1KHz_STEP
	#define MW_MAX_STEP				AM_9KHz_STEP
	#define MW_SEEK_STEP			AM_9KHz_STEP
	#define MW_VALIDSTATION_STEP	AM_1KHz_STEP
#endif

#ifdef CHN_BAND
	#define DE					1		//0: 75us,		1: 50us

	#define FM_BAND_BOTTOM		8750
	#define FM_BAND_TOP			10800
	#define FM_SEEK_DOWN_LIMIT	8750
	#define FM_SEEK_UP_LIMIT	10800

	#define MW_SEEK_DOWN_LIMIT	504
	#define MW_SEEK_UP_LIMIT	1710
	#define MW_BAND_BOTTOM		504
	#define MW_BAND_TOP			1710

	#define FM_MIN_STEP				FM_100KHz_STEP
	#define FM_MAX_STEP				FM_1MHz_STEP
	#define FM_SEEK_STEP			FM_100KHz_STEP
	#define FM_VALIDSTATION_STEP	FM_100KHz_STEP

	#define MW_MIN_STEP				AM_1KHz_STEP
	#define MW_MAX_STEP				AM_9KHz_STEP
	#define MW_SEEK_STEP			AM_9KHz_STEP
	#define MW_VALIDSTATION_STEP	AM_1KHz_STEP
#endif

#ifdef EUP_BAND
	#define DE					1		//0: 75us,		1: 50us

	#define FM_BAND_BOTTOM		8750
	#define FM_BAND_TOP			10800
	#define FM_SEEK_DOWN_LIMIT	8750
	#define FM_SEEK_UP_LIMIT	10800

	#define MW_SEEK_DOWN_LIMIT	504
	#define MW_SEEK_UP_LIMIT	1710
	#define MW_BAND_BOTTOM		504
	#define MW_BAND_TOP			1710

	#define FM_MIN_STEP				FM_50KHz_STEP
	#define FM_MAX_STEP				FM_1MHz_STEP
	#define FM_SEEK_STEP			FM_50KHz_STEP
	#define FM_VALIDSTATION_STEP	FM_100KHz_STEP

	#define MW_MIN_STEP				AM_1KHz_STEP
	#define MW_MAX_STEP				AM_9KHz_STEP
	#define MW_SEEK_STEP			AM_9KHz_STEP
	#define MW_VALIDSTATION_STEP	AM_1KHz_STEP
#endif

#ifdef USA_BAND
	#define DE					0		//0: 75us,		1: 50us

	#define FM_BAND_BOTTOM		8750
	#define FM_BAND_TOP			10790
	#define FM_SEEK_DOWN_LIMIT	8750
	#define FM_SEEK_UP_LIMIT	10790

	#define MW_BAND_BOTTOM		500
	#define MW_BAND_TOP			1710
	#define MW_SEEK_DOWN_LIMIT	500
	#define MW_SEEK_UP_LIMIT	1710

	#define FM_MIN_STEP				FM_200KHz_STEP
	#define FM_MAX_STEP				FM_1MHz_STEP
	#define FM_SEEK_STEP			FM_200KHz_STEP
	#define FM_VALIDSTATION_STEP	FM_100KHz_STEP

	#define MW_MIN_STEP				AM_1KHz_STEP
	#define MW_MAX_STEP				AM_10KHz_STEP
	#define MW_SEEK_STEP			AM_10KHz_STEP
	#define MW_VALIDSTATION_STEP	AM_1KHz_STEP
#endif

#ifdef JPN_BAND
	#define DE					1		//0: 75us,		1: 50us

	#define FM_BAND_BOTTOM		7600
	#define FM_BAND_TOP			9100
	#define FM_SEEK_DOWN_LIMIT	7600
	#define FM_SEEK_UP_LIMIT	9100

	#define MW_SEEK_DOWN_LIMIT	504
	#define MW_SEEK_UP_LIMIT	1710
	#define MW_BAND_BOTTOM		504
	#define MW_BAND_TOP			1710

	#define FM_MIN_STEP				FM_100KHz_STEP
	#define FM_MAX_STEP				FM_1MHz_STEP
	#define FM_SEEK_STEP			FM_100KHz_STEP
	#define FM_VALIDSTATION_STEP	FM_100KHz_STEP

	#define MW_MIN_STEP				AM_1KHz_STEP
	#define MW_MAX_STEP				AM_9KHz_STEP
	#define MW_SEEK_STEP			AM_9KHz_STEP
	#define MW_VALIDSTATION_STEP	AM_1KHz_STEP
#endif


#define	VOLUME				31

#define BLEND_START_SNR		63
#define BLEND_STOP_SNR		63
#define BLND_START_RSSI		5       //0:8, 1:10, 2:12, 3:14, 4:16, 5:18, 6:20, 7:22, 8:24, 9:26, 10:28, 11:30, 12:32, 13:34, 14:36, 15:38(unit:dbuVEMF)
#define BLND_STOP_RSSI		15     	//0:8, 1:10, 2:12, 3:14, 4:16, 5:18, 6:20, 7:22, 8:24, 9:26, 10:28, 11:30, 12:32, 13:34, 14:36, 15:38(unit:dbuVEMF)
#define BLND_MOD			0		//0: RSSI,	1: SNR
#define DBLND				0//1//0		//0: blend enable,	1: blend disable
#define MONO				0		//0: stereo,	1: force mono

#define FM_GAIN				4       //0:0dB, 1:3.5dB, 2:6dB, 3:9.5dB, 4:-2.5dB, 5:-3.66dB, 6:-6dB, 7:-8.5dB
#define AM_GAIN				5		//0:6dB, 1:3dB, 2:0dB, 3:-3dB, 4:-6dB, 5:-9dB, 6:-12dB, 7:-15dB, 8:-18dB
#define	FLT_SEL				0//1       //0:1.2k, 1:2.4k, 2:3.6k, 3:4.8k, 4:6k

//#define AM_RSSI_BIAS		0
#define AM_VOLUME			8

#define DEPOP_TC			3       //0:250ms,  1:500ms,    2:750ms,     3:1s
#define	AUDV_DCLVL			5

#define FM_AFCD				0//1//0       //0:afc enable, 1:afc disable
#define FM_TH_AFC			1//3    	//0:5k, 1:15k, 2:25k, 3:35k, 4:50k, 5:100k, 6:150k, 7:200k

#define AM_AFCD				0		//0:afc enable, 1:afc disable
#define AM_TH_AFC			0 

#define FM_AFCTH_PREV		20				//Range from 0 to 127
#define FM_AFCTH			15				//Range from 0 to 127
#define FM_AFCTH_NEXT		20				//Range from 0 to 127
#define FM_SNR_TH			0x14
#define FM_RSSI_TH			-98


#define MW_AFCTH_PREV		6				//Range from 128 to 127
#define MW_AFCTH_NEXT		6				//Range from 128 to 127
#define MW_AFCTH			4				//Range from 128 to 127
#define AM_SNR_TH			0x3A//0x28
#define AM_RSSI_TH			-85//-70//-66

#define DSMUTEL			1
#define DSMUTER			1

#define FM_DSMUTE			0
#define FM_SMUTE_START_RSSI	5		//2	 //yyq
#define FM_SMUTE_SLOPE_RSSI	3
#define FM_SMUTE_START_SNR	0x15
#define FM_SMUTE_SLOPE_SNR	2		//3  //yyq
#define FM_SMUTE_MIN_GAIN	4

#define AM_DSMUTE			0
#define AM_SMUTE_START_RSSI	0x17
#define AM_SMUTE_SLOPE_RSSI	4	
#define AM_SMUTE_START_SNR	0x30
#define AM_SMUTE_SLOPE_SNR	0		//2  //yyq
#define AM_SMUTE_MIN_GAIN	1	

#define BAND_NUM			2

enum Band_Mode	{FM_MODE,MW_MODE};

struct Str_Band
{
	enum Band_Mode	Band;						// 频段
	uint16_t			Band_Top;					//	tune台频段上门限
	uint16_t			Band_Bottom;				//	tune台频段下门限
	uint16_t			Seek_Up_Limit;				//	seek频段上门限
	uint16_t			Seek_Down_Limit;			//	seek频段下门限
	uint8_t			Min_Step;					//
	uint8_t			Max_Step;					//
	uint8_t			Seek_Step;					// SeekFromCurrentCh 函数用的频率步进
	uint8_t			ValidStation_Step;			// ValidStation 函数用的频率步进
	uint8_t			AFCTH_Prev;					//
	uint8_t			AFCTH;						//
	uint8_t			AFCTH_Next;					// 
};
  

#define INIT_FAIL_TH 10//3

#define SEEKUP 1
#define SEEKDOWN 0

uint8_t KT_AMFM_PreInit(void);
uint8_t KT_AMFM_Init(void);
uint8_t KT_AMFM_Standby(void);
uint8_t KT_AMFM_WakeUp(void);
uint8_t KT_AMFM_VolumeSet(uint8_t cVol);
uint8_t KT_AMFM_BLND_START_RSSISet(uint8_t cRSSI);
uint8_t KT_AMFM_BLND_STOP_RSSISet(uint8_t cRSSI);
uint8_t KT_AMFM_Mute(void);
uint8_t KT_AMFM_SetMode(uint8_t cAMFM_Mode);
uint8_t KT_AMFM_SetLineIn(uint8_t cLineNum);
bool KT_AMFM_SetBass(uint8_t cBass);
void KT_AMFM_Softmute_Initial(void);


uint8_t KT_AM_SetBW(uint8_t cFlt_Sel);
uint16_t KT_AM_GetCap(void);
void KT_AM_RDQ(void);


uint8_t KT_FM_Tune(uint16_t iFrequency);
uint8_t KT_AM_Tune(uint16_t iFrequency);

uint8_t KT_FM_ReadRSSI(int8_t *RSSI);
uint8_t KT_AM_ReadRSSI(int8_t *RSSI);

uint16_t KT_FM_GetFreq(void);
uint16_t KT_AM_GetFreq(void);

int16_t KT_FM_GetAFC(void);
int16_t KT_AM_GetAFC(void);

uint8_t KT_FM_GetSNR(void);
uint8_t KT_AM_GetSNR(void);

uint8_t KT_AMFM_SeekFromCurrentCh(bool bSeekDir, uint16_t *Frequency);   //     seekDir: 0-->seek down 1-->seek up
uint8_t KT_FM_ValidStation(uint16_t iFrequency);
uint8_t KT_AM_ValidStation(uint16_t iFrequency);

bool KT_AMFM_Tuning_Indicator(void);

#ifdef GOODCHANNEL
	void KT_FM_Good_Channel(void);
	void KT_AM_Good_Channel(void);
#endif

void KT_Byte_Write(uint8_t Register_Address, uint8_t Byte_Data);
uint8_t KT_Byte_Read(uint8_t Register_Address);
void KT093x_smgain_by_mcu_or_cpu();
void KT093x_smgain_by_mcu();
void KT093x_amp_auto();
