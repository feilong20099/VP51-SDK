#ifndef __MKCard_API_FUN_H__
#define __MKCard_API_FUN_H__

#include "MKCardApiDef.h"

static LPCTSTR MKApi_GetErrorDescription(MKError type)
{
	switch(type)
	{
	case MKError_No :
		return _T("No error");
		break;
	case MKError_SystemNotInit:
		return _T("System Resource Does't initialize");
		break;
	case MKError_SystemHadInitialized:
		return _T("System Resource had initialized");
		break;
	case MKError_DriverNotLoaded:
		return _T("Device driver not loaded");
		break;
	case MKError_Mem :
		return _T("Memory error");
		break;
	case MKError_Param:
		return _T("Function param error");
		break;
	case MKError_PortIsCapture:
		return _T("This port is capturing");
		break;
	case MKError_PortIsSend:
		return _T("This port is sending");
		break;
	case MKError_PortNoCapture:
		return _T("This port does not support capture");
		break;
	case MKError_PortNoSend:
		return _T("This port does not support send");
		break;
	case MKError_PortNoSimul:
		return _T("This port does not support simul");

	case MKError_DeviceIoControl:
		return _T("DeviceIoControl return FALSE");
		break;

	case MKError_SyncStandard:
		return _T("Broadcast standard can not be synchronized with the synchronous standard");
		break;

	case MKError_WriteEROM:
		return _T("Write EROM failed");
		break;

	case MKError_VideoBuffLength:
		return _T("The length of video buffer not enough");
		break;

	case MKError_AudioBuffLength:
		return _T("The length of audio buffer not enough");
		break;

	case MKError_ReadData:
		return _T("Read DMA data failed");
		break;

	case MKError_WriteData:
		return _T("Write DMA data failed");
		break;

	case MKError_DeviceOpen:
		return _T("Device open failed");
		break;

	case MKError_DeviceInit:
		return _T("Device init failed");
		break;

	case MKError_ReadOpen:
		return _T("Device read open failed");
		break;

	case MKError_WriteOpen:
		return _T("Device write open failed");
		break;

	case MKError_ParamSize:
		return _T("Param size is error");
		break;

	case MKError_NotSupport:
		return _T("Device does not support");
		break;

	case MKError_NoCache:
		return _T("No more cache");
		break;
	
	case MKError_BBInterrupt:
		return _T("Create BB interrupt failed");
		break;

	case MKError_CardIsOpen:
		return _T("Card is open");
		break;

	case MKError_UnknowDevice:
		return _T("Unknow device");
		break;

	case MKError_NoCross:
		return _T("No cross api service");

	case MKError_TimeOut:
		return _T("Time out");

	case MKError_Socket:
		return _T("Socket error");

	case MKError_NoReplay:
		return _T("Control cmd no replay");

	case MKError_Drop:
		return _T("Drop frame by frame counter");

	case MKError_DeviceRemoved:
		return _T("Device has been removed");

	case MKError_NoSerialNumber:
		return _T("Device has no SerialNumber");

	case MKError_SetDMAInterrupt:
		return _T("Set DMA interrupt failed");
		break;

	case MKError_UnknowCommand:
		return _T("Unknown Command");
	default:
		break;
	}

	return _T("Unknown error");
}

