#if !defined(__RTC__H__)
#define __RTC__H__

#include <vcclr.h>
#include <msclr/marshal_cppstd.h>
#include "../Define.h"

using namespace System;
using namespace msclr::interop;
using namespace System::Runtime::InteropServices;

namespace NOEMHost
{
    public ref class RTC
    {
    public:
        RTC();
        virtual ~RTC();

    public:
        unsigned short		m_nYear;
        unsigned char		m_nMonth;
        unsigned char		m_nDay;
        unsigned char		m_nHour;
        unsigned char		m_nMinute;
        unsigned char		m_nSecond;
        unsigned char		m_nReserved;

    private:
        ST_RTC_TYPE *_pRTC;
    };
}

#endif
