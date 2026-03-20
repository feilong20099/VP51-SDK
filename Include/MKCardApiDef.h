#ifndef __MKCARD_API_DEF_H__
#define __MKCARD_API_DEF_H__


#define MKCard_COUNT_MAX 8
#define MKPort_COUNT_MAX 10
#define MKAudio_CHANNEL_COUNT_MAX 2

#ifndef IN
#	define IN
#endif

#ifndef OUT
#	define OUT
#endif

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#ifdef __GNUC__
#include <minwindef.h>
#include <minwinbase.h>
#include <tchar.h>
#include <guiddef.h>
#endif

#else
#define _T(x)  x
typedef char TCHAR, * PTCHAR;
typedef const char* LPCTSTR;

typedef int BOOL;
typedef unsigned long long UINT64, * PUINT64;
typedef void* HANDLE;

#define FALSE 0
#define TRUE 1

#endif

enum MKError
{
	MKError_No = 0x0000,		// 无错误

	MKError_SystemNotInit,			// 系统资源没有初始化
	MKError_SystemHadInitialized,	// 系统资源已经初始化

	MKError_DriverNotLoaded,		// 设备驱动没有加载

	MKError_Mem,					// 内存错误
	MKError_Param,					// 参数错误

	MKError_NoInitDeviceInfo,		// 没有初始化设备信息

	MKError_PortIsCapture,			// 端口正在采集
	MKError_PortIsSend,				// 端口正在播出

	MKError_PortNoCapture,			// 端口不支持采集
	MKError_PortNoSend,				// 端口不支持播出
	MKError_PortNoSimul,			// 端口不支持同步采集和播出

	MKError_DeviceIoControl,		// DeviceIoControl发生错误

	MKError_SyncStandard,			// 播出信号制式与同步信号制式无法同步

	MKError_WriteEROM,				// EROM写入操作发生错误

	MKError_VideoBuffLength,		// 视频缓存长度不够
	MKError_AudioBuffLength,		// 音频缓存长度不够
	MKError_ReadData,				// 读取数据失败
	MKError_WriteData,				// 写入数据失败

	MKError_DeviceOpen,				// 打开设备失败
	MKError_DeviceInit,				// 初始化设备失败

	MKError_ReadOpen,				// 打开读取失败
	MKError_WriteOpen,				// 打开写入失败

	MKError_ParamSize,				// 参数长度错误
	MKError_NotSupport,				// 当前设备不支持的功能

	MKError_UnknowDevice,			// 未知设备

	MKError_NoCache,				// 无缓存
	MKError_BBInterrupt,			// 创建中断失败

	MKError_CardIsOpen,				// 板卡已经被打开
	MKError_PortIsClose,			// 端口处于关闭状态

	MKError_TimeOut,				// 进程锁操作超时

	MKError_NoCross,				// 没有发现跨进程访问API服务程序

	MKError_Socket,					// 套接字错误

	MKError_NoReplay,				// 没有收到命令回应

	MKError_Drop,					// 播出帧设置不及时，自动丢弃

	MKError_DeviceRemoved,			// 设备已被移除

	MKError_NoSerialNumber,			// 设备没有序列号

	MKError_SetDMAInterrupt,		// 设置DMA中断失败

	MKError_UnknowCommand,			// 未知命令

	MKError_Unknow = 0xFFFF		// 未知错误

};

//////////////////////////////////////////////////////////////////////////

enum MKAudioPortType
{
	MKAPT_NONE			= 0x00,

	MKAPT_ESDI_IN		= 0x01,
	MKAPT_ESDI_OUT		= 0x02,

	MKAPT_ANALOG_IN		= 0x10,
	MKAPT_ANALOG_OUT	= 0x20,

	MKAPT_DIGITAL_IN	= 0x100,
	MKAPT_DIGITAL_OUT	= 0x200,

	MKAPT_HDMI_IN		= 0x1000,
	MKAPT_HDMI_OUT		= 0x2000,

};

enum MKVideoPortType
{
	MKVPT_SDI_IN		= 0x01,
	MKVPT_SDI_OUT		= 0x02,

	MKVPT_ASI_IN		= 0x04,
	MKVPT_ASI_OUT		= 0x08,
	