static int MKApi_GetVideoWidth(MKVideoStandard standard)
{
	switch(standard)
	{
	case MKVS_SD_486I_59_94:		// 720x486	NTSC 数字电视
		return 720;
		
	case MKVS_ASI:
	case MKVS_SD_576I_50:			// 720x576	PAL
		return 720;

	case MKVS_1024_768_50P:			// 1024x768
	case MKVS_1024_768_60P:
	case MKVS_1024_768_70P:
	case MKVS_1024_768_75P:
		return 1024;

	case MKVS_HD_720P_50:			// 1280x720
	case MKVS_HD_720P_59_94:
	case MKVS_HD_720P_60:
	case MKVS_HD_720P_23_98:
	case MKVS_HD_720P_24:
	case MKVS_HD_720P_25:
	case MKVS_HD_720P_29_97:
	case MKVS_HD_720P_30:
		return 1280;

	case MKVS_HD_1035I_59_94:		// 1920x1035
	case MKVS_HD_1035I_60:
	case MKVS_HD_1080I_50:			// 1920x1080
	case MKVS_HD_1080I_59_94:
	case MKVS_HD_1080I_60:
	case MKVS_HD_1080P_50_LINKB:
	case MKVS_HD_1080P_59_94_LINKB:
	case MKVS_HD_1080P_60_LINKB:
	case MKVS_HD_1080P_23_98:
	case MKVS_HD_1080P_24:
	case MKVS_HD_1080P_25:
	case MKVS_HD_1080P_29_97:
	case MKVS_HD_1080P_30:
	case MKVS_HD_1080P_50:
	case MKVS_HD_1080P_59_94:
	case MKVS_HD_1080P_60:
		return 1920;

	case MKVS_HD_1080P_23_98_DCI:
	case MKVS_HD_1080P_24_DCI:
	case MKVS_HD_1080P_25_DCI:
	case MKVS_HD_1080P_29_97_DCI:
	case MKVS_HD_1080P_30_DCI:
	case MKVS_HD_1080P_50_DCI:
	case MKVS_HD_1080P_59_94_DCI:
	case MKVS_HD_1080P_60_DCI:
		return 2048;

	case MKVS_ULTRA_HD_2160P_23_98:	// 3840x2160
	case MKVS_ULTRA_HD_2160P_24:
	case MKVS_ULTRA_HD_2160P_25:
	case MKVS_ULTRA_HD_2160P_29_97:
	case MKVS_ULTRA_HD_2160P_30:
	case MKVS_ULTRA_HD_2160P_50:
	case MKVS_ULTRA_HD_2160P_59_94:
	case MKVS_ULTRA_HD_2160P_60:
		return 3840;

	case MKVS_ULTRA_HD_2160P_23_98_DCI:
	case MKVS_ULTRA_HD_2160P_24_DCI:
	case MKVS_ULTRA_HD_2160P_25_DCI:
	case MKVS_ULTRA_HD_2160P_29_97_DCI:
	case MKVS_ULTRA_HD_2160P_30_DCI:
	case MKVS_ULTRA_HD_2160P_50_DCI:
	case MKVS_ULTRA_HD_2160P_59_94_DCI:
	case MKVS_ULTRA_HD_2160P_60_DCI:
		return 4096;

	case MKVS_ULTRA_HD_4320P_23_98:	// 7680x4320
	case MKVS_ULTRA_HD_4320P_24:
	case MKVS_ULTRA_HD_4320P_25:
	case MKVS_ULTRA_HD_4320P_29_97:
	case MKVS_ULTRA_HD_4320P_30:
	case MKVS_ULTRA_HD_4320P_50:
	case MKVS_ULTRA_HD_4320P_59_94:
	case MKVS_ULTRA_HD_4320P_60:
		return 7680;
	case MKVS_ULTRA_HD_4320P_23_98_DCI:
	case MKVS_ULTRA_HD_4320P_24_DCI:
	case MKVS_ULTRA_HD_4320P_25_DCI:
	case MKVS_ULTRA_HD_4320P_29_97_DCI:
	case MKVS_ULTRA_HD_4320P_30_DCI:
	case MKVS_ULTRA_HD_4320P_50_DCI:
	case MKVS_ULTRA_HD_4320P_59_94_DCI:
	case MKVS_ULTRA_HD_4320P_60_DCI:
		return 8192;
	case MKVS_800_600_P60:
		return 800;
	default:
		return 0;
	}
	return 0;
}

