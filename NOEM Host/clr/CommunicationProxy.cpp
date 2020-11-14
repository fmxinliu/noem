#include "stdafx.h"
#include "CommunicationProxy.h"
#include "crypt_user.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GET_COMMAND_START(pValue, count)            \
    int size = count;                               \
    BYTE *pValue = new BYTE[size];

#define GET_COMMAND_END(pValue, data, ret)          \
    if (!ret) {                                     \
        data = gcnew array<BYTE>(size);             \
        for (int i = 0;i < size;i++)                \
            data[i] = pValue[i];                    \
    }                                               \
    delete[] pValue;                                \
    pValue = NULL;                                  \
    return ret;

#define SET_COMMAND_START(data, pValue)             \
    int size = data->Length;                        \
    BYTE *pValue = new BYTE[size];                  \
    for (int i = 0;i < size;i++)                    \
        pValue[i] = data[i];

#define SET_COMMAND_END(pValue, ret)                \
    delete[] pValue;                                \
    pValue = NULL;                                  \
    return ret;

#define HOLD_VALUE(destValue, srcValue, ret)        \
    if (!ret) {                                     \
        destValue = srcValue;                       \
    }

using namespace NOEMHost;

/************************************************************************/
/************************************************************************/
Communication::Communication()
{
    _pcomm = new CCommunication;
}
/************************************************************************/
/************************************************************************/
Communication::~Communication()
{
    delete _pcomm;
    _pcomm = NULL;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_TestConnection(void)
{
    return _pcomm->Run_TestConnection();
}
/************************************************************************/
/************************************************************************/
int Communication::Run_SetParam(int index, int value)
{
    return _pcomm->Run_SetParam(index, value);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetParam(int index, int% paramValue)
{
    int value = 0;
    int ret = _pcomm->Run_GetParam(index, &value);
    HOLD_VALUE(paramValue, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetDeviceInfo(String^% deviceInfo)
{
    char szDeviceInfo[256] = {};
    int ret = _pcomm->Run_GetDeviceInfo(szDeviceInfo);
    HOLD_VALUE(deviceInfo, marshal_as<String^>(szDeviceInfo), ret);
    return ret;
}
/***************************************************************************/
/***************************************************************************/
int Communication::Run_SetIDNote(int index, array<BYTE>^ data)
{
    SET_COMMAND_START(data, pValue);
    int ret = _pcomm->Run_SetIDNote(index, pValue);
    SET_COMMAND_END(pValue, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetIDNote(int index, array<BYTE>^% data)
{
    GET_COMMAND_START(pValue, ID_NOTE_SIZE);
    int ret = _pcomm->Run_GetIDNote(index, pValue);
    GET_COMMAND_END(pValue, data, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_SetModuleSN(array<BYTE>^ data)
{
    SET_COMMAND_START(data, pValue);
    int ret = _pcomm->Run_SetModuleSN(pValue);
    SET_COMMAND_END(pValue, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetModuleSN(array<BYTE>^% data)
{
    GET_COMMAND_START(pValue, MODULE_SN_LEN);
    int ret = _pcomm->Run_GetModuleSN(pValue);
    GET_COMMAND_END(pValue, data, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_SetDevPass(array<BYTE>^ data)
{
    SET_COMMAND_START(data, pValue);
    int ret = _pcomm->Run_SetDevPass(pValue);
    SET_COMMAND_END(pValue, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_VerfiyDevPass(array<BYTE>^% data)
{
    GET_COMMAND_START(pValue, MAX_DEVPASS_LEN);
    int ret = _pcomm->Run_VerfiyDevPass(pValue);
    GET_COMMAND_END(pValue, data, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_EnterStandbyState()
{
    return _pcomm->Run_EnterStandbyState();
}
/************************************************************************/
/************************************************************************/
int Communication::Run_UpgradeFirmware(array<BYTE>^ data, DWORD length)
{
    SET_COMMAND_START(data, pValue);
    int ret = _pcomm->Run_UpgradeFirmware(pValue, length);
    SET_COMMAND_END(pValue, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetImage()
{
    return _pcomm->Run_GetImage();
}
/************************************************************************/
/************************************************************************/
int Communication::Run_FingerDetect(int% paramValue)
{
    int value = 0;
    int ret = _pcomm->Run_FingerDetect(&value);
    HOLD_VALUE(paramValue, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
#define IMAGE_BUFFER_SIZE 100000 // max size : 300*300
int Communication::Run_UpImage(int type, array<BYTE>^% data, int% width, int% height)
{
    int w = 0, h = 0;
    GET_COMMAND_START(pValue, IMAGE_BUFFER_SIZE);
    int ret = _pcomm->Run_UpImage(type, pValue, &w, &h);
    HOLD_VALUE(width, w, ret);
    HOLD_VALUE(height, h, ret);
    GET_COMMAND_END(pValue, data, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_DownImage(array<BYTE>^ data, int width, int height)
{
    SET_COMMAND_START(data, pValue);
    int ret = _pcomm->Run_DownImage(pValue, width, height);
    SET_COMMAND_END(pValue, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_SLEDControl(int value)
{
    return _pcomm->Run_SLEDControl(value);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_AdjustSensor(void)
{
    return _pcomm->Run_AdjustSensor();
}
/************************************************************************/
/************************************************************************/
int Communication::Run_StoreChar(int index, int bufferIndex, int% paramValue)
{
    int value = 0;
    int ret = _pcomm->Run_StoreChar(index, bufferIndex, &value);
    HOLD_VALUE(paramValue, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_LoadChar(int index, int bufferIndex)
{
    return _pcomm->Run_LoadChar(index, bufferIndex);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_UpChar(int index, array<BYTE>^% data, unsigned int %length)
{
    unsigned int nSize = 0;
    GET_COMMAND_START(pValue, MAX_TEMPLATE_SIZE);
    int ret = _pcomm->Run_UpChar(index, pValue, &nSize);
    HOLD_VALUE(length, nSize, ret);
    GET_COMMAND_END(pValue, data, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_DownChar(int index, array<BYTE>^ data, unsigned int length)
{
    SET_COMMAND_START(data, pValue);
    int ret = _pcomm->Run_DownChar(index, pValue, length);
    SET_COMMAND_END(pValue, ret);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_DelChar(int stmplNo, int etmplNo)
{
    return _pcomm->Run_DelChar(stmplNo, etmplNo);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetEmptyID(int stmplNo, int etmplNo, int% emptyID)
{
    int value = 0;
    int ret = _pcomm->Run_GetEmptyID(stmplNo, etmplNo, &value);
    HOLD_VALUE(emptyID, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetStatus(int tmplNo, int% status)
{
    int value = 0;
    int ret = _pcomm->Run_GetStatus(tmplNo, &value);
    HOLD_VALUE(status, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetBrokenID(int stmplNo, int etmplNo, int% count, int% firstID)
{
    int value1 = 0, value2 = 0;
    int ret = _pcomm->Run_GetBrokenID(stmplNo, etmplNo, &value1, &value2);
    HOLD_VALUE(count, value1, ret);
    HOLD_VALUE(firstID, value2, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetEnrollCount(int stmplNo, int etmplNo, int% enrollCount)
{
    int value = 0;
    int ret = _pcomm->Run_GetEnrollCount(stmplNo, etmplNo, &value);
    HOLD_VALUE(enrollCount, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetEnrolledIDList(int% length, array<int>^% data)
{
    int value = 0;
    int count = 5000;
    int *pValue = new int[count];
    int ret = _pcomm->Run_GetEnrolledIDList(&value, pValue);
    if (!ret) {
        length = value;
        data = gcnew array<int>(value);
        for (int i = 0;i < length;++i) {
            data[i] = pValue[i];
        }
    }
    delete[] pValue;
    pValue = NULL;
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_Generate(int bufferIndex)
{
    return _pcomm->Run_Generate(bufferIndex);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_Merge(int bufferIndex, int mergeCount)
{
    return _pcomm->Run_Merge(bufferIndex, mergeCount);
}
/************************************************************************/
/************************************************************************/
int Communication::Run_Match(int bufferIndex0, int bufferIndex1, int% learnResult)
{
    int value = 0;
    int ret = _pcomm->Run_Match(bufferIndex0, bufferIndex1, &value);
    HOLD_VALUE(learnResult, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_Search(int bufferIndex, int startID, int searchCount, int% tmplNo, int% learnResult)
{
    int value1 = 0, value2 = 0;
    int ret = _pcomm->Run_Search(bufferIndex, startID, searchCount, &value1, &value2);
    HOLD_VALUE(tmplNo, value1, ret);
    HOLD_VALUE(learnResult, value2, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_Verify(int tmplNo, int bufferIndex, int% learnResult)
{
    int value = 0;
    int ret = _pcomm->Run_Verify(tmplNo, bufferIndex, &value);
    HOLD_VALUE(learnResult, value, ret);
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_SetRTC(RTC^ rtc)
{
    return _pcomm->Run_SetRTC(*rtc->GetRTCData());
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetRTC(RTC^% rtc)
{
    ST_RTC_TYPE data;
    int ret = _pcomm->Run_GetRTC(&data);
    if (!ret) {
        rtc = gcnew RTC;
        rtc->SetRTCData(&data);
    }
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_GetOEMRSAPubKey(array<BYTE>^% cryptN, array<BYTE>^% cryptE, int %keySizeByte)
{
    int keySize = 0;
    BYTE *pValueN = new BYTE[CRYPT_DEF_RSA_BYTE];
    BYTE *pValueE = new BYTE[CRYPT_DEF_RSA_BYTE];

    memset(pValueN, 0x00, CRYPT_DEF_RSA_BYTE);
    memset(pValueE, 0x00, CRYPT_DEF_RSA_BYTE);

    int ret = _pcomm->Run_GetOEMRSAPubKey(pValueN, pValueE, &keySize);
    if (!ret) {
        keySizeByte = keySize;
        cryptN = gcnew array<BYTE>(keySize);
        cryptE = gcnew array<BYTE>(keySize);
        for (int i = 0;i < keySize;++i) {
            cryptN[i] = pValueN[i];
            cryptE[i] = pValueE[i];
        }
    }
    delete[] pValueN;
    delete[] pValueE;
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::Run_SetHostRSAPubKey(array<BYTE>^ cryptN, array<BYTE>^ cryptE, int keySizeByte)
{
    BYTE *pValueN = new BYTE[keySizeByte];
    BYTE *pValueE = new BYTE[keySizeByte];

    for (int i = 0;i < keySizeByte;++i) {
        pValueN[i] = cryptN[i];
        pValueE[i] = cryptE[i];
    }

    int ret = _pcomm->Run_SetHostRSAPubKey(pValueN, pValueE, keySizeByte);

    delete[] pValueN;
    delete[] pValueE;
    return ret;
}
/************************************************************************/
/************************************************************************/
int Communication::InitConnection(int mode, String^ port, int baudRate, BYTE srcDeviceID, BYTE DstDeviceID)
{
    CString strPort = marshal_as<std::string>(port).c_str();
    return _pcomm->InitConnection(mode, strPort, baudRate, srcDeviceID, DstDeviceID);
}
/************************************************************************/
/************************************************************************/
bool Communication::EnableCommunicaton(int devNum, bool verifyDeviceID, array<BYTE>^ devPwd, bool msgOut)
{
    BYTE *pValue = new BYTE[devPwd->Length];
    int ret = _pcomm->EnableCommunicaton(devNum, verifyDeviceID, pValue, msgOut);
    delete[] pValue;
    return !!ret;
}
/************************************************************************/
/************************************************************************/
bool Communication::OpenSerialPort(String^ comPortIndex, int baudRateIndex)
{
    CString strComPortIndex = marshal_as<std::string>(comPortIndex).c_str();
    return !!_pcomm->OpenSerialPort(strComPortIndex, baudRateIndex);
}
/************************************************************************/
/************************************************************************/
bool Communication::Run_Command_NP(WORD p_wCMD)
{
    return !!_pcomm->Run_Command_NP(p_wCMD);
}
/************************************************************************/
/************************************************************************/
void Communication::CloseConnection()
{
    _pcomm->CloseConnection();
}
/************************************************************************/
/************************************************************************/
void Communication::SetIPandPort(String^ destination, DWORD dwPort)
{
    CString strDestination = marshal_as<std::string>(destination).c_str();
    _pcomm->SetIPandPort(strDestination, dwPort);
}
/************************************************************************/
/************************************************************************/
void Communication::SetCallbackWnd(IntPtr hWnd)
{
    //IntPtr managedHWND(hwnd);
    HWND nativeHWND = (HWND)hWnd.ToPointer();
    _pcomm->SetCallbackWnd(nativeHWND);
}