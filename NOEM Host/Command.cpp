/********************************************************************
	created:	2012/01/26
	file base:	Command
	file ext:	cpp
	author:		SYFP Team
	
	purpose:	통신파케트 관련 구조체 및 함수들을 정의한다.
*********************************************************************/

#include "stdafx.h"
#include "Define.h"
#include "Command.h"

#include "crypt_user.h"

extern HWND	g_hMainWnd;

CSerial			g_Serial;
DWORD			g_dwPacketSize = 0;
BYTE			g_Packet[1024*64];
PST_CMD_PACKET	g_pCmdPacket = (PST_CMD_PACKET)g_Packet;
PST_RCM_PACKET	g_pRcmPacket = (PST_RCM_PACKET)g_Packet;

/***************************************************************************/
/***************************************************************************/
WORD GetCheckSum(BOOL bCmdData)
{
	WORD	w_Ret = 0;
	
	//w_Ret = g_pCmdPacket->;

	return w_Ret;
}
/***************************************************************************/
/***************************************************************************/
BOOL CheckReceive( BYTE* p_pbyPacket, DWORD p_dwPacketLen, WORD p_wPrefix, WORD p_wCMDCode )
{
	int				i;
	WORD			w_wCalcCheckSum, w_wCheckSum;
	ST_RCM_PACKET*	w_pstRcmPacket;

	w_pstRcmPacket = (ST_RCM_PACKET*)p_pbyPacket;

	//. Check prefix code
 	if (p_wPrefix != w_pstRcmPacket->m_wPrefix)
 		return FALSE;
 	
	//. Check checksum
	w_wCheckSum = MAKEWORD(p_pbyPacket[p_dwPacketLen-2], p_pbyPacket[p_dwPacketLen-1]);
	w_wCalcCheckSum = 0;
	for (i=0; i<(int)p_dwPacketLen-2; i++)
	{
		w_wCalcCheckSum = w_wCalcCheckSum + p_pbyPacket[i];
	}
	
	if (w_wCheckSum != w_wCalcCheckSum)
		return FALSE;
	
	if (p_wCMDCode != w_pstRcmPacket->m_wCMDCode)
	{
		return FALSE;
	}

	return TRUE;
}
/***************************************************************************/
/***************************************************************************/
void InitCmdPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID, BYTE* p_pbyData, WORD p_wDataLen)
{
	int		i;
	WORD	w_wCheckSum;

	memset( g_Packet, 0, sizeof( g_Packet ));
	g_pCmdPacket->m_wPrefix = CMD_PREFIX_CODE;
	g_pCmdPacket->m_bySrcDeviceID = p_bySrcDeviceID;
	g_pCmdPacket->m_byDstDeviceID = p_byDstDeviceID;
	g_pCmdPacket->m_wCMDCode = p_wCMDCode;
	g_pCmdPacket->m_wDataLen = p_wDataLen;

	if (p_wDataLen)
		memcpy(g_pCmdPacket->m_abyData, p_pbyData, p_wDataLen);

	w_wCheckSum = 0;

	for (i=0; i<sizeof(ST_CMD_PACKET)-2; i++)
	{
		w_wCheckSum = w_wCheckSum + g_Packet[i];
	}

	g_pCmdPacket->m_wCheckSum = w_wCheckSum;
	
	g_dwPacketSize = sizeof(ST_CMD_PACKET);
}
/***************************************************************************/
/***************************************************************************/
void	InitCmdDataPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID, BYTE* p_pbyData, WORD p_wDataLen)
{
	int		i;
	WORD	w_wCheckSum;

	g_pCmdPacket->m_wPrefix = CMD_DATA_PREFIX_CODE;
	g_pCmdPacket->m_bySrcDeviceID = p_bySrcDeviceID;
	g_pCmdPacket->m_byDstDeviceID = p_byDstDeviceID;
	g_pCmdPacket->m_wCMDCode = p_wCMDCode;
	g_pCmdPacket->m_wDataLen = p_wDataLen;

	memcpy(&g_pCmdPacket->m_abyData[0], p_pbyData, p_wDataLen);

	//. Set checksum
	w_wCheckSum = 0;
	
	for (i=0; i<(p_wDataLen + 8); i++)
	{
		w_wCheckSum = w_wCheckSum + g_Packet[i];
	}

	g_Packet[p_wDataLen+8] = LOBYTE(w_wCheckSum);
	g_Packet[p_wDataLen+9] = HIBYTE(w_wCheckSum);

	g_dwPacketSize = p_wDataLen + 10;
}
/***************************************************************************/
/***************************************************************************/
BOOL SendCommand(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	DWORD	w_nSendCnt = 0;
	LONG	w_nResult = 0;
	int		w_nRet;
	BOOL	w_bRet;

	g_Serial.Purge();

	::SendMessage(g_hMainWnd, WM_CMD_PACKET_HOOK, 0, 0);

	// encrypt packet
	w_nRet = EncryptCommandPacket();

	w_nResult = g_Serial.Write(g_Packet, g_dwPacketSize , &w_nSendCnt, NULL, COMM_TIMEOUT);
	
	if(ERROR_SUCCESS != w_nResult)
	{
		return FALSE;
	}

	if (w_nRet == ERR_SUCCESS)
	{
		w_bRet = ReceiveDataAck(p_wCMDCode, p_bySrcDeviceID, p_byDstDeviceID);
	}
	else
	{
		w_bRet = ReceiveAck(p_wCMDCode, p_bySrcDeviceID, p_byDstDeviceID);
	}

	return w_bRet;
}
/***************************************************************************/
/***************************************************************************/
BOOL ReceiveAck(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	DWORD	w_nAckCnt = 0;
	LONG	w_nResult = 0;
	DWORD	w_dwTimeOut = COMM_TIMEOUT;

	if (p_wCMDCode == CMD_TEST_CONNECTION)
		w_dwTimeOut = 2000;
	else if (p_wCMDCode == CMD_ADJUST_SENSOR)
		w_dwTimeOut = 30000;
	
l_read_packet:

	//w_nResult = g_Serial.Read(g_Packet, sizeof(ST_RCM_PACKET), &w_nAckCnt, NULL, w_dwTimeOut);	
	if (!ReadDataN(g_Packet, sizeof(ST_RCM_PACKET), w_dwTimeOut))
	{
		return FALSE;
	}

	g_dwPacketSize = sizeof(ST_RCM_PACKET);	

	::SendMessage(g_hMainWnd, WM_RCM_PACKET_HOOK, 0, 0);

	if (!CheckReceive(g_Packet, sizeof(ST_RCM_PACKET), RCM_PREFIX_CODE, p_wCMDCode))
		return FALSE;
	
	if (g_pCmdPacket->m_byDstDeviceID != p_bySrcDeviceID)
	{
		goto l_read_packet;
	}

	return TRUE;
}
/***************************************************************************/
/***************************************************************************/
BOOL ReceiveDataAck(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	DWORD	w_nAckCnt = 0;
	LONG	w_nResult = 0;
	DWORD	w_dwTimeOut = COMM_TIMEOUT;
	int		w_nRet;
	BOOL	w_bRet;
	
	//w_nResult = g_Serial.Read(g_Packet, 8, &w_nAckCnt, NULL, w_dwTimeOut);	
	
	if (!ReadDataN(g_Packet, 8, w_dwTimeOut))
	{
		::MessageBox(NULL, _T("Please check connection with device"), _T("Err"), MB_ICONWARNING );
		return false;
	}
	
	//w_nResult = g_Serial.Read(g_Packet+8, g_pRcmPacket->m_wDataLen+2, &w_nAckCnt, NULL, w_dwTimeOut);	
	if (!ReadDataN(g_Packet+8, g_pRcmPacket->m_wDataLen+2, w_dwTimeOut))
	{
		::MessageBox(NULL, _T("Please check connection with device"), _T("Err"), MB_ICONWARNING );
		return false;
	}

	// decrypt packet
	w_nRet = DecryptCommandPacket();
	if (w_nRet == ERR_SUCCESS)
	{
		if (g_pRcmPacket->m_wPrefix == RCM_PREFIX_CODE)
			g_dwPacketSize = sizeof(ST_CMD_PACKET);
		else if (g_pRcmPacket->m_wPrefix == RCM_DATA_PREFIX_CODE)
			g_dwPacketSize = g_pRcmPacket->m_wDataLen + 10;
		else
			return false;
	
		::SendMessage(g_hMainWnd, WM_RCM_PACKET_HOOK, 0, 0);

		w_bRet = CheckReceive(g_Packet, g_dwPacketSize, g_pRcmPacket->m_wPrefix, p_wCMDCode);
	}
	else
	{
		::SendMessage(g_hMainWnd, WM_RCM_PACKET_HOOK, 0, 0);

		w_bRet = CheckReceive(g_Packet, g_pRcmPacket->m_wDataLen + 10, RCM_DATA_PREFIX_CODE, p_wCMDCode);
	}

	return w_bRet;
}
/***************************************************************************/
/***************************************************************************/
BOOL SendDataPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	DWORD	w_nSendCnt = 0;
	LONG	w_nResult = 0;
	
	::SendMessage(g_hMainWnd, WM_CMD_PACKET_HOOK, 0, 0);

	EncryptCommandPacket();
	
	w_nResult = g_Serial.Write(g_Packet, g_dwPacketSize , &w_nSendCnt, NULL, COMM_TIMEOUT);
	
	if(ERROR_SUCCESS != w_nResult)
	{
		::MessageBox(NULL, _T("Fail in sending a command packet !"), _T("Communication Error"), MB_ICONWARNING );
		return FALSE;
	}
	
	return ReceiveDataAck(p_wCMDCode, p_bySrcDeviceID, p_byDstDeviceID);
}
/***************************************************************************/
/***************************************************************************/
BOOL ReceiveDataPacket(WORD	p_wCMDCode, BYTE p_byDataLen, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	return ReceiveDataAck(p_wCMDCode, p_bySrcDeviceID, p_byDstDeviceID);
}
/***************************************************************************/
/***************************************************************************/
BOOL ReadDataN(BYTE* p_pData, int p_nLen, DWORD p_dwTimeOut)
{
	DWORD	w_nAckCnt = 0;
	LONG	w_nResult = 0;
	int		w_nRecvLen, w_nTotalRecvLen;

	w_nRecvLen = p_nLen;
	w_nTotalRecvLen = 0;

	while(w_nTotalRecvLen < p_nLen )
	{
		w_nResult = g_Serial.Read(&p_pData[w_nTotalRecvLen], w_nRecvLen, &w_nAckCnt, NULL, p_dwTimeOut);	

		if ( ERROR_SUCCESS != w_nResult )
		{
			return FALSE;
		}

		w_nRecvLen = w_nRecvLen - w_nAckCnt;
		w_nTotalRecvLen = w_nTotalRecvLen + w_nAckCnt;
	}

	return TRUE;
}
/***************************************************************************/
/***************************************************************************/
CString GetErrorMsg(DWORD p_dwErrorCode)
{
	CString w_ErrMsg;

	switch(LOBYTE(p_dwErrorCode))
	{
	case ERROR_SUCCESS:
		w_ErrMsg = _T("Succcess");
		break;
	case ERR_VERIFY:
		w_ErrMsg = _T("Verify NG");
		break;
	case ERR_IDENTIFY:
		w_ErrMsg = _T("Identify NG");
		break;
	case ERR_EMPTY_ID_NOEXIST:
		w_ErrMsg = _T("Empty Template no Exist");
		break;
	case ERR_BROKEN_ID_NOEXIST:
		w_ErrMsg = _T("Broken Template no Exist");
		break;
	case ERR_TMPL_NOT_EMPTY:
		w_ErrMsg = _T("Template of this ID Already Exist");
		break;
	case ERR_TMPL_EMPTY:
		w_ErrMsg = _T("This Template is Already Empty");
		break;
	case ERR_INVALID_TMPL_NO:
		w_ErrMsg = _T("Invalid Template No");
		break;
	case ERR_ALL_TMPL_EMPTY:
		w_ErrMsg = _T("All Templates are Empty");
		break;
	case ERR_INVALID_TMPL_DATA:
		w_ErrMsg = _T("Invalid Template Data");
		break;
	case ERR_DUPLICATION_ID:
		w_ErrMsg = _T("Duplicated ID : ");
		break;
	case ERR_BAD_QUALITY:
		w_ErrMsg = _T("Bad Quality Image");
		break;
	case ERR_MERGE_FAIL:
		w_ErrMsg = _T("Merge failed");
		break;
	case ERR_NOT_AUTHORIZED:
		w_ErrMsg = _T("Device not authorized.");
		break;
	case ERR_MEMORY:
		w_ErrMsg = _T("Memory Error ");
		break;
	case ERR_INVALID_PARAM:
		w_ErrMsg = _T("Invalid Parameter");
		break;
	case ERR_GEN_COUNT:
		w_ErrMsg = _T("Generation Count is invalid");
		break;
	case ERR_INVALID_BUFFER_ID:
		w_ErrMsg = _T("Ram Buffer ID is invalid.");
		break;
	case ERR_INVALID_OPERATION_MODE:
		w_ErrMsg = _T("Invalid Operation Mode!");
		break;
	case ERR_FP_NOT_DETECTED:
		w_ErrMsg = _T("Finger is not detected.");
		break;
	case ERR_INPUT_CORRECT_FP:
		w_ErrMsg = _T("Finger rolled too much");
		break;
	case ERR_MOVE_FINGER:
		w_ErrMsg = _T("Press other side of finger");
		break;
	case ERR_LEAK_TEMPLATE_INFO:
		w_ErrMsg = _T("Template Information Leak");
		break;
	default:
		w_ErrMsg.Format(_T("Fail, error code=%d"), p_dwErrorCode);
		break;
	}

	return w_ErrMsg;
}