static int MKApi_GetVideoHeight(MKVideoStandard standard)
{
	switch(standard)
	{
	case MKVS_SD_486I_59_94:		// 720x486	NTSC 数字电视
		return 486;
		
	case MKVS_ASI:
	case MKVS_SD_576I_50:			// 720x576	PAL
		return 576;

	case MKVS_1024_768_50P:			// 1024x768
	case MKVS_1024_768_60P:
	case MKVS_1024_768_70P:
	case MKVS_1024_768_75P:
		return 768;

	case MKVS_HD_720P_50:			// 1280x720
	case MKVS_HD_720P_59_94:
	case MKVS_HD_720P_60:
	case MKVS_HD_720P_23_98:
	case MKVS_HD_720P_24:
	case MKVS_HD_720P_25:
	case MKVS_HD_720P_29_97:
	case MKVS_HD_720P_30:
		return 720;

	case MKVS_HD_1035I_59_94:		// 1920x1035
	case MKVS_HD_1035I_60:
		return 1035;

	case MKVS_HD_1080I_50:			// 1920x1080
	case MKVS_HD_1080I_59_94:
	case MKVS_HD_1080I_60:
	case MKVS_HD_1080P_50_LINKB:
	case MKVS_HD_1080P_59_94_LINKB:
	case MKVS_HD_1080P_60_LINKB:
	case MKVS_HD_1080P_23_98:
	case MKVS_HD_1080P_24:
	case MKVS_HD_1080P_25:
	case MKVS_HD_1080P_29_97:
	case MKVS_HD_1080P_30:
	case MKVS_HD_1080P_50:
	case MKVS_HD_1080P_59_94:
	case MKVS_HD_1080P_60:
		return 1080;
	case MKVS_HD_1080P_23_98_DCI:
	case MKVS_HD_1080P_24_DCI:
	case MKVS_HD_1080P_25_DCI:
	case MKVS_HD_1080P_29_97_DCI:
	case MKVS_HD_1080P_30_DCI:
	case MKVS_HD_1080P_50_DCI:
	case MKVS_HD_1080P_59_94_DCI:
	case MKVS_HD_1080P_60_DCI:
		return 1080;
	case MKVS_ULTRA_HD_2160P_23_98: // 3840x2160
	case MKVS_ULTRA_HD_2160P_24:
	case MKVS_ULTRA_HD_2160P_25:
	case MKVS_ULTRA_HD_2160P_29_97:
	case MKVS_ULTRA_HD_2160P_30:
	case MKVS_ULTRA_HD_2160P_50:
	case MKVS_ULTRA_HD_2160P_59_94:
	case MKVS_ULTRA_HD_2160P_60:
		return 2160;
	case MKVS_ULTRA_HD_2160P_23_98_DCI:
	case MKVS_ULTRA_HD_2160P_24_DCI:
	case MKVS_ULTRA_HD_2160P_25_DCI:
	case MKVS_ULTRA_HD_2160P_29_97_DCI:
	case MKVS_ULTRA_HD_2160P_30_DCI:
	case MKVS_ULTRA_HD_2160P_50_DCI:
	case MKVS_ULTRA_HD_2160P_59_94_DCI:
	case MKVS_ULTRA_HD_2160P_60_DCI:
		return 2160;
	case MKVS_ULTRA_HD_4320P_23_98: // 3840x2160
	case MKVS_ULTRA_HD_4320P_24:
	case MKVS_ULTRA_HD_4320P_25:
	case MKVS_ULTRA_HD_4320P_29_97:
	case MKVS_ULTRA_HD_4320P_30:
	case MKVS_ULTRA_HD_4320P_50:
	case MKVS_ULTRA_HD_4320P_59_94:
	case MKVS_ULTRA_HD_4320P_60:
		return 4320;
	case MKVS_ULTRA_HD_4320P_23_98_DCI: // 3840x2160
	case MKVS_ULTRA_HD_4320P_24_DCI:
	case MKVS_ULTRA_HD_4320P_25_DCI:
	case MKVS_ULTRA_HD_4320P_29_97_DCI:
	case MKVS_ULTRA_HD_4320P_30_DCI:
	case MKVS_ULTRA_HD_4320P_50_DCI:
	case MKVS_ULTRA_HD_4320P_59_94_DCI:
	case MKVS_ULTRA_HD_4320P_60_DCI:
		return 4320;
	case MKVS_800_600_P60:
		return 600;
	default:
		return 0;
	}
	return 0;
}