	MKVPT_HDMI_IN		= 0x10,
	MKVPT_HDMI_OUT		= 0x20,

	MKVPT_AV_IN			= 0x100,
	MKVPT_AV_OUT		= 0x200,

	MKVPT_AV_IN_SDI		= 0x1000,
	MKVPT_AV_IN_CVBS	= 0x2000,
	MKVPT_AV_IN_YC		= 0x4000,
	MKVPT_AV_IN_YUV		= 0x8000,

	MKVPT_SAMPLE_8BIT	= 0x10000,
	MKVPT_SAMPLE_10BIT	= 0x20000,

	MKVPT_IP_IN			= 0x100000,
	MKVPT_IP_OUT		= 0x200000,
};


//////////////////////////////////////////////////////////////////////////

enum MKCardType
{
	MKCardType_Unknow				= 0x0000,
	MKCardType_HD_05S				= 0x0001,	//HD SDI 4 in SDI 1 out
	MKCardType_HD_04H				= 0x0002,	//HD HDMI 4 in
	MKCardType_HD_EXPORT			= 0x0003,	//HD 2 SDI 1 HDMI out at the same time
	MKCardType_HD_05C				= 0x0004,	//HD ThunderBolt, SDI 4 in SDI 1 out
	MKCardType_HD_09S				= 0x0005,	//HD SDI 8 in SDI 1 out
	MKCardType_HD_05S_SUPER			= 0x0006,	//HD SDI 4 in (last 3 port can switch in or out ) SDI 1 out
	MKCardType_HD_08SH				= 0x0050,	//HD 04H 05S combine
	MKCardType_UHD_05S				= 0x0400,	//UHD SDI 4 in SDI 1 out
	MKCardType_UHD_04H				= 0x0401,	//UHD HDMI 4 in
	MKCardType_UHD_EXPORT			= 0x0402,	//UHD 2 SDI 1 HDMI out at the same time
	MKCardType_UHD_101S				= 0x0403,	//UHD SDI 1 in SDI 1 out
	MKCardType_UHD_101H				= 0x0404,	//UHD HDMI 1 in HDMI 1 out
	MKCardType_UHD_202SH			= 0x0405,	//UHD (SDI or HDMI) 1 in (SDI or HDMI) 1 out
	MKCardType_UHD_05S_SUPER		= 0x0406,	//UHD SDI 4 in (last 3 port can switch in or out )SDI 1 out
	MKCardType_UHD_01H				= 0x0407,	//UHD HDMI 1 in and HDMI 1 loop out

};

enum MKDMAMode
{
	MKDMAMode_Unknow = 0,
	MKDMAMode_MMap = 1,		//Memory Mapped
	MKDMAMode_Stream = 2,	//Streaming
};

enum MKVideoStandard
{
	MKVS_Unknow = 0,

	MKVS_SD_486I_59_94,			// 720x486	NTSC digital television

	MKVS_SD_576I_50,			// 720x576	PAL

	MKVS_1024_768_50P,			// 1024x768
	MKVS_1024_768_60P,
	MKVS_1024_768_70P,
	MKVS_1024_768_75P,

	MKVS_HD_720P_23_98,			// 1280x720
	MKVS_HD_720P_24,
	MKVS_HD_720P_25,
	MKVS_HD_720P_29_97,
	MKVS_HD_720P_30,
	MKVS_HD_720P_50,			// 1280x720
	MKVS_HD_720P_59_94,
	MKVS_HD_720P_60,

	MKVS_HD_1035I_59_94,		// 1920x1035
	MKVS_HD_1035I_60,

	MKVS_HD_1080I_50,			// 1920x1080
	MKVS_HD_1080I_59_94,
	MKVS_HD_1080I_60,

	MKVS_HD_1080P_50_LINKB,
	MKVS_HD_1080P_59_94_LINKB,
	MKVS_HD_1080P_60_LINKB,

	MKVS_HD_1080P_23_98,
	MKVS_HD_1080P_24,
	MKVS_HD_1080P_25,
	MKVS_HD_1080P_29_97,
	MKVS_HD_1080P_30,
	MKVS_HD_1080P_50,
	MKVS_HD_1080P_59_94,
	MKVS_HD_1080P_60,

