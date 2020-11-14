using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Test {
    public class Program {
        static void Main(string[] args) {
            TestCommunication testObj = new TestCommunication();
            // 1
            testObj.Run_TestConnection();
            // 2
            testObj.Run_SetParam(0, 0);
            // 3
            int value = -1;
            testObj.Run_GetParam(0, ref value);
            // 4
            string devInfo = "信息";
            testObj.Run_GetDeviceInfo(ref devInfo);
            // 5.
            testObj.Run_SetIDNote(0, new byte[] { 1, 2, 3 } );
            // 6.
            byte[] d6 = {1, 2, 3};
            testObj.Run_GetIDNote(0, ref d6);
            // 7.
            testObj.Run_SetModuleSN(new byte[] { 1, 2, 3 });
            // 8.
            byte[] d8 = { 1, 2, 3 };
            testObj.Run_GetModuleSN(ref d8);
            // 9.
            testObj.Run_SetDevPass(new byte[] { 1, 2, 3 });
            // 10.
            byte[] d10 = { 1, 2, 3 };
            testObj.Run_VerfiyDevPass(ref d10);
            // 11.
            testObj.Run_EnterStandbyState();
            // 12.
            int size12 = 3;
            testObj.Run_UpgradeFirmware(new byte[] { 1, 2, 3 }, size12);
            // 13.
            testObj.Run_GetImage();
            // 14.
            int v14 = -1;
            testObj.Run_FingerDetect(ref v14);
            // 15.
            int w = -1, h = -1;
            byte[] d15 = { 1, 2, 3 };
            testObj.Run_UpImage(0, ref d15, ref w, ref h);
            // 16.
            byte[] d16 = { 1, 2, 3 };
            testObj.Run_DownImage(d16, 1, 3);
            // 17.
            testObj.Run_SLEDControl(0);
            // 18.
            testObj.Run_AdjustSensor();
            // 19.
            int v19 = -1;
            testObj.Run_StoreChar(0, 0, ref v19);
            // 20.
            testObj.Run_LoadChar(0, 0);
            // 21.
            int size21 = 777;
            byte[] d21 = { 1, 2, 3 };
            testObj.Run_UpChar(0, ref d21, ref size21);
            // 22.
            testObj.Run_DownChar(0, new byte[] { 1, 2, 3 }, 777);
            // 23.
            testObj.Run_DelChar(0, 0);
            // 24.
            int v24 = -1;
            testObj.Run_GetEmptyID(0, 0, ref v24);
            // 25.
            int v25 = -1;
            testObj.Run_GetStatus(0, ref v25);
            // 26.
            int v26_1 = -1, v26_2 = -1;
            testObj.Run_GetBrokenID(0, 0, ref v26_1, ref v26_2);
            // 27.
            int v27 = -1;
            testObj.Run_GetEnrollCount(0, 0, ref v27);
            // 28.
            int v28 = -1;
            int[] d28 = { 1, 2, 3 };
            testObj.Run_GetEnrolledIDList(ref v28, ref d28);
            // 29.
            testObj.Run_Generate(0);
            // 30.
            testObj.Run_Merge(0, 0);
            // 31.
            int v31 = -1;
            testObj.Run_Match(0, 0, ref v31);
            // 32.
            int v32_1 = -1, v32_2 = -1;
            testObj.Run_Search(0, 0, 1, ref v32_1, ref v32_2);
            // 33.
            int v33 = -1;
            testObj.Run_Verify(0, 0, ref v33);
            // 34.
            NOEMHost.RTC rtc34 = new NOEMHost.RTC();
            rtc34.Day = 10;
            testObj.Run_SetRTC(rtc34);
            // 35.
            NOEMHost.RTC rtc35 = null;
            testObj.Run_GetRTC(ref rtc35);
            // 36.
            byte[] d36_1 = { 1, 2, 3 };
            byte[] d36_2 = { 4, 5, 6 };
            int v36 = -1;
            testObj.Run_GetOEMRSAPubKey(ref d36_1, ref d36_2, ref v36);
            // 37.
            byte[] d37_1 = { 1, 2, 3 };
            byte[] d37_2 = { 4, 5, 6 };
            int v37 = 3;
            testObj.Run_SetHostRSAPubKey(d37_1, d37_2, v37);
            // 38.
            testObj.InitConnection(0, "COM1", BaudRateIndex.BAUD9600, 0, 1);
            // 39.
            testObj.EnableCommunicaton(2, true, Encoding.ASCII.GetBytes("123"), true);
            // 40.
            testObj.OpenSerialPort("COM2", BaudRateIndex.BAUD9600);
            // 41.
            testObj.Run_Command_NP(999);
            // 42.
            testObj.CloseConnection();
            // 43.
            testObj.SetIPandPort("COM5", 777);
            // 44.
            testObj.SetCallbackWnd(new IntPtr(123456));
        }
    }
}