static int MKApi_GetVideoTimeNum(MKVideoStandard standard)
{
	switch(standard)
	{
	case MKVS_HD_720P_24:
	case MKVS_HD_1080P_24:
	case MKVS_HD_1080P_24_DCI:
	case MKVS_ULTRA_HD_2160P_24:
	case MKVS_ULTRA_HD_2160P_24_DCI:
	case MKVS_ULTRA_HD_4320P_24:
	case MKVS_ULTRA_HD_4320P_24_DCI:
		return 24;

	case MKVS_ASI:
	case MKVS_SD_576I_50:
	case MKVS_HD_720P_25:
	case MKVS_HD_1080I_50:
	case MKVS_HD_1080P_50_LINKB:
	case MKVS_HD_1080P_25:
	case MKVS_HD_1080P_25_DCI:
	case MKVS_ULTRA_HD_2160P_25:
	case MKVS_ULTRA_HD_2160P_25_DCI:
	case MKVS_ULTRA_HD_4320P_25:
	case MKVS_ULTRA_HD_4320P_25_DCI:
		return 25;

	case MKVS_HD_720P_30:
	case MKVS_HD_1035I_60:
	case MKVS_HD_1080I_60:
	case MKVS_HD_1080P_60_LINKB:
	case MKVS_HD_1080P_30:
	case MKVS_HD_1080P_30_DCI:
	case MKVS_ULTRA_HD_2160P_30:
	case MKVS_ULTRA_HD_2160P_30_DCI:
	case MKVS_ULTRA_HD_4320P_30:
	case MKVS_ULTRA_HD_4320P_30_DCI:
		return 30;

	case MKVS_1024_768_50P:
	case MKVS_HD_720P_50:
	case MKVS_HD_1080P_50:
	case MKVS_HD_1080P_50_DCI:
	case MKVS_ULTRA_HD_2160P_50:
	case MKVS_ULTRA_HD_2160P_50_DCI:
	case MKVS_ULTRA_HD_4320P_50:
	case MKVS_ULTRA_HD_4320P_50_DCI:
		return 50;

	case MKVS_1024_768_60P:
	case MKVS_HD_720P_60:
	case MKVS_HD_1080P_60:
	case MKVS_HD_1080P_60_DCI:
	case MKVS_ULTRA_HD_2160P_60:
	case MKVS_ULTRA_HD_2160P_60_DCI:
	case MKVS_ULTRA_HD_4320P_60:
	case MKVS_ULTRA_HD_4320P_60_DCI:
	case MKVS_800_600_P60:
		return 60;
	case MKVS_1024_768_70P:
		return 70;
	case MKVS_1024_768_75P:
		return 75;

	case MKVS_HD_720P_23_98:
	case MKVS_HD_1080P_23_98:
	case MKVS_HD_1080P_23_98_DCI:
	case MKVS_ULTRA_HD_2160P_23_98:
	case MKVS_ULTRA_HD_2160P_23_98_DCI:
	case MKVS_ULTRA_HD_4320P_23_98:
	case MKVS_ULTRA_HD_4320P_23_98_DCI:
		return 24000;

	case MKVS_SD_486I_59_94:
	case MKVS_HD_720P_29_97:
	case MKVS_HD_1035I_59_94:
	case MKVS_HD_1080I_59_94:
	case MKVS_HD_1080P_59_94_LINKB:
	case MKVS_HD_1080P_29_97:
	case MKVS_HD_1080P_29_97_DCI:
	case MKVS_ULTRA_HD_2160P_29_97:
	case MKVS_ULTRA_HD_2160P_29_97_DCI:
	case MKVS_ULTRA_HD_4320P_29_97:
	case MKVS_ULTRA_HD_4320P_29_97_DCI:
		return 30000;

	case MKVS_HD_720P_59_94:
	case MKVS_HD_1080P_59_94:
	case MKVS_HD_1080P_59_94_DCI:
	case MKVS_ULTRA_HD_2160P_59_94:
	case MKVS_ULTRA_HD_2160P_59_94_DCI:
	case MKVS_ULTRA_HD_4320P_59_94:
	case MKVS_ULTRA_HD_4320P_59_94_DCI:
		return 60000;

	default:
		return 0;
	}
}