int EncryptCommandPacket(void)
{
	unsigned short w_nCmdDataSize;
	unsigned int i;
	unsigned int w_nEncSize;
	unsigned int w_nTotalEncSize;
	int w_nRet;
	unsigned short w_nCalcSum;
	unsigned int w_nTmpSize;
	unsigned char w_bCryptedComm = 0;
	unsigned char w_pCryptTempBuf1[CRYPT_DEF_RSA_BYTE + 16];
	unsigned char w_pCryptTempBuf2[CRYPT_DEF_RSA_BYTE + 16];
	
	// check encrypt communication flag
	w_bCryptedComm = GetCryptState();
	if (w_bCryptedComm == 0)
		return ERR_FAIL;
	
	// check command code
	if ((g_pCmdPacket->m_wCMDCode == CMD_TEST_CONNECTION) ||
		(g_pCmdPacket->m_wCMDCode == CMD_GET_OEM_RSA_PUB_KEY) ||
		(g_pCmdPacket->m_wCMDCode == CMD_SET_HOST_RSA_PUB_KEY))
		return ERR_FAIL;
	
	// calculate command data size
	if (g_pCmdPacket->m_wPrefix == CMD_PREFIX_CODE)
		w_nCmdDataSize = CMD_PACKET_LEN;
	else if (g_pCmdPacket->m_wPrefix == CMD_DATA_PREFIX_CODE)
		w_nCmdDataSize = g_pCmdPacket->m_wDataLen + 10;
	else
		return ERR_FAIL;
	w_nCmdDataSize -= PKT_POS_DEC_DATA;

	// set crypted size
	w_nTmpSize = (((w_nCmdDataSize - 1) / CRYPT_DEF_RSA_SPLIT_SIZE) + 1) * CRYPT_DEF_RSA_SPLIT_SIZE;
	
	// padding data
	memset(&g_Packet[w_nCmdDataSize + PKT_POS_DEC_DATA], 0x00, w_nTmpSize - w_nCmdDataSize);
	
	// set encrypt data
	memcpy(w_pCryptTempBuf1, &g_Packet[PKT_POS_DEC_DATA], CRYPT_DEF_RSA_SPLIT_SIZE);
	w_nTotalEncSize = 0;
	for (i = 0; i < w_nTmpSize; i += CRYPT_DEF_RSA_SPLIT_SIZE)
	{
		// padding block
		crt_user_PaddingBlock(w_pCryptTempBuf1, CRYPT_DEF_RSA_SPLIT_SIZE, CRYPT_DEF_RSA_BYTE);

		// set encrypt
		w_nRet = crt_user_RSAEnc(w_pCryptTempBuf1, CRYPT_DEF_RSA_BYTE, w_pCryptTempBuf2, &w_nEncSize, g_pCryptRemoteN, g_pCryptRemoteE);
		if (w_nRet != CRT_SUCCESS)
			return ERR_FAIL;
		
		// set data to command packet
		if (i + CRYPT_DEF_RSA_SPLIT_SIZE < w_nTmpSize)
			memcpy(w_pCryptTempBuf1, &g_Packet[PKT_POS_DEC_DATA + i + CRYPT_DEF_RSA_SPLIT_SIZE], CRYPT_DEF_RSA_SPLIT_SIZE);
		memcpy(&g_Packet[PKT_POS_ENC_DATA + w_nTotalEncSize], &w_pCryptTempBuf2[0], w_nEncSize);

		// set encrypted size
		w_nTotalEncSize += w_nEncSize;
	}

	// set encrypted command parameters
	PKT_VAL_CMD_CODE = CMD_ENCRYPTED_COMMAND;
	PKT_VAL_DATA_LEN = w_nTotalEncSize;
	
	// set checksum
	w_nCalcSum = 0;
	for (i = 0; i < (unsigned int)PKT_POS_CHKSUM; i++)
		w_nCalcSum += g_Packet[i];
	PKT_VAL_CHKSUM = w_nCalcSum;

	// set converted data length
	g_dwPacketSize = PKT_POS_CHKSUM + 2;
	
	return ERR_SUCCESS;
}