	MKVS_HD_1080P_23_98_DCI,	// 2048x1080
	MKVS_HD_1080P_24_DCI,
	MKVS_HD_1080P_25_DCI,
	MKVS_HD_1080P_29_97_DCI,
	MKVS_HD_1080P_30_DCI,
	MKVS_HD_1080P_50_DCI,
	MKVS_HD_1080P_59_94_DCI,
	MKVS_HD_1080P_60_DCI,

	MKVS_ULTRA_HD_2160P_23_98,	// 3840x2160
	MKVS_ULTRA_HD_2160P_24,
	MKVS_ULTRA_HD_2160P_25,
	MKVS_ULTRA_HD_2160P_29_97,
	MKVS_ULTRA_HD_2160P_30,
	MKVS_ULTRA_HD_2160P_50,
	MKVS_ULTRA_HD_2160P_59_94,
	MKVS_ULTRA_HD_2160P_60,

	MKVS_ULTRA_HD_2160P_23_98_DCI,	// 4096x2160
	MKVS_ULTRA_HD_2160P_24_DCI,
	MKVS_ULTRA_HD_2160P_25_DCI,
	MKVS_ULTRA_HD_2160P_29_97_DCI,
	MKVS_ULTRA_HD_2160P_30_DCI,
	MKVS_ULTRA_HD_2160P_50_DCI,
	MKVS_ULTRA_HD_2160P_59_94_DCI,
	MKVS_ULTRA_HD_2160P_60_DCI,

	MKVS_ULTRA_HD_4320P_23_98,		// 7680x4320
	MKVS_ULTRA_HD_4320P_24,
	MKVS_ULTRA_HD_4320P_25,
	MKVS_ULTRA_HD_4320P_29_97,
	MKVS_ULTRA_HD_4320P_30,
	MKVS_ULTRA_HD_4320P_50,
	MKVS_ULTRA_HD_4320P_59_94,
	MKVS_ULTRA_HD_4320P_60,

	MKVS_ULTRA_HD_4320P_23_98_DCI,	// 8192x4320
	MKVS_ULTRA_HD_4320P_24_DCI,
	MKVS_ULTRA_HD_4320P_25_DCI,
	MKVS_ULTRA_HD_4320P_29_97_DCI,
	MKVS_ULTRA_HD_4320P_30_DCI,
	MKVS_ULTRA_HD_4320P_50_DCI,
	MKVS_ULTRA_HD_4320P_59_94_DCI,
	MKVS_ULTRA_HD_4320P_60_DCI,

	//end
	MKVS_ASI = 0x1000,			// ASI

	MKVS_NonStandard,
	MKVS_800_600_P60,			// 800x600

	MKVS_MAX

};


//////////////////////////////////////////////////////////////////////////

struct MKPortInfo_t
{
	MKAudioPortType	audio_type;							// 端口的音频类型
	MKVideoPortType	video_type;							// 端口的视频类型

	int				audio_max_channels;					// 音频最大支持的声道数量
	union DATA_SUPPORT
	{
		unsigned char all;
		struct
		{
			unsigned char video : 1;
			unsigned char audio : 1;
			unsigned char reserved : 6;
		};
	};
	DATA_SUPPORT	capture_data_spport;
	DATA_SUPPORT	send_data_spport;
};

struct MKCardInfo_t
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	wchar_t				device_path[1024];					// 设备路径
#else
	char				device_path[1024];					// 设备路径
#endif
	MKCardType			type;								// 卡类型
	unsigned short		hardware_version;					// 硬件版本
	unsigned short		hardware_type;						// 
	unsigned char		dma_mode;							// MKDMAMode
	unsigned char		port_config;						// in out config
	unsigned char		reserved0[2];						// 
	unsigned int		sub_ver;							// 硬件子版本
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	GUID				dev_guid;							// 硬件guid
#else
	unsigned char		dev_guid[16];						// 硬件guid