static int MKApi_GetVideoTimeScale(MKVideoStandard standard)
{
	switch(standard)
	{
	case MKVS_ASI:
	case MKVS_SD_576I_50:
	case MKVS_1024_768_50P:
	case MKVS_1024_768_60P:
	case MKVS_1024_768_70P:
	case MKVS_1024_768_75P:
	case MKVS_HD_720P_24:
	case MKVS_HD_720P_25:
	case MKVS_HD_720P_30:
	case MKVS_HD_720P_50:			
	case MKVS_HD_720P_60:
	case MKVS_HD_1035I_60:
	case MKVS_HD_1080I_50:
	case MKVS_HD_1080I_60:
	case MKVS_HD_1080P_50_LINKB:
	case MKVS_HD_1080P_60_LINKB:
	case MKVS_HD_1080P_24:
	case MKVS_HD_1080P_25:
	case MKVS_HD_1080P_30:
	case MKVS_HD_1080P_50:
	case MKVS_HD_1080P_60:
	case MKVS_ULTRA_HD_2160P_24:
	case MKVS_ULTRA_HD_2160P_25:
	case MKVS_ULTRA_HD_2160P_30:
	case MKVS_ULTRA_HD_2160P_50:
	case MKVS_ULTRA_HD_2160P_60:
	case MKVS_HD_1080P_24_DCI:
	case MKVS_HD_1080P_25_DCI:
	case MKVS_HD_1080P_30_DCI:
	case MKVS_HD_1080P_50_DCI:
	case MKVS_HD_1080P_60_DCI:
	case MKVS_ULTRA_HD_2160P_24_DCI:
	case MKVS_ULTRA_HD_2160P_25_DCI:
	case MKVS_ULTRA_HD_2160P_30_DCI:
	case MKVS_ULTRA_HD_2160P_50_DCI:
	case MKVS_ULTRA_HD_2160P_60_DCI:
	case MKVS_ULTRA_HD_4320P_24:
	case MKVS_ULTRA_HD_4320P_25:
	case MKVS_ULTRA_HD_4320P_30:
	case MKVS_ULTRA_HD_4320P_50:
	case MKVS_ULTRA_HD_4320P_60:
	case MKVS_ULTRA_HD_4320P_24_DCI:
	case MKVS_ULTRA_HD_4320P_25_DCI:
	case MKVS_ULTRA_HD_4320P_30_DCI:
	case MKVS_ULTRA_HD_4320P_50_DCI:
	case MKVS_ULTRA_HD_4320P_60_DCI:
	case MKVS_800_600_P60:
		return 1;

	case MKVS_SD_486I_59_94:		
	case MKVS_HD_720P_23_98:
	case MKVS_HD_720P_29_97:
	case MKVS_HD_720P_59_94:
	case MKVS_HD_1035I_59_94:
	case MKVS_HD_1080I_59_94:
	case MKVS_HD_1080P_59_94_LINKB:
	case MKVS_HD_1080P_23_98:
	case MKVS_HD_1080P_29_97:
	case MKVS_HD_1080P_59_94:
	case MKVS_ULTRA_HD_2160P_23_98:
	case MKVS_ULTRA_HD_2160P_29_97:
	case MKVS_ULTRA_HD_2160P_59_94:
	case MKVS_HD_1080P_23_98_DCI:
	case MKVS_HD_1080P_29_97_DCI:
	case MKVS_HD_1080P_59_94_DCI:
	case MKVS_ULTRA_HD_2160P_23_98_DCI:
	case MKVS_ULTRA_HD_2160P_29_97_DCI:
	case MKVS_ULTRA_HD_2160P_59_94_DCI:
	case MKVS_ULTRA_HD_4320P_23_98:
	case MKVS_ULTRA_HD_4320P_29_97:
	case MKVS_ULTRA_HD_4320P_59_94:
	case MKVS_ULTRA_HD_4320P_23_98_DCI:
	case MKVS_ULTRA_HD_4320P_29_97_DCI:
	case MKVS_ULTRA_HD_4320P_59_94_DCI:
		return 1001;

	default:
		break;
	}
	return 1;
}

static float MKApi_GetVideoFps(MKVideoStandard standard)
{
	int time_num = MKApi_GetVideoTimeNum(standard);
	int time_scale = MKApi_GetVideoTimeScale(standard);

	if (time_num && time_scale)
		return (float)time_num/time_scale;
	return 0;
}

