using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NOEMHost;

namespace Test {
    public class TestCommunication {
        private Communication comm;

        public TestCommunication() {
            comm = new Communication();
        }

        public int Run_TestConnection() {
            return comm.Run_TestConnection();
        }

        public int Run_SetParam(int p_nParamIndex, int p_nParamValue) {
            return comm.Run_SetParam(p_nParamIndex, p_nParamValue);
        }

        public int Run_GetParam(int p_nParamIndex, ref int p_pnParamValue) {
            return comm.Run_GetParam(p_nParamIndex, ref p_pnParamValue);
        }

        public int Run_GetDeviceInfo(ref string p_pszDevInfo) {
            return comm.Run_GetDeviceInfo(ref p_pszDevInfo);
        }

        public int Run_SetIDNote(int p_nTmplNo, byte[] pNote) {
            return comm.Run_SetIDNote(p_nTmplNo, pNote);
        }

        public int Run_GetIDNote(int p_nTmplNo, byte[] pNote) {
            return comm.Run_GetIDNote(p_nTmplNo, pNote);
        }

        public int Run_SetModuleSN(byte[] pModuleSN) {
            return comm.Run_SetModuleSN(pModuleSN);
        }

        public int Run_GetModuleSN(byte[] pModuleSN) {
            return comm.Run_GetModuleSN(pModuleSN);
        }

        public int Run_SetDevPass(byte[] pDevPass) {
            return comm.Run_SetDevPass(pDevPass);
        }

        public int Run_VerfiyDevPass(byte[] pDevPass) {
            return comm.Run_VerfiyDevPass(pDevPass);
        }

        public int Run_EnterStandbyState() {
            return comm.Run_EnterStandbyState();
        }

        public int Run_UpgradeFirmware(byte[] p_pData, uint p_nSize) {
            return comm.Run_UpgradeFirmware(p_pData, p_nSize);
        }

        public int Run_GetImage() {
            return comm.Run_GetImage();
        }

        public int Run_FingerDetect(ref int p_pnDetectResult) {
            return comm.Run_FingerDetect(ref p_pnDetectResult);
        }

        public int Run_UpImage(int p_nType, byte[] p_pData, ref int p_pnImgWidth, ref int p_pnImgHeight) {
            return comm.Run_UpImage(p_nType, p_pData, ref p_pnImgWidth, ref p_pnImgHeight);
        }

        public int Run_DownImage(byte[] p_pData, int p_nWidth, int p_nHeight) {
            return comm.Run_DownImage(p_pData, p_nWidth, p_nHeight);
        }

        public int Run_SLEDControl(int p_nState) {
            return comm.Run_SLEDControl(p_nState);
        }

        public int Run_AdjustSensor() {
            return comm.Run_AdjustSensor();
        }

        public int Run_StoreChar(int p_nTmplNo, int p_nRamBufferID, ref int p_pnDupTmplNo) {
            return comm.Run_StoreChar(p_nTmplNo, p_nRamBufferID, ref p_pnDupTmplNo);
        }

        public int Run_LoadChar(int p_nTmplNo, int p_nRamBufferID) {
            return comm.Run_LoadChar(p_nTmplNo, p_nRamBufferID);
        }

        public int Run_UpChar(int p_nRamBufferID, byte[] p_pbyTemplate, ref uint p_pnSize) {
            return comm.Run_UpChar(p_nRamBufferID, p_pbyTemplate, ref p_pnSize);
        }

        public int Run_DownChar(int p_nRamBufferID, byte[] p_pbyTemplate, uint p_nSize) {
            return comm.Run_DownChar(p_nRamBufferID, p_pbyTemplate,  p_nSize);
        }

        public int Run_DelChar(int p_nSTmplNo, int p_nETmplNo) {
            return comm.Run_DelChar(p_nSTmplNo, p_nETmplNo);
        }

        public int Run_GetEmptyID(int p_nSTmplNo, int p_nETmplNo, ref int p_pnEmptyID) {
            return comm.Run_GetEmptyID(p_nSTmplNo, p_nETmplNo, ref p_pnEmptyID);
        }
        public int Run_GetStatus(int p_nTmplNo, ref int p_pnStatus) {
            return comm.Run_GetStatus(p_nTmplNo, ref p_pnStatus);
        }

