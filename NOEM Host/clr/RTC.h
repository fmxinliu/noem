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
        property unsigned short Year
        {
            void set(unsigned short year);
            unsigned short get();
        }

        property unsigned char Month
        {
            void set(unsigned char month);
            unsigned char get();
        }

        property unsigned char Day
        {
            void set(unsigned char day);
            unsigned char get();
        }

        property unsigned char Hour
        {
            void set(unsigned char hour);
            unsigned char get();
        }

        property unsigned char Minute
        {
            void set(unsigned char minute);
            unsigned char get();
        }

        property unsigned char Second
        {
            void set(unsigned char second);
            unsigned char get();
        }

        property unsigned char Reserved
        {
            void set(unsigned char reserved);
            unsigned char get();
        }

    private:
        ST_RTC_TYPE *_pRTC;
    };
}

#endif