int DecryptCommandPacket(void)
{
	unsigned short w_nCalcSum;
	unsigned int i;
	unsigned int w_nDecSize;
	unsigned short w_nTotalSize;
	unsigned int w_nEncSize;
	int w_nRet;
	unsigned char w_pCryptTempBuf[CRYPT_DEF_RSA_BYTE + 16];
	
	// check command code
	if (PKT_VAL_CMD_CODE != CMD_ENCRYPTED_COMMAND)
		return ERR_SUCCESS;
	
	// check encrypted data size
	if ((PKT_VAL_DATA_LEN % CRYPT_DEF_RSA_BYTE) != 0)
		return ERR_FAIL;
	
	// check checksum
	w_nCalcSum = 0;
	for (i = 0; i < (unsigned int)PKT_POS_CHKSUM; i++)
		w_nCalcSum += g_Packet[i];
	if (w_nCalcSum != PKT_VAL_CHKSUM)
		return ERR_FAIL;
	
	// decrypt data
	w_nTotalSize = 0;
	w_nEncSize = (unsigned int)PKT_VAL_DATA_LEN;
	for (i = 0; i < w_nEncSize; i += CRYPT_DEF_RSA_BYTE)
	{
		// rsa decrypt
		w_nRet = crt_user_RSADec(&g_Packet[PKT_POS_ENC_DATA + i], CRYPT_DEF_RSA_BYTE, w_pCryptTempBuf, &w_nDecSize, g_pCryptN, g_pCryptD);
		if (w_nRet != CRT_SUCCESS)
			return ERR_FAIL;

		// remove padding
		crt_user_RemovePaddingBlock(w_pCryptTempBuf, w_nDecSize, CRYPT_DEF_RSA_SPLIT_SIZE);
		
		// set packet buf
		memcpy(&g_Packet[PKT_POS_DEC_DATA + w_nTotalSize], w_pCryptTempBuf, CRYPT_DEF_RSA_SPLIT_SIZE);
		w_nTotalSize += CRYPT_DEF_RSA_SPLIT_SIZE;
	}
	
	return ERR_SUCCESS;
}