static LPCTSTR MKApi_GetVideoStandardDesc(MKVideoStandard standard)
{
	switch(standard)
	{
	case MKVS_SD_486I_59_94:		// 720x486	NTSC 数字电视
		return _T("486I 59.94");
		
	case MKVS_SD_576I_50:			// 720x576	PAL
		return _T("576I 50");

	case MKVS_1024_768_50P:			// 1024x768
		return _T("1024 x 768 50P");
	case MKVS_1024_768_60P:
		return _T("1024 x 768 60P");
	case MKVS_1024_768_70P:
		return _T("1024 x 768 70P");
	case MKVS_1024_768_75P:
		return _T("1024 x 768 75P");

	case MKVS_HD_720P_23_98:		// 1280x720
		return _T("720P 23.98");
	case MKVS_HD_720P_24:
		return _T("720P 24");
	case MKVS_HD_720P_25:
		return _T("720P 25");
	case MKVS_HD_720P_29_97:
		return _T("720P 29.97");
	case MKVS_HD_720P_30:
		return _T("720P 30");
	case MKVS_HD_720P_50:
		return _T("720P 50");
	case MKVS_HD_720P_59_94:
		return _T("720P 59.94");
	case MKVS_HD_720P_60:
		return _T("720P 60");

	case MKVS_HD_1035I_59_94:
		return _T("1035I 59.94");
	case MKVS_HD_1035I_60:
		return _T("1035I 60");

	case MKVS_HD_1080I_50:			// 1920x1080
		return _T("1080I 50");
	case MKVS_HD_1080I_59_94:
		return _T("1080I 59.94");
	case MKVS_HD_1080I_60:
		return _T("1080I 60");

	case MKVS_HD_1080P_50_LINKB:
		return _T("1080P 50 LinkB");
	case MKVS_HD_1080P_59_94_LINKB:
		return _T("1080P 59.94 LinkB");
	case MKVS_HD_1080P_60_LINKB:
		return _T("1080P 60 LinkB");

	case MKVS_HD_1080P_23_98:
		return _T("1080P 23.98");
	case MKVS_HD_1080P_24:
		return _T("1080P 24");
	case MKVS_HD_1080P_25:
		return _T("1080P 25");
	case MKVS_HD_1080P_29_97:
		return _T("1080P 29.97");
	case MKVS_HD_1080P_30:
		return _T("1080P 30");
	case MKVS_HD_1080P_50:
		return _T("1080P 50");
	case MKVS_HD_1080P_59_94:
		return _T("1080P 59.94");
	case MKVS_HD_1080P_60:
		return _T("1080P 60");

	case MKVS_HD_1080P_23_98_DCI:
		return _T("1080P 23.98 DCI");
	case MKVS_HD_1080P_24_DCI:
		return _T("1080P 24 DCI");
	case MKVS_HD_1080P_25_DCI:
		return _T("1080P 25 DCI");
	case MKVS_HD_1080P_29_97_DCI:
		return _T("1080P 29.97 DCI");
	case MKVS_HD_1080P_30_DCI:
		return _T("1080P 30 DCI");
	case MKVS_HD_1080P_50_DCI:
		return _T("1080P 50 DCI");
	case MKVS_HD_1080P_59_94_DCI:
		return _T("1080P 59.94 DCI");
	case MKVS_HD_1080P_60_DCI:
		return _T("1080P 60 DCI");
	case MKVS_ULTRA_HD_2160P_23_98:
		return _T("2160P 23.98");
	case MKVS_ULTRA_HD_2160P_24:
		return _T("2160P 24");
	case MKVS_ULTRA_HD_2160P_25:
		return _T("2160P 25");
	case MKVS_ULTRA_HD_2160P_29_97:
		return _T("2160P 29.97");
	case MKVS_ULTRA_HD_2160P_30:
		return _T("2160P 30");
	case MKVS_ULTRA_HD_2160P_50:
		return _T("2160P 50");
	case MKVS_ULTRA_HD_2160P_59_94:
		return _T("2160P 59.94");
	case MKVS_ULTRA_HD_2160P_60:
		return _T("2160P 60");

	case MKVS_ULTRA_HD_2160P_23_98_DCI:
		return _T("2160P 23.98 DCI");
	case MKVS_ULTRA_HD_2160P_24_DCI:
		return _T("2160P 24 DCI");
	case MKVS_ULTRA_HD_2160P_25_DCI:
		return _T("2160P 25 DCI");
	case MKVS_ULTRA_HD_2160P_29_97_DCI:
		return _T("2160P 29.97 DCI");
	case MKVS_ULTRA_HD_2160P_30_DCI:
		return _T("2160P 30 DCI");
	case MKVS_ULTRA_HD_2160P_50_DCI:
		return _T("2160P 50 DCI");
	case MKVS_ULTRA_HD_2160P_59_94_DCI:
		return _T("2160P 59.94 DCI");
	case MKVS_ULTRA_HD_2160P_60_DCI:
		return _T("2160P 60 DCI");
	case MKVS_ULTRA_HD_4320P_23_98:
		return _T("4320P 23.98");

	case MKVS_ULTRA_HD_4320P_24:
		return _T("4320P 24");
	case MKVS_ULTRA_HD_4320P_25:
		return _T("4320P 25");
	case MKVS_ULTRA_HD_4320P_29_97:
		return _T("4320P 29.97");
	case MKVS_ULTRA_HD_4320P_30:
		return _T("4320P 30");
	case MKVS_ULTRA_HD_4320P_50:
		return _T("4320P 50");
	case MKVS_ULTRA_HD_4320P_59_94:
		return _T("4320P 59.94");
	case MKVS_ULTRA_HD_4320P_60:
		return _T("4320P 60");

	case MKVS_ULTRA_HD_4320P_23_98_DCI:
		return _T("4320P 23.98 DCI");
	case MKVS_ULTRA_HD_4320P_24_DCI:
		return _T("4320P 24 DCI");
	case MKVS_ULTRA_HD_4320P_25_DCI:
		return _T("4320P 25 DCI");
	case MKVS_ULTRA_HD_4320P_29_97_DCI:
		return _T("4320P 29.97 DCI");
	case MKVS_ULTRA_HD_4320P_30_DCI:
		return _T("4320P 30 DCI");
	case MKVS_ULTRA_HD_4320P_50_DCI:
		return _T("4320P 50 DCI");
	case MKVS_ULTRA_HD_4320P_59_94_DCI:
		return _T("4320P 59.94 DCI");
	case MKVS_ULTRA_HD_4320P_60_DCI:
		return _T("4320P 60 DCI");
	case MKVS_ASI:
		return _T("ASI");
	case MKVS_NonStandard:
		return _T("Non Standard");
	case MKVS_800_600_P60:
		return _T("800x600 P60");
	default:
		return _T("Unknow");
	}
}

