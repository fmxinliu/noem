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
            int value = 0;
            testObj.Run_GetParam(0, ref value);
            // 4
            string devInfo = "";
            testObj.Run_GetDeviceInfo(ref devInfo);
            // 5.
            testObj.Run_SetIDNote(0, new byte[] { 1, 2, 3 } );
        }
    }
}
