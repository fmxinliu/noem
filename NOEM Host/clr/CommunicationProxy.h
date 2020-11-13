#if !defined(__COMMUNICATION__H__)
#define __COMMUNICATION__H__

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>
#include "../Communication.h"
#include "RTC.h"

using namespace std;
using namespace System;
using namespace msclr::interop;

namespace NOEMHost
{
    public ref class Communication
    {
    public:
        Communication();
        virtual ~Communication();

    public:
        int Run_TestConnection(void);
        int Run_SetParam(int index, int value);
        int Run_GetParam(int index, int% value);
        int Run_GetDeviceInfo(String^% deviceInfo);
        int Run_SetIDNote(int index, array<BYTE>^ data);
        int Run_GetIDNote(int index, array<BYTE>^ data);
        int Run_SetModuleSN(array<BYTE>^ data);
        int Run_GetModuleSN(array<BYTE>^ data);
        int Run_SetDevPass(array<BYTE>^ data);
        int Run_VerfiyDevPass(array<BYTE>^ data);
        int Run_EnterStandbyState(void);
        int Run_UpgradeFirmware(array<BYTE>^ data, DWORD length);

        int Run_GetImage(void);
        int Run_FingerDetect(int% value);
        int Run_UpImage(int type, array<BYTE>^ data, int% width, int% height);
        int Run_DownImage(array<BYTE>^ data, int width, int height);
        int Run_SLEDControl(int value);
        int Run_AdjustSensor(void);

        int Run_StoreChar(int index, int bufferIndex, int% value);
        int Run_LoadChar(int index, int bufferIndex);
        int Run_UpChar(int bufferIndex, array<BYTE>^ abyTemplate, unsigned int %length);
        int Run_DownChar(int bufferIndex, array<BYTE>^ abyTemplate, unsigned int length);

        int Run_DelChar(int stmplNo, int etmplNo);
        int Run_GetEmptyID(int stmplNo, int etmplNo, int% emptyID);
        int Run_GetStatus(int tmplNo, int% status);
        int Run_GetBrokenID(int stmplNo, int etmplNo, int% count, int% firstID);
        int Run_GetEnrollCount(int stmplNo, int etmplNo, int% enrollCount);
        int Run_GetEnrolledIDList(int% count, array<int>^% ids);

        int Run_Generate(int bufferIndex);
        int Run_Merge(int bufferIndex, int mergeCount);
        int Run_Match(int bufferIndex0, int bufferIndex1, int% learnResult);
        int Run_Search(int bufferIndex, int startID, int searchCount, int% tmplNo, int% learnResult);
        int Run_Verify(int tmplNo, int bufferIndex, int% learnResult);

        int Run_SetRTC(RTC^ rtc);
        int Run_GetRTC(RTC^ rtc);

        int Run_GetOEMRSAPubKey(array<BYTE>^ cryptN, array<BYTE>^ cryptE, int %keySizeByte);
        int Run_SetHostRSAPubKey(array<BYTE>^ cryptN, array<BYTE>^ cryptE, int keySizeByte);

        int InitConnection(int mode, String^ port, int baudRate, BYTE srcDeviceID, BYTE DstDeviceID);

        bool EnableCommunicaton(int devNum, bool verifyDeviceID, array<BYTE>^ devPwd, bool msgOut);
        bool OpenSerialPort(String^ comPortIndex, int baudRateIndex);
        bool Run_Command_NP(WORD p_wCMD);

        void CloseConnection();
        void SetIPandPort(String^ destination, DWORD dwPort);
        void SetCallbackWnd(IntPtr hWnd);

    private:
        CCommunication *_pcomm;
    };
}

#endif
