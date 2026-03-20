// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <afxmt.h>
#include <MMSystem.h>
#include <Shlwapi.h>

#pragma warning(disable:4786)
#include <set>
#include <map>
#include <vector>

#include "VBufferT.h"
#include "EC_ThreadData.h"
#include <afxcontrolbars.h>
#include "FifoList.h"

#include "MKCardApiFun.h"
#include "MKCardCrossApi.h"

#include "MKDeviceDef.h"

#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p)			{ if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)		{ if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_CLOSE_HANDLE(h)		{ if(h) { CloseHandle(h);   (h)=INVALID_HANDLE_VALUE; } }

#define MEDIA_PIX_FMT_UYVY 0x59565955				////FCC('UYVY'),< packed YUV 4:2:2, 8bit, Cb Y0 Cr Y1

struct AUDIO_FRAME_SAMPLE
{
	UINT32			buffer_alloc_length[MKAudio_CHANNEL_COUNT_MAX];	// 缓冲区长度
	UINT32			data_length[MKAudio_CHANNEL_COUNT_MAX];		// 数据长度
	BYTE*			buffer[MKAudio_CHANNEL_COUNT_MAX];			// 缓冲区指针

	int				nSampleCount;
	__int64			frame_counter;

	unsigned char*	pAllData;
	int	nAllocBytes;
	AUDIO_FRAME_SAMPLE()
	{
		memset(this, 0, sizeof(AUDIO_FRAME_SAMPLE));
	}
	~AUDIO_FRAME_SAMPLE()
	{
		SAFE_DELETE_ARRAY(pAllData);
	}
};

BOOL FrameToTimeCode(unsigned __int64 llFrameCounter, int& nHour, int& nMinute, int& nSecond, int& nFrame, double fps);
int GetAudioFrameSampleCount(unsigned __int64 iCounter, int video_time_num, int video_time_scale);
void AllocAudioFrame(AUDIO_FRAME_SAMPLE* pAudioFrame, int totalSize, int planarSize);
void convert_UYVY_BGRA(BYTE *pSrc, int src_stride, int nWidth, int nHeight, BYTE *pDest, int dst_stride);




#endif //PCH_H