#endif
	unsigned short		max_video_width;
	unsigned short		max_video_height;
	unsigned char		cap_vsd_NonStandard;				// 采集端口支持非标准制式
	unsigned char		cap_color_adjust;					// 采集端口支持颜色调整
	unsigned char		reserved1[94];						// 

	int					port_count;							// 端口数量
	MKPortInfo_t		port_info_list[MKPort_COUNT_MAX];	// 端口信息

};

//wParam lParam reserved
typedef void (*MK_DeviceChangedCb) (WPARAM wParam, LPARAM lParam, void* p_user);

struct MKDeviceInfo_t
{
	int				card_count;								// 卡数量
	MKCardInfo_t	card_info_list[MKCard_COUNT_MAX];		// 卡信息

};

struct MKAudioInfo_t					// 音频数据信息结构
{
	unsigned char	sample_bits;		// 音频样本采样位数 32,24,16
	unsigned char	sample_frequency;	// 音频样本采样率	48K,44k,32k
	unsigned char	channels;			// 音频声道数量
} ;

struct MKVideoInfo_t					// 视频数据信息结构
{
	MKVideoStandard standard;			// 视频标准
	unsigned int	format;				// 视频数据格式 FOURCC码 MAKEFOURCC('U','Y','V','Y')
	int				sample_bits;		// 视频采样位数 8bit,10bit	
};

struct MKVideoInfoEx_t					// 视频数据信息结构
{
	MKVideoStandard standard;			// 视频标准
	int				width;				// 视频幅面 宽	
	int				height;				// 视频幅面 高
	BOOL			bProgressiveFrame;
	float			fps;				// 视频帧率	
	unsigned short	frameRate_N;		// 分子 numerator
	unsigned short	frameRate_D;		// 分母 denominator 
	unsigned char	reserved[64];		//
};

struct MKMediaInfo_t					// 数据信息结构
{
	MKAudioInfo_t	audio_info;			// 音频信息
	MKVideoInfo_t	video_info;			// 视频信息
};

//////////////////////////////////////////////////////////////////////////
struct MKControl_HVSync_t				// 外同步行场相位控制参数
{
	IN MKVideoStandard standard_send;	// 播出信号制式
	IN MKVideoStandard standard_sync;	// 同步信号制式

	OUT unsigned int hor;				// 行
	OUT unsigned int ver;				// 场
};

struct MKControl_AFDLine_t				// AFD信息插入行设置
{
	IN MKVideoStandard standard;		// 信号制式
	
	IN OUT short line0;					// 奇场插入行
	IN OUT short line1;					// 偶场插入行
};

struct MKControl_Regsiter_t				// 寄存器设置
{
	IN unsigned int address;			// 寄存器地址
	IN OUT unsigned int value;			// 寄存器值
	IN unsigned int bar_idx;
};

struct MKControl_I2C_t					//
{
	IN  char caller[10];				// 调用者
	IN  unsigned char sub_addr;					// 地址
	IN  unsigned char count;						// 处理数据个数
	IN OUT unsigned char value[32];				// 数据
} ;

enum MKCOLORADJUST_TYPE
{
	MKCOLORADJUST_BRIGHTNESS = 0x1,			// 亮度
	MKCOLORADJUST_SATURATION = 0x2,			// 饱和度
	MKCOLORADJUST_HUE = 0x4,				// 色调
	MKCOLORADJUST_COLORTEMPERATURE = 0x8,	// 色温
	MKCOLORADJUST_CONTRAST = 0x10,			// 对比度
};

struct MKControl_ColorAdjust			// 寄存器设置
{
	IN int type;						// MKCOLORADJUST_TYPE，可以是任意组合
	IN OUT DWORD Brightness;			// 亮度调整值,默认值128,范围[0,200]
	IN OUT DWORD Saturation;			// 饱和度调整值,默认值128,范围[0,140]
	IN OUT DWORD Hue;					// 色调调整值,默认值128,范围[110,140]
	IN OUT DWORD ColorTemperature;		// 色温调整值,默认值128,范围[110,140]
	IN OUT DWORD Contrast;				// 对比度调整值,默认值128,范围[80,200]
};
//////////////////////////////////////////////////////////////////////////

enum MKControlType
{										
	// 获取板卡芯片工作温度
	// 接口参数=int
	// 参数长度=sizeof(int)
	// 返回华氏温度值
	MKCT_GetTemperature,

