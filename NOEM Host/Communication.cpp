// Communication.cpp: implementation of the CCommunication class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Communication.h"
#include "Command.h"
#include "USBCommand.h"
#include "Device.h"
#include "crypt_user.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern HWND	g_hDevelopWnd;
void	DoEvents();

extern int g_nMaxFpCount;

int BAUDRATES[] = {9600,19200,38400,57600,115200,230400,460800,921600};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/************************************************************************/
/************************************************************************/
CCommunication::CCommunication()
{
	m_hUsbHandle = INVALID_HANDLE_VALUE;
	m_hMainWnd = NULL;
}
/************************************************************************/
/************************************************************************/
CCommunication::~CCommunication()
{
	if (m_hUsbHandle != INVALID_HANDLE_VALUE)
	{

	}
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_TestConnection(void)
{
	BOOL	w_bRet;
	
	InitCmdPacket(CMD_TEST_CONNECTION, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);
	
	SEND_COMMAND(CMD_TEST_CONNECTION, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_SetParam(int p_nParamIndex, int p_nParamValue)
{
	BOOL	w_bRet;
	BYTE	w_abyData[5];
	
	w_abyData[0] = p_nParamIndex;
	memcpy(&w_abyData[1], &p_nParamValue, 4);
	
	InitCmdPacket(CMD_SET_PARAM, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 5);
 	 	
	SEND_COMMAND(CMD_SET_PARAM, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetParam(int p_nParamIndex, int* p_pnParamValue)
{
	BOOL	w_bRet;
	BYTE	w_byData;

	w_byData = p_nParamIndex;
	
	InitCmdPacket(CMD_GET_PARAM, m_bySrcDeviceID, m_byDstDeviceID, &w_byData, 1);
 	 	
	SEND_COMMAND(CMD_GET_PARAM, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	memcpy(p_pnParamValue, g_pRcmPacket->m_abyData, 4);
	
	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
int CCommunication::Run_GetDeviceInfo(char* p_szDevInfo)
{
	BOOL	w_bRet;
	WORD	w_wDevInfoLen;
	
	w_bRet = Run_Command_NP(CMD_GET_DEVICE_INFO);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;	
	
	w_wDevInfoLen = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	
	RECEIVE_DATAPACKET(CMD_GET_DEVICE_INFO, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(w_bRet == false)
		return ERR_CONNECTION;
	
	if ( RESPONSE_RET != ERR_SUCCESS )	
		return RESPONSE_RET;
	
	memcpy(p_szDevInfo, g_pRcmPacket->m_abyData, w_wDevInfoLen);
	
	return ERR_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
int	CCommunication::Run_SetIDNote(int p_nTmplNo, BYTE* pNote)
{
	BOOL	w_bRet = false;
	BYTE	w_abyData[ID_NOTE_SIZE+2];
	WORD	w_wData;
	
	//. Assemble command packet
	w_wData = ID_NOTE_SIZE + 2;
	InitCmdPacket(CMD_SET_ID_NOTE, m_bySrcDeviceID, m_byDstDeviceID, (BYTE*)&w_wData, 2);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_SET_ID_NOTE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(w_bRet == false)
		return ERR_CONNECTION;
	
	if( RESPONSE_RET != ERR_SUCCESS)	
		return RESPONSE_RET;
	
	Sleep(10);
	
	//. Assemble data packet
	w_abyData[0] = LOBYTE(p_nTmplNo);
	w_abyData[1] = HIBYTE(p_nTmplNo);
	memcpy(&w_abyData[2], pNote, ID_NOTE_SIZE);
	
	InitCmdDataPacket(CMD_SET_ID_NOTE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, ID_NOTE_SIZE+2);
	
	//. Send data packet to target
	SEND_DATAPACKET(CMD_SET_ID_NOTE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (w_bRet == false)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetIDNote(int p_nTmplNo, BYTE* pNote)
{
	BOOL		w_bRet = false;
	BYTE		w_abyData[2];
	int			w_nTemplateNo = 0;
	WORD		w_nCmdCks = 0, w_nSize = 0;
	
	//. Assemble command packet
	w_abyData[0] = LOBYTE(p_nTmplNo);
	w_abyData[1] = HIBYTE(p_nTmplNo);
	InitCmdPacket(CMD_GET_ID_NOTE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_GET_ID_NOTE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(w_bRet == false)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	RECEIVE_DATAPACKET(CMD_GET_ID_NOTE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (w_bRet == false)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	memcpy(pNote, &g_pRcmPacket->m_abyData[0], ID_NOTE_SIZE);
	
	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_SetModuleSN(BYTE* pModuleSN)
{
	BOOL	w_bRet = false;
	BYTE	w_abyData[MODULE_SN_LEN];
	WORD	w_wData;
	
	//. Assemble command packet
	w_wData = MODULE_SN_LEN;
	InitCmdPacket(CMD_SET_MODULE_SN, m_bySrcDeviceID, m_byDstDeviceID, (BYTE*)&w_wData, 2);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_SET_MODULE_SN, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(w_bRet == false)
		return ERR_CONNECTION;
	
	if( RESPONSE_RET != ERR_SUCCESS)	
		return RESPONSE_RET;
	
	Sleep(10);
	
	//. Assemble data packet
	memcpy(&w_abyData[0], pModuleSN, MODULE_SN_LEN);
	
	InitCmdDataPacket(CMD_SET_MODULE_SN, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, MODULE_SN_LEN);
	
	//. Send data packet to target
	SEND_DATAPACKET(CMD_SET_MODULE_SN, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (w_bRet == false)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetModuleSN(BYTE* pModuleSN)
{
	BOOL		w_bRet = false;
	
	//. Assemble command packet
	InitCmdPacket(CMD_GET_MODULE_SN, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_GET_MODULE_SN, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(w_bRet == false)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	RECEIVE_DATAPACKET(CMD_GET_MODULE_SN, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (w_bRet == false)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	memcpy(pModuleSN, &g_pRcmPacket->m_abyData[0], MODULE_SN_LEN);
	
	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_SetDevPass(BYTE* pDevPass)
{
	BOOL	w_bRet;

	InitCmdPacket(CMD_SET_DEVPASS, m_bySrcDeviceID, m_byDstDeviceID, pDevPass, MAX_DEVPASS_LEN);

	SEND_COMMAND(CMD_SET_DEVPASS, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_VerfiyDevPass(BYTE* pDevPass)
{
	BOOL	w_bRet;

	InitCmdPacket(CMD_VERIFY_DEVPASS, m_bySrcDeviceID, m_byDstDeviceID, pDevPass, MAX_DEVPASS_LEN);

	SEND_COMMAND(CMD_VERIFY_DEVPASS, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_EnterStandbyState(void)
{
	BOOL	w_bRet;

	InitCmdPacket(CMD_ENTER_STANDY_STATE, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

	SEND_COMMAND(CMD_ENTER_STANDY_STATE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
#define DOWN_FW_DATA_UINT	512
int	CCommunication::Run_UpgradeFirmware(BYTE* p_pData, DWORD p_nSize)
{
	int		i, n, r;
	BOOL	w_bRet;
	BYTE	w_abyData[840];

	w_abyData[0] = LOBYTE(LOWORD(p_nSize));
	w_abyData[1] = HIBYTE(LOWORD(p_nSize));
	w_abyData[2] = LOBYTE(HIWORD(p_nSize));
	w_abyData[3] = HIBYTE(HIWORD(p_nSize));

	//. Assemble command packet
	InitCmdPacket(CMD_UPGRADE_FIRMWARE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);

	SEND_COMMAND(CMD_UPGRADE_FIRMWARE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(!w_bRet)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	n = p_nSize/DOWN_FW_DATA_UINT;
	r = p_nSize%DOWN_FW_DATA_UINT;

	if (m_nConnectionMode == SERIAL_CON_MODE)
	{
		for (i=0; i<n; i++)
		{
			memcpy(&w_abyData[0], &p_pData[i*DOWN_FW_DATA_UINT], DOWN_FW_DATA_UINT);

			InitCmdDataPacket(CMD_UPGRADE_FIRMWARE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, DOWN_FW_DATA_UINT);

			SEND_DATAPACKET(CMD_UPGRADE_FIRMWARE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

			if (w_bRet == false)
				return ERR_CONNECTION;

			if (RESPONSE_RET != ERR_SUCCESS)
				return RESPONSE_RET;			

			if (m_hMainWnd)
				SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i+1)*DOWN_FW_DATA_UINT*100/p_nSize, 2);

			Sleep(6);
		}

		if (r > 0)
		{
			memcpy(&w_abyData[0], &p_pData[i*DOWN_FW_DATA_UINT], r);

			InitCmdDataPacket(CMD_UPGRADE_FIRMWARE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, r);

			SEND_DATAPACKET(CMD_UPGRADE_FIRMWARE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

			if (w_bRet == false)
				return ERR_CONNECTION;

			if (RESPONSE_RET != ERR_SUCCESS)
				return RESPONSE_RET;

			if (m_hMainWnd)
				SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i*DOWN_FW_DATA_UINT+r)*100/p_nSize, 2);
		}
	}
	else if (m_nConnectionMode == USB_CON_MODE)
	{
		w_bRet = USB_DownFirmware(m_hUsbHandle, p_pData, p_nSize);

		if(w_bRet != RT_OK)
			return ERR_CONNECTION;
	}

	return ERR_SUCCESS;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetImage(void)
{
	BOOL	w_bRet;
	
	w_bRet = Run_Command_NP(CMD_GET_IMAGE);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_FingerDetect(int* p_pnDetectResult)
{
	BOOL	w_bRet;
	
	w_bRet = Run_Command_NP(CMD_FINGER_DETECT);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	*p_pnDetectResult = g_pRcmPacket->m_abyData[0];
	
	return ERR_SUCCESS;	
}
/************************************************************************/
/************************************************************************/
#define IMAGE_DATA_UINT	496
int	CCommunication::Run_UpImage(int p_nType, BYTE* p_pFpData, int* p_pnImgWidth, int* p_pnImgHeight)
{
	int		i, n, r, w, h, size;
	BOOL	w_bRet;
	BYTE	w_byData;

	w_byData = p_nType;

	InitCmdPacket(CMD_UP_IMAGE, m_bySrcDeviceID, m_byDstDeviceID, &w_byData, 1);
	
	SEND_COMMAND(CMD_UP_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	w = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	h = MAKEWORD(g_pRcmPacket->m_abyData[2], g_pRcmPacket->m_abyData[3]);
	
	size = w*h;

	n = (size)/IMAGE_DATA_UINT;
	r = (size)%IMAGE_DATA_UINT;

	if (m_nConnectionMode == SERIAL_CON_MODE)
	{
		for (i=0; i<n; i++)
		{
			RECEIVE_DATAPACKET(CMD_UP_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
			
			if(w_bRet == false)
				return ERR_CONNECTION;
			
			if (RESPONSE_RET != ERR_SUCCESS)
				return RESPONSE_RET;
			
			memcpy(&p_pFpData[i*IMAGE_DATA_UINT], &g_pRcmPacket->m_abyData[2], IMAGE_DATA_UINT);
			
			if (m_hMainWnd)
				SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i+1)*IMAGE_DATA_UINT*100/(w*h), 1);
		}
		
		if (r > 0)
		{
			RECEIVE_DATAPACKET(CMD_UP_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
			
			if(w_bRet == false)
				return ERR_CONNECTION;
			
			if (RESPONSE_RET != ERR_SUCCESS)
				return RESPONSE_RET;
			
			memcpy(&p_pFpData[i*IMAGE_DATA_UINT], &g_pRcmPacket->m_abyData[2], r);
			
			if (m_hMainWnd)
				SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i*IMAGE_DATA_UINT+r)*100/(w*h), 1);
		}	
	}
	else if (m_nConnectionMode == USB_CON_MODE)
	{
		w_bRet = USB_ReceiveImage(m_hUsbHandle, p_pFpData, w*h, p_nType);
		
		if(w_bRet == false)
			return ERR_CONNECTION;
	}

	*p_pnImgWidth = w;
	*p_pnImgHeight = h;
	
	return ERR_SUCCESS;	
}
/************************************************************************/
/************************************************************************/
#define DOWN_IMAGE_DATA_UINT	496
int	CCommunication::Run_DownImage(BYTE* p_pData, int p_nWidth, int p_nHeight)
{
	int		i, n, r, w, h;
	BOOL	w_bRet;
	BYTE	w_abyData[840];
	
	w = p_nWidth;
	h = p_nHeight;

	w_abyData[0] = LOBYTE(p_nWidth);
	w_abyData[1] = HIBYTE(p_nWidth);
	w_abyData[2] = LOBYTE(p_nHeight);
	w_abyData[3] = HIBYTE(p_nHeight);

	//. Assemble command packet
	InitCmdPacket(CMD_DOWN_IMAGE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	SEND_COMMAND(CMD_DOWN_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	n = (w*h)/DOWN_IMAGE_DATA_UINT;
	r = (w*h)%DOWN_IMAGE_DATA_UINT;

	if (m_nConnectionMode == SERIAL_CON_MODE)
	{
		for (i=0; i<n; i++)
		{
			w_abyData[0] = LOBYTE(i);
			w_abyData[1] = HIBYTE(i);
			memcpy(&w_abyData[2], &p_pData[i*DOWN_IMAGE_DATA_UINT], DOWN_IMAGE_DATA_UINT);
			
			InitCmdDataPacket(CMD_DOWN_IMAGE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2+DOWN_IMAGE_DATA_UINT);

			SEND_DATAPACKET(CMD_DOWN_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
			
			if (w_bRet == false)
				return ERR_CONNECTION;
			
			if (RESPONSE_RET != ERR_SUCCESS)
				return RESPONSE_RET;			
			
			if (m_hMainWnd)
				SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i+1)*DOWN_IMAGE_DATA_UINT*100/(w*h), 0);

			Sleep(6);
		}
		
		if (r > 0)
		{
			w_abyData[0] = LOBYTE(i);
			w_abyData[1] = HIBYTE(i);
			memcpy(&w_abyData[2], &p_pData[i*DOWN_IMAGE_DATA_UINT], r);
			
			InitCmdDataPacket(CMD_DOWN_IMAGE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2+r);

			SEND_DATAPACKET(CMD_DOWN_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
			
			if (w_bRet == false)
				return ERR_CONNECTION;
			
			if (RESPONSE_RET != ERR_SUCCESS)
				return RESPONSE_RET;
			
			if (m_hMainWnd)
				SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i*DOWN_IMAGE_DATA_UINT+r)*100/(w*h), 0);
		}
	}
	else if (m_nConnectionMode == USB_CON_MODE)
	{
		w_bRet = USB_DownImage(m_hUsbHandle, p_pData, p_nWidth*p_nHeight);
		
		if(w_bRet != RT_OK)
			return ERR_CONNECTION;
	}

	return ERR_SUCCESS;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_SLEDControl(int p_nState)
{
	BOOL	w_bRet;
	BYTE	w_abyData[2];
	
	w_abyData[0] = LOBYTE(p_nState);
	w_abyData[1] = HIBYTE(p_nState);
	
	InitCmdPacket(CMD_SLED_CTRL, m_bySrcDeviceID, m_byDstDeviceID, (BYTE*)&w_abyData, 2);
	
	SEND_COMMAND(CMD_SLED_CTRL, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_AdjustSensor(void)
{
	BOOL	w_bRet;

	InitCmdPacket(CMD_ADJUST_SENSOR, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

	SEND_COMMAND(CMD_ADJUST_SENSOR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_StoreChar(int p_nTmplNo, int p_nRamBufferID, int* p_pnDupTmplNo)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];
	
	w_abyData[0] = LOBYTE(p_nTmplNo);
	w_abyData[1] = HIBYTE(p_nTmplNo);
	w_abyData[2] = LOBYTE(p_nRamBufferID);
	w_abyData[3] = HIBYTE(p_nRamBufferID);
	
	InitCmdPacket(CMD_STORE_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_STORE_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
	{
		if (RESPONSE_RET == ERR_DUPLICATION_ID)
			*p_pnDupTmplNo = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);

		return RESPONSE_RET;
	}
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_LoadChar(int p_nTmplNo, int p_nRamBufferID)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];
	
	w_abyData[0] = LOBYTE(p_nTmplNo);
	w_abyData[1] = HIBYTE(p_nTmplNo);
	w_abyData[2] = LOBYTE(p_nRamBufferID);
	w_abyData[3] = HIBYTE(p_nRamBufferID);
	
	InitCmdPacket(CMD_LOAD_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_LOAD_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
#define	CHAR_SPLIT_UNIT			(496)
#define	CHAR_SPLIT_THR			(1024)
int	CCommunication::Run_UpChar(int p_nRamBufferID, BYTE* p_pbyTemplate, unsigned int *p_pnSize)
{
	BOOL		w_bRet = false;
	BYTE		w_abyData[2];
	int			w_nTemplateNo = 0;
	WORD		w_nCmdCks = 0, w_nSize = 0;
	WORD		w_nTemplateSize = 0;
	int			w_nRemainSize = 0;
	
	//. Assemble command packet
	w_abyData[0] = LOBYTE(p_nRamBufferID);
	w_abyData[1] = HIBYTE(p_nRamBufferID);
	InitCmdPacket(CMD_UP_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_UP_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(w_bRet == false)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	w_nTemplateSize = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	
	if (w_nTemplateSize < CHAR_SPLIT_THR)
	{
		RECEIVE_DATAPACKET(CMD_UP_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
		if (w_bRet == false)
			return ERR_CONNECTION;
		if (RESPONSE_RET != ERR_SUCCESS)
			return RESPONSE_RET;
		memcpy(p_pbyTemplate, &g_pRcmPacket->m_abyData[0], w_nTemplateSize);
	}
	else
	{
		w_nRemainSize = w_nTemplateSize;
		while (w_nRemainSize > 0)
		{
			RECEIVE_DATAPACKET(CMD_UP_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
			if (w_bRet == false)
				return ERR_CONNECTION;
			if (RESPONSE_RET != ERR_SUCCESS)
				return RESPONSE_RET;
			if (w_nRemainSize > CHAR_SPLIT_UNIT)
			{
				memcpy(&p_pbyTemplate[w_nTemplateSize - w_nRemainSize], &g_pRcmPacket->m_abyData[0], CHAR_SPLIT_UNIT);
				w_nRemainSize -= CHAR_SPLIT_UNIT;
			}
			else
			{
				memcpy(&p_pbyTemplate[w_nTemplateSize - w_nRemainSize], &g_pRcmPacket->m_abyData[0], w_nRemainSize);
				w_nRemainSize = 0;
			}
		}
	}

	*p_pnSize = w_nTemplateSize;
	
	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_DownChar(int p_nRamBufferID, BYTE* p_pbyTemplate, unsigned int p_nSize)
{
	BOOL	w_bRet = false;
	BYTE	w_abyData[MAX_TEMPLATE_SIZE + 2];
	WORD	w_wData;
	int		w_nRemainSize = 0;
	int		i;
	
	//. Assemble command packet
	if (p_nSize < CHAR_SPLIT_THR)
		w_wData = p_nSize + 2;
	else
		w_wData = p_nSize + 4;
	InitCmdPacket(CMD_DOWN_CHAR, m_bySrcDeviceID, m_byDstDeviceID, (BYTE*)&w_wData, 2);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_DOWN_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (w_bRet == false)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	Sleep(10);
	
	//. Assemble data packet
	if (p_nSize < CHAR_SPLIT_THR)
	{
		w_abyData[0] = LOBYTE(p_nRamBufferID);
		w_abyData[1] = HIBYTE(p_nRamBufferID);
		memcpy(&w_abyData[2], p_pbyTemplate, p_nSize);
	
		InitCmdDataPacket(CMD_DOWN_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, p_nSize + 2);
	
		//. Send data packet to target
		SEND_DATAPACKET(CMD_DOWN_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
		if (w_bRet == false)
			return ERR_CONNECTION;
	}
	else
	{
		w_nRemainSize = p_nSize;
		i = 0;
		while (w_nRemainSize > 0)
		{
			w_abyData[0] = LOBYTE(p_nRamBufferID);
			w_abyData[1] = HIBYTE(p_nRamBufferID);
			w_abyData[2] = LOBYTE(i);
			w_abyData[3] = HIBYTE(i);
			if (w_nRemainSize > CHAR_SPLIT_UNIT)
			{
				memcpy(&w_abyData[4], &p_pbyTemplate[i * CHAR_SPLIT_UNIT], CHAR_SPLIT_UNIT);
				InitCmdDataPacket(CMD_DOWN_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, CHAR_SPLIT_UNIT + 4);
				w_nRemainSize -= CHAR_SPLIT_UNIT;
			}
			else
			{
				memcpy(&w_abyData[4], &p_pbyTemplate[i * CHAR_SPLIT_UNIT], w_nRemainSize);
				InitCmdDataPacket(CMD_DOWN_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, w_nRemainSize + 4);
				w_nRemainSize = 0;
			}
	
			//. Send data packet to target
			SEND_DATAPACKET(CMD_DOWN_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
			if (w_bRet == false)
				return ERR_CONNECTION;

			i++;
		}
	}
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int CCommunication::Run_DelChar(int p_nSTmplNo, int p_nETmplNo)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];
	
	w_abyData[0] = LOBYTE(p_nSTmplNo);
	w_abyData[1] = HIBYTE(p_nSTmplNo);
	w_abyData[2] = LOBYTE(p_nETmplNo);
	w_abyData[3] = HIBYTE(p_nETmplNo);
	
	//. Assemble command packet
	InitCmdPacket(CMD_DEL_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	SEND_COMMAND(CMD_DEL_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	return RESPONSE_RET;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetEmptyID(int p_nSTmplNo, int p_nETmplNo, int* p_pnEmptyID)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];

	w_abyData[0] = LOBYTE(p_nSTmplNo);
	w_abyData[1] = HIBYTE(p_nSTmplNo);
	w_abyData[2] = LOBYTE(p_nETmplNo);
	w_abyData[3] = HIBYTE(p_nETmplNo);

	//. Assemble command packet
	InitCmdPacket(CMD_GET_EMPTY_ID, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	SEND_COMMAND(CMD_GET_EMPTY_ID, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	if ( RESPONSE_RET != ERR_SUCCESS )	
		return RESPONSE_RET;
	
	*p_pnEmptyID = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	
	return ERR_SUCCESS;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetStatus(int p_nTmplNo, int* p_pnStatus)
{
	BOOL	w_bRet;
	BYTE	w_abyData[2];
	
	w_abyData[0] = LOBYTE(p_nTmplNo);
	w_abyData[1] = HIBYTE(p_nTmplNo);
	
	InitCmdPacket(CMD_GET_STATUS, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2);
	
	SEND_COMMAND(CMD_GET_STATUS, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	if ( RESPONSE_RET != ERR_SUCCESS )	
		return RESPONSE_RET;
	
	*p_pnStatus = g_pRcmPacket->m_abyData[0];
	
	return ERR_SUCCESS;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetBrokenID(int p_nSTmplNo, int p_nETmplNo, int* p_pnCount, int* p_pnFirstID)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];
	
	w_abyData[0] = LOBYTE(p_nSTmplNo);
	w_abyData[1] = HIBYTE(p_nSTmplNo);
	w_abyData[2] = LOBYTE(p_nETmplNo);
	w_abyData[3] = HIBYTE(p_nETmplNo);

	//. Assemble command packet
	InitCmdPacket(CMD_GET_BROKEN_ID, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	SEND_COMMAND(CMD_GET_BROKEN_ID, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet)
		return ERR_CONNECTION;
	
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	*p_pnCount		= MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	*p_pnFirstID	= MAKEWORD(g_pRcmPacket->m_abyData[2], g_pRcmPacket->m_abyData[3]);
	
	return ERR_SUCCESS;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_GetEnrollCount(int p_nSTmplNo, int p_nETmplNo, int* p_pnEnrollCount)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];
	
	w_abyData[0] = LOBYTE(p_nSTmplNo);
	w_abyData[1] = HIBYTE(p_nSTmplNo);
	w_abyData[2] = LOBYTE(p_nETmplNo);
	w_abyData[3] = HIBYTE(p_nETmplNo);
	
	//. Assemble command packet
	InitCmdPacket(CMD_GET_ENROLL_COUNT, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	SEND_COMMAND(CMD_GET_ENROLL_COUNT, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	if(RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	*p_pnEnrollCount = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	
	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
#define 	VALID_FLAG_BIT_CHECK(A, V)		(A[0 + V/8] & (0x01 << (V & 0x07)))
int	CCommunication::Run_GetEnrolledIDList(int* p_pnCount, int* p_pnIDs)
{
	int			i, w_nValidBufSize;
	BOOL		w_bRet = false;	
	BYTE*		w_pValidBuf;

	InitCmdPacket(CMD_GET_ENROLLED_ID_LIST, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

	//. Send command packet to target
	SEND_COMMAND(CMD_GET_ENROLLED_ID_LIST, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(w_bRet == false)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	w_nValidBufSize = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);

	RECEIVE_DATAPACKET(CMD_GET_ENROLLED_ID_LIST, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (w_bRet == false)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	w_pValidBuf = new BYTE[w_nValidBufSize];

	memcpy(w_pValidBuf, &g_pRcmPacket->m_abyData[0], w_nValidBufSize);

	*p_pnCount = 0;
	for (i=1; i<(w_nValidBufSize*8); i++)
	{
		if (VALID_FLAG_BIT_CHECK(w_pValidBuf, i) != 0)
		{
			p_pnIDs[*p_pnCount] = i;
			*p_pnCount = *p_pnCount + 1;
		}
		if (i >= g_nMaxFpCount)
			break;
	}

	delete[] w_pValidBuf;

	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_Generate(int p_nRamBufferID)
{
	BOOL	w_bRet;
	BYTE	w_abyData[2];
	
	w_abyData[0] = LOBYTE(p_nRamBufferID);
	w_abyData[1] = HIBYTE(p_nRamBufferID);
	
	InitCmdPacket(CMD_GENERATE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_GENERATE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if(!w_bRet) 
		return ERR_CONNECTION;
	
	return RESPONSE_RET;		
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_Merge(int p_nRamBufferID, int p_nMergeCount)
{
	BOOL	w_bRet;
	BYTE	w_abyData[3];
	
	w_abyData[0] = LOBYTE(p_nRamBufferID);
	w_abyData[1] = HIBYTE(p_nRamBufferID);
	w_abyData[2] = p_nMergeCount;
	
	InitCmdPacket(CMD_MERGE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 3);
	
	SEND_COMMAND(CMD_MERGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet) 
		return ERR_CONNECTION;
	
	return RESPONSE_RET;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_Match(int p_nRamBufferID0, int p_nRamBufferID1, int* p_pnLearnResult)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];
	
	w_abyData[0] = LOBYTE(p_nRamBufferID0);
	w_abyData[1] = HIBYTE(p_nRamBufferID0);
	w_abyData[2] = LOBYTE(p_nRamBufferID1);
	w_abyData[3] = HIBYTE(p_nRamBufferID1);
	
	InitCmdPacket(CMD_MATCH, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	SEND_COMMAND(CMD_MATCH, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet) 
		return ERR_CONNECTION;

	if(RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	*p_pnLearnResult	= g_pRcmPacket->m_abyData[0];

	return RESPONSE_RET;	
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_Search(int p_nRamBufferID, int p_nStartID, int p_nSearchCount, int* p_pnTmplNo, int* p_pnLearnResult)
{
	BOOL	w_bRet;
	BYTE	w_abyData[6];
	
	w_abyData[0] = LOBYTE(p_nRamBufferID);
	w_abyData[1] = HIBYTE(p_nRamBufferID);
	w_abyData[2] = LOBYTE(p_nStartID);
	w_abyData[3] = HIBYTE(p_nStartID);
	w_abyData[4] = LOBYTE(p_nSearchCount);
	w_abyData[5] = HIBYTE(p_nSearchCount);
	
	InitCmdPacket(CMD_SEARCH, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 6);
	
	//. Send command packet to target
	SEND_COMMAND(CMD_SEARCH, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	if(RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	
	*p_pnTmplNo			= MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	*p_pnLearnResult	= g_pRcmPacket->m_abyData[2];
	
	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	CCommunication::Run_Verify(int p_nTmplNo, int p_nRamBufferID, int* p_pnLearnResult)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];
	
	w_abyData[0] = LOBYTE(p_nTmplNo);
	w_abyData[1] = HIBYTE(p_nTmplNo);
	w_abyData[2] = LOBYTE(p_nRamBufferID);
	w_abyData[3] = HIBYTE(p_nRamBufferID);
	
	//. Assemble command packet
	InitCmdPacket(CMD_VERIFY, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);
	
	SEND_COMMAND(CMD_VERIFY, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	
	if (!w_bRet)
		return ERR_CONNECTION;
	
	*p_pnLearnResult = g_pRcmPacket->m_abyData[2];
	
	return RESPONSE_RET;
}
int CCommunication::Run_SetRTC(ST_RTC_TYPE p_stRTCInfo)
{
	BOOL	w_bRet;

	//. Assemble command packet
	InitCmdPacket(CMD_SET_RTC, m_bySrcDeviceID, m_byDstDeviceID, (unsigned char*)&p_stRTCInfo, sizeof(ST_RTC_TYPE));

	SEND_COMMAND(CMD_SET_RTC, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	if (!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}
int CCommunication::Run_GetRTC(ST_RTC_TYPE *p_stRTCInfo)
{
	BOOL	w_bRet;

	if (p_stRTCInfo == NULL)
		return ERR_INVALID_PARAM;

	InitCmdPacket(CMD_GET_RTC, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

	//. Send command packet to target
	SEND_COMMAND(CMD_GET_RTC, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	if(RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	memcpy((unsigned char*)p_stRTCInfo, g_pRcmPacket->m_abyData, sizeof(ST_RTC_TYPE));

	return RESPONSE_RET;
}
int CCommunication::Run_GetOEMRSAPubKey(BYTE* p_pCryptRemoteN, BYTE* p_pCryptRemoteE, int *p_nKeySizeByte)
{
	BOOL w_bRet = false;
	WORD w_nKeySize = 0;

	// check parameter
	if ((p_pCryptRemoteN == NULL) ||
		(p_pCryptRemoteE == NULL) ||
		(p_nKeySizeByte == NULL))
		return ERR_INVALID_PARAM;

	// assemble command packet
	InitCmdPacket(CMD_GET_OEM_RSA_PUB_KEY, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

	// send command packet to target
	SEND_COMMAND(CMD_GET_OEM_RSA_PUB_KEY, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	if (w_bRet == false)
		return ERR_CONNECTION;
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	// check key size
	w_nKeySize = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	if (w_nKeySize != CRYPT_DEF_RSA_BYTE * 2)
		return ERR_INVALID_PARAM;

	RECEIVE_DATAPACKET(CMD_GET_OEM_RSA_PUB_KEY, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	if (w_bRet == false)
		return ERR_CONNECTION;
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	memcpy(p_pCryptRemoteN, &g_Packet[10 + (0 * CRYPT_DEF_RSA_BYTE)], CRYPT_DEF_RSA_BYTE);
	memcpy(p_pCryptRemoteE, &g_Packet[10 + (1 * CRYPT_DEF_RSA_BYTE)], CRYPT_DEF_RSA_BYTE);
	*p_nKeySizeByte = CRYPT_DEF_RSA_BYTE;

	return ERR_SUCCESS;
}
int CCommunication::Run_SetHostRSAPubKey(BYTE* p_pCryptN, BYTE* p_pCryptE, int p_nKeySizeByte)
{
	BOOL w_bRet = false;
	BYTE w_abyData[CRYPT_DEF_RSA_BYTE * 2];
	WORD w_wData;

	// check parameters
	if ((p_pCryptN == NULL) ||
		(p_pCryptE == NULL) ||
		(p_nKeySizeByte != CRYPT_DEF_RSA_BYTE))
		return ERR_INVALID_PARAM;

	// assemble command packet
	w_wData = p_nKeySizeByte * 2 + 2;
	InitCmdPacket(CMD_SET_HOST_RSA_PUB_KEY, m_bySrcDeviceID, m_byDstDeviceID, (BYTE*)&w_wData, 2);

	// send command packet to target
	SEND_COMMAND(CMD_SET_HOST_RSA_PUB_KEY, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	if (w_bRet == false)
		return ERR_CONNECTION;
	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	Sleep(10);

	// assemble data packet
	memcpy(&w_abyData[0 * CRYPT_DEF_RSA_BYTE], p_pCryptN, p_nKeySizeByte);
	memcpy(&w_abyData[1 * CRYPT_DEF_RSA_BYTE], p_pCryptE, p_nKeySizeByte);
	InitCmdDataPacket(CMD_SET_HOST_RSA_PUB_KEY, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, p_nKeySizeByte * 2);

	// send data packet to target
	SEND_DATAPACKET(CMD_SET_HOST_RSA_PUB_KEY, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	if (w_bRet == false)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}
/***************************************************************************/
/***************************************************************************/
BOOL CCommunication::OpenSerialPort(CString p_strComPortIndex, int p_nBaudRateIndex)
{
	LONG    w_lRetCode = ERROR_SUCCESS;
	
	ASSERT(p_nBaudRateIndex >= 0);
	ASSERT(p_nBaudRateIndex <= BAUD921600);
	
	p_strComPortIndex = _T("\\\\.\\") + p_strComPortIndex;

	//. Attempt to open the serial port (COM1)
	w_lRetCode = g_Serial.Open(p_strComPortIndex, 2048, 2048,true);
	if (w_lRetCode != ERROR_SUCCESS){
		return false;
	}
	
	//. Setup the serial port (9600,8N1, which is the default setting)
	w_lRetCode = g_Serial.Setup((CSerial::EBaudrate)BAUDRATES[p_nBaudRateIndex], CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (w_lRetCode != ERROR_SUCCESS){
		return false;
	}
	
	//. Register only for the receive event
	w_lRetCode = g_Serial.SetMask(CSerial::EEventRecv);
	if (w_lRetCode != ERROR_SUCCESS){
		return false;
	}
	return true;
}
/************************************************************************
*      Test Connection with Target
************************************************************************/
BOOL CCommunication::EnableCommunicaton(int p_nDeviceID, BOOL p_bVerifyDeviceID, BYTE* p_pDevPwd, BOOL p_bMsgOut)
{
	return true;	
}
/************************************************************************
     p_nConMode = 0 : Open Serial Port
	 p_nParam1	= ComPort Index 
	 p_nParam2	= BaudRate Index
************************************************************************/
int CCommunication::InitConnection(int p_nConMode, CString p_strComPort, int p_nBaudRate, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	BOOL	w_blRet = false;
	CString w_strMsg, w_szDestIP;
	
	g_Serial.Close();
	
	m_bySrcDeviceID = p_bySrcDeviceID;
	m_byDstDeviceID = p_byDstDeviceID;

	m_nConnectionMode = p_nConMode;
	
	if (p_nConMode == SERIAL_CON_MODE)
	{
		w_blRet = OpenSerialPort(p_strComPort, p_nBaudRate);
		
		if( w_blRet != TRUE )
		{	
 			w_strMsg.Format(_T("Failed to open %s port!"), p_strComPort);
			AfxMessageBox(w_strMsg);
			CloseConnection();
			return ERR_COM_OPEN_FAIL;
		}
	}
	else if (p_nConMode == USB_CON_MODE)
	{
		if ( OpenUSB( &m_hUsbHandle, p_byDstDeviceID ) == FALSE )
		{
			AfxMessageBox(_T("Failed to open USB port!"));
			CloseConnection();
			return ERR_USB_OPEN_FAIL;
		}
	}

	return CONNECTION_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
BOOL CCommunication::Run_Command_NP( WORD p_wCMD )
{
	BOOL w_bRet;

	//. Assemble command packet
	InitCmdPacket(p_wCMD, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

 	SEND_COMMAND(p_wCMD, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	return w_bRet;
}
/***************************************************************************/
/***************************************************************************/
void CCommunication::CloseConnection() 
{
	if (m_nConnectionMode == SERIAL_CON_MODE)
	{
		//. Close serial port
		g_Serial.Close();
	}
	else if (m_nConnectionMode == USB_CON_MODE)
	{
		
	}
}
/***************************************************************************/
/***************************************************************************/
void CCommunication::SetIPandPort(CString strDestination, DWORD dwPort)
{
	m_strDestIp = strDestination;
	m_dwPort	= dwPort;		 
}
/***************************************************************************/
/***************************************************************************/
void CCommunication::SetCallbackWnd(HWND hWnd)
{
	m_hMainWnd = hWnd;
}