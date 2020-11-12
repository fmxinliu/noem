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

void RTC::Year::set(unsigned short year)
{
    _pRTC->m_nYear = year;
}

unsigned short RTC::Year::get()
{
    return _pRTC->m_nYear;
}

void RTC::Month::set(unsigned char month)
{
    _pRTC->m_nMonth = month;
}

unsigned char RTC::Month::get()
{
    return _pRTC->m_nMonth;
}

void RTC::Day::set(unsigned char day)
{
    _pRTC->m_nDay = day;
}

unsigned char RTC::Day::get()
{
    return _pRTC->m_nDay;
}

void RTC::Hour::set(unsigned char hour)
{
    _pRTC->m_nHour = hour;
}

unsigned char RTC::Hour::get()
{
    return _pRTC->m_nHour;
}

void RTC::Minute::set(unsigned char minute)
{
    _pRTC->m_nMinute = minute;
}

unsigned char RTC::Minute::get()
{
    return _pRTC->m_nMinute;
}

void RTC::Second::set(unsigned char second)
{
    _pRTC->m_nSecond = second;
}

unsigned char RTC::Second::get()
{
    return _pRTC->m_nSecond;
}

void RTC::Reserved::set(unsigned char reserved)
{
    _pRTC->m_nReserved = reserved;
}

unsigned char RTC::Reserved::get()
{
    return _pRTC->m_nReserved;
}