        public int Run_GetBrokenID(int p_nSTmplNo, int p_nETmplNo, ref int p_pnCount, ref int p_pnFirstID) {
            return comm.Run_GetBrokenID(p_nSTmplNo, p_nETmplNo, ref p_pnCount, ref p_pnFirstID);
        }

        public int Run_GetEnrollCount(int p_nSTmplNo, int p_nETmplNo, ref int p_pnEnrollCount) {
            return comm.Run_GetEnrollCount(p_nSTmplNo, p_nETmplNo, ref p_pnEnrollCount);
        }

        public int Run_GetEnrolledIDList(ref int p_pnCount, ref int[] p_pnIDs) {
            return comm.Run_GetEnrolledIDList(ref p_pnCount, ref p_pnIDs);
        }

        public int Run_Generate(int p_nRamBufferID) {
            return comm.Run_Generate(p_nRamBufferID);
        }

        public int Run_Merge(int p_nRamBufferID, int p_nMergeCount) {
            return comm.Run_Merge(p_nRamBufferID, p_nMergeCount);
        }

        public int Run_Match(int p_nRamBufferID0, int p_nRamBufferID1, ref int p_pnLearnResult) {
            return comm.Run_Match(p_nRamBufferID0, p_nRamBufferID1, ref p_pnLearnResult);
        }

        public int Run_Search(int p_nRamBufferID, int p_nStartID, int p_nSearchCount, ref int p_pnTmplNo, ref int p_pnLearnResult) {
            return comm.Run_Search(p_nRamBufferID, p_nStartID, p_nSearchCount, ref p_pnTmplNo, ref p_pnLearnResult);
        }

        public int Run_Verify(int p_nTmplNo, int p_nRamBufferID, ref int p_pnLearnResult) {
            return comm.Run_Verify(p_nTmplNo, p_nRamBufferID, ref p_pnLearnResult);
        }

        public int Run_SetRTC(RTC p_stRTCInfo) {
            return comm.Run_SetRTC(p_stRTCInfo);
        }

        public int Run_GetRTC(RTC p_stRTCInfo) {
            return comm.Run_GetRTC(p_stRTCInfo);
        }

        public int Run_GetOEMRSAPubKey(byte[] p_pCryptRemoteN, byte[] p_pCryptRemoteE, ref int p_nKeySizeByte) {
            return comm.Run_GetOEMRSAPubKey(p_pCryptRemoteN, p_pCryptRemoteE, ref p_nKeySizeByte);
        }

	    public int Run_SetHostRSAPubKey(byte[] p_pCryptN, byte[] p_pCryptE, int p_nKeySizeByte) {
            return comm.Run_SetHostRSAPubKey(p_pCryptN, p_pCryptE, p_nKeySizeByte);
        }

        public int InitConnection(int p_nConMode, string p_strComPort, int p_nBaudRate, byte p_bySrcDeviceID, byte p_byDstDeviceID) {
            return comm.InitConnection(p_nConMode, p_strComPort, p_nBaudRate, p_bySrcDeviceID, p_byDstDeviceID);
        }

        public bool EnableCommunicaton(int p_nDevNum, bool p_bVerifyDeviceID, byte[] p_pDevPwd, bool p_bMsgOut) {
            return comm.EnableCommunicaton(p_nDevNum, p_bVerifyDeviceID, p_pDevPwd, p_bMsgOut);
        }

        public bool OpenSerialPort(string p_strComPortIndex, int p_nBaudRateIndex) {
            return comm.OpenSerialPort(p_strComPortIndex, p_nBaudRateIndex);
        }

        public bool Run_Command_NP(ushort p_wCMD) {
            return comm.Run_Command_NP(p_wCMD);
        }

        public void CloseConnection() {
            comm.CloseConnection();
        }

	    public void SetIPandPort(string strDestination, uint dwPort) {
            comm.SetIPandPort(strDestination, dwPort);
        }

	    public void SetCallbackWnd(IntPtr hWnd) {
            comm.SetCallbackWnd(hWnd);
        }
    }
}