static bool MKApi_IsProgressiveFrame(MKVideoStandard vstd)
{
	switch (vstd)
	{
	case MKVS_SD_486I_59_94:
	case MKVS_SD_576I_50:
	case MKVS_HD_1035I_59_94:
	case MKVS_HD_1035I_60:
	case MKVS_HD_1080I_50:
	case MKVS_HD_1080I_59_94:
	case MKVS_HD_1080I_60:
	case MKVS_HD_1080P_50_LINKB:
	case MKVS_HD_1080P_59_94_LINKB:
	case MKVS_HD_1080P_60_LINKB:
		return false;
	default:
		break;
	}
	return true;
}

static int MKApi_GetAudioFrameSampleCount(unsigned __int64 iFrameCounter, int video_time_num, int video_time_scale)
{
	static int AUDIO_2997[5] = { 1602, 1601, 1602, 1601, 1602 };
	static int AUDIO_5997[5] = { 801, 801, 800, 801, 801 };

	switch (video_time_num)
	{
	case 24000:
		return 2 * video_time_scale;
		break;
	case 30000:
	{
		int nIndex = iFrameCounter % 5;
		return AUDIO_2997[nIndex];
	}
	break;
	case 60000:
	{
		int nIndex = iFrameCounter % 5;
		return AUDIO_5997[nIndex];
	}
	break;
	}
	return 48000 / video_time_num;
}

static void MKApi_FormatVideoInfoEx(TCHAR* pszStr, int cchMax, MKVideoInfoEx_t* p_video_info)
{
	if (p_video_info && pszStr)
	{
		if (p_video_info->standard == MKVS_NonStandard)
		{
			if (p_video_info->bProgressiveFrame)
			{
				if (p_video_info->frameRate_D == 1)
					_stprintf_s(pszStr, cchMax, _T("%dx%d_P%d"), p_video_info->width,
						p_video_info->height, p_video_info->frameRate_N);
				else
					_stprintf_s(pszStr, cchMax, _T("%dx%d_P%.2f"), p_video_info->width,
						p_video_info->height, p_video_info->fps);
			}
			else
			{
				if (p_video_info->frameRate_D == 1)
					_stprintf_s(pszStr, cchMax, _T("%dx%d_I%d"), p_video_info->width,
						p_video_info->height, p_video_info->frameRate_N * 2);
				else
					_stprintf_s(pszStr, cchMax, _T("%dx%d_I%.2f"), p_video_info->width,
						p_video_info->height, p_video_info->fps * 2);
			}
		}
		else
		{
			_tcscpy_s(pszStr, cchMax, 
				MKApi_GetVideoStandardDesc(p_video_info->standard));
		}
	}
}

#endif//__MKCard_API_FUN_H__