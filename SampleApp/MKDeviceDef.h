#ifndef _SCDEVICE_DEF_INCLUDE_H_
#define _SCDEVICE_DEF_INCLUDE_H_


struct MKCard_t
{
	HANDLE	m_hCard;
	CDialogEx*	m_pPortDlg[MKPort_COUNT_MAX];
};

struct MKDevice_t
{
	MKDeviceInfo_t  m_DeviceInfo;
	
	MKCard_t		m_CardList[MKCard_COUNT_MAX];
};

#endif //_SCDEVICE_DEF_INCLUDE_H_