	// 获取\设置板卡寄存器数据
	// 接口参数=SCControl_Regsiter_t
	// 参数长度=sizeof(SCControl_Regsiter_t)
	MKCT_GetReigster,
	MKCT_SetReigster,

	// 读取\写板卡I2C数据
	// 接口参数=MKControl_I2C_t
	// 参数长度=sizeof(MKControl_I2C_t)
	MKCT_ReadI2C,
	MKCT_WriteI2C,

	// 获取\设置板卡视频调整值
	// 接口参数=MKControl_ColorAdjust
	// 参数长度=sizeof(MKControl_ColorAdjust)
	MKCT_GetColorAdjust,
	MKCT_SetColorAdjust,
};

//////////////////////////////////////////////////////////////////////////

enum MKCbType							// 回调事件类型
{
	MKCbType_GetData,					// 可以获取数据
	MKCbType_SetData,					// 可以发送数据
	MKCbType_SignalChange,				// 信号发生改变
};

enum MKStreamType						// 流数据类型
{
	MKStreamType_Unknown,			
	MKStreamType_Audio,					// 音频
	MKStreamType_Video,					// 视频
	MKStreamType_SignalChange,
};

struct MKCaptureStreamCb_t				// 采集事件回调结构
{
	MKStreamType	stream_type;		// 采集事件类型
	int				channel;			// 通道
	UINT64			frame_counter;		// 帧计数
	int				length;				// 数据长度
	unsigned char*	buffer;				// 数据指针
	MKCardInfo_t	card_info;			// 触发回调的卡信息

} ;

typedef void (*MKApi_CaptureStreamCb) (const MKCaptureStreamCb_t* p_cbstream, void* p_user);

struct MKCaptureStreamParam_t					// 采集参数结构
{
	int				video_sample_bits;			// 视频采样位数 8bit,10bit
	MKApi_CaptureStreamCb	p_capture_video_cb;	// 视频数据回调函数
	void*			p_video_user;				// 视频回调函数用户数据
	MKApi_CaptureStreamCb	p_capture_audio_cb;	// 音频数据回调函数
	void*			p_audio_user;				// 音频回调函数用户数据
};

struct MKCaptureStreamParamEx : public MKCaptureStreamParam_t
{
	int memory_mode;
};

//////////////////////////////////////////////////////////////////////////

struct MKSendCb_t						// 播出事件回调结构
{
	MKCbType	cb_type;				// 播出事件类型

	HANDLE		send_handle;			// 触发事件的采集句柄
	MKCardInfo_t	card_info;			// 触发事件的卡信息
	int			port_index;				// 触发事件的端口序号
} ;

typedef void (*MKApi_SendCb) (const MKSendCb_t* p_cbdata, void* p_user);

struct MKSendParam_t					// 播出参数结构
{
	MKApi_SendCb	p_send_cb;			// 播出事件回调函数
	void*			p_user;				// 回调函数用户数据

	MKVideoInfo_t	video;				// 视频参数
};

struct MKSendParamEx : public MKSendParam_t
{
	unsigned int	process_id;
	int				send_buffer_mode;	// 1 = 共享内存模式
};

//////////////////////////////////////////////////////////////////////////

struct MKAudioData_t									// 音频数据结构
{
	int				buffer_alloc_length[MKAudio_CHANNEL_COUNT_MAX];	// 缓冲区长度
	int				data_length[MKAudio_CHANNEL_COUNT_MAX];		// 数据长度
	unsigned char*	buffer[MKAudio_CHANNEL_COUNT_MAX];			// 缓冲区指针
	
	MKAudioInfo_t	info;										// 音频信息

};

struct MKVideoData_t							// 视频数据结构
{
	int				buffer_alloc_length;		// 缓冲区长度
	int				data_length;				// 数据长度
	unsigned char*	buffer;						// 数据指针
	
	MKVideoInfo_t	info;						// 视频信息
} ;

struct MKMediaData_t							// 数据结构
{
	MKAudioData_t	audio_data;					// 音频数据
	MKVideoData_t	video_data;					// 视频数据

} ;


#endif