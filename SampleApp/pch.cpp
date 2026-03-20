// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

#ifdef _AMD64_
#pragma comment(lib, "libMKCardCrossApi_x64.lib")
#else
#pragma comment(lib, "libMKCardCrossApi_x86.lib")
#endif

#pragma comment(lib ,"Shlwapi.lib")

#define clip(x, min, max) (x > max ? max : (x < min ? min : x));

static const float g_matYUV_2_RGB_full_709[4][4] = {
	{ 1.16438356f, 0, 1.79274107f, 0 },
	{ 1.16438356f, -0.21324861f, -0.53290933f, 0 },
	{ 1.16438356f, 2.11240179f, 0, 0 },
	{ 0, 0, 0, 1 }
};

__inline void YUV_2_RGB_Full_BT709(int Y, int U, int V, unsigned char& R, unsigned char& G, unsigned char& B)
{
	Y -= 16;
	U -= 128;
	V -= 128;
	R = clip(int(g_matYUV_2_RGB_full_709[0][0] * Y + g_matYUV_2_RGB_full_709[0][1] * U + g_matYUV_2_RGB_full_709[0][2] * V + 0.5), 0, 255);
	G = clip(int(g_matYUV_2_RGB_full_709[1][0] * Y + g_matYUV_2_RGB_full_709[1][1] * U + g_matYUV_2_RGB_full_709[1][2] * V + 0.5), 0, 255);
	B = clip(int(g_matYUV_2_RGB_full_709[2][0] * Y + g_matYUV_2_RGB_full_709[2][1] * U + g_matYUV_2_RGB_full_709[2][2] * V + 0.5), 0, 255);
}

void convert_UYVY_BGRA(BYTE *pSrc, int src_stride, int nWidth, int nHeight, BYTE *pDest, int dst_stride)
{
	int x_count = nWidth >> 1;
	int src_extra = src_stride - (nWidth << 1);
	int dst_extra = dst_stride - (nWidth << 2);
	int i, j;
	for (i = 0; i < nHeight; i++)
	{
		for (j = 0; j < x_count; j++)
		{
			YUV_2_RGB_Full_BT709(pSrc[1], pSrc[0], pSrc[2], pDest[2], pDest[1], pDest[0]);
			pDest[3] = 255;
			pDest += 4;

			YUV_2_RGB_Full_BT709(pSrc[3], pSrc[0], pSrc[2], pDest[2], pDest[1], pDest[0]);
			pDest[3] = 255;
			pDest += 4;

			pSrc += 4;
		}
		pSrc += src_extra;
		pDest += dst_extra;
	}
}

BOOL FrameToTimeCode(unsigned __int64 llFrameCounter, int& nHour, int& nMinute, int& nSecond, int& nFrame, double fps)
{
	double fSecond = llFrameCounter / fps;
	nSecond = (int)fSecond;
	fSecond -= nSecond;
	nHour = nSecond / 3600;
	nSecond -= nHour * 3600;
	nMinute = nSecond / 60;
	nSecond -= nMinute * 60;

	nFrame = int(fSecond * fps + 0.5);

	return TRUE;
}

static int AUDIO_2997[5] = { 1602, 1601, 1602, 1601, 1602 };
static int AUDIO_5997[5] = { 801, 801, 800, 801, 801 };

int GetAudioFrameSampleCount(unsigned __int64 iCounter, int video_time_num, int video_time_scale)
{
	switch (video_time_num)
	{
	case 24000:
		return 2 * video_time_scale;
		break;
	case 30000:
	{
		int nIndex = iCounter % 5;
		return AUDIO_2997[nIndex];
	}
	break;
	case 60000:
	{
		int nIndex = iCounter % 5;
		return AUDIO_5997[nIndex];
	}
	break;
	}
	return 48000 / video_time_num;
}

void AllocAudioFrame(AUDIO_FRAME_SAMPLE* pAudioFrame, int totalSize, int planarSize)
{
	pAudioFrame->pAllData = new unsigned char[totalSize];
	pAudioFrame->nAllocBytes = totalSize;

	pAudioFrame->buffer[0] = pAudioFrame->pAllData;
	pAudioFrame->buffer_alloc_length[0] = planarSize;
	pAudioFrame->data_length[0] = 0;
	for (int i = 1; i < MKAudio_CHANNEL_COUNT_MAX; i++)
	{
		pAudioFrame->buffer[i] = pAudioFrame->buffer[i - 1] + planarSize;
		pAudioFrame->buffer_alloc_length[i] = planarSize;

		pAudioFrame->data_length[i] = 0;
	}
}

