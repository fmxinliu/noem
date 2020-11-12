#include "stdafx.h"
#include "RTC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace NOEMHost;

RTC::RTC()
{
    _pRTC = new ST_RTC_TYPE;
    memset(_pRTC, 0, sizeof(ST_RTC_TYPE));
}

RTC::~RTC()
{
    delete _pRTC;
    _pRTC = NULL;
}
