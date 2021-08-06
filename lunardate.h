#ifndef LUNARDATE_H_INCLUDED
#define LUNARDATE_H_INCLUDED

#include <string>
#include <vector>
#include <time.h>

struct LunarDate
{
    std::string year;
    std::string month;
    std::string day;

    std::string dump() const
    {
        return "农历" + year + "年" + month + "月" + day;
    }
};

// based on https://raw.githubusercontent.com/mochaaP/mugtungshing/dev/src/lunar/index.js

static const int lunarYearArr[] = {
    0x195a6, // 1970-1979
    0x095b0,
    0x049b0,
    0x0a974,
    0x0a4b0,
    0x0b27a,
    0x06a50,
    0x06d40,
    0x0af46,
    0x0ab60,
    0x09570, // 1980-1989
    0x04af5,
    0x04970,
    0x064b0,
    0x074a3,
    0x0ea50,
    0x06b58,
    0x055c0,
    0x0ab60,
    0x096d5,
    0x092e0, // 1990-1999
    0x0c960,
    0x0d954,
    0x0d4a0,
    0x0da50,
    0x07552,
    0x056a0,
    0x0abb7,
    0x025d0,
    0x092d0,
    0x0cab5, // 2000-2009
    0x0a950,
    0x0b4a0,
    0x0baa4,
    0x0ad50,
    0x055d9,
    0x04ba0,
    0x0a5b0,
    0x15176,
    0x052b0,
    0x0a930, // 2010-2019
    0x07954,
    0x06aa0,
    0x0ad50,
    0x05b52,
    0x04b60,
    0x0a6e6,
    0x0a4e0,
    0x0d260,
    0x0ea65,
    0x0d530, // 2020-2029
    0x05aa0,
    0x076a3,
    0x096d0,
    0x04afb,
    0x04ad0,
    0x0a4d0,
    0x1d0b6,
    0x0d250,
    0x0d520,
    0x0dd45, // 2030-2039
    0x0b5a0,
    0x056d0,
    0x055b2,
    0x049b0,
    0x0a577,
    0x0a4b0,
    0x0aa50,
    0x1b255,
    0x06d20,
    0x0ada0, // 2040-2049
    0x14b63,
    0x09370,
    0x049f8,
    0x04970,
    0x064b0,
    0x168a6,
    0x0ea50,
    0x06b20,
    0x1a6c4,
    0x0aae0, // 2050-2059
    0x0a2e0,
    0x0d2e3,
    0x0c960,
    0x0d557,
    0x0d4a0,
    0x0da50,
    0x05d55,
    0x056a0,
    0x0a6d0,
    0x055d4, // 2060-2069
    0x052d0,
    0x0a9b8,
    0x0a950,
    0x0b4a0,
    0x0b6a6,
    0x0ad50,
    0x055a0,
    0x0aba4,
    0x0a5b0,
    0x052b0, // 2070-2079
    0x0b273,
    0x06930,
    0x07337,
    0x06aa0,
    0x0ad50,
    0x14b55,
    0x04b60,
    0x0a570,
    0x054e4,
    0x0d160, // 2080-2089
    0x0e968,
    0x0d520,
    0x0daa0,
    0x16aa6,
    0x056d0,
    0x04ae0,
    0x0a9d4,
    0x0a2d0,
    0x0d150,
    0x0f252, // 2090-2099
    0x0d520 // 2100
};
static const char* lunarMonth[] = {
    "正",
    "二",
    "三",
    "四",
    "五",
    "六",
    "七",
    "八",
    "九",
    "十",
    "冬",
    "腊"
};
static const char* lunarDay[] = {
    "一",
    "二",
    "三",
    "四",
    "五",
    "六",
    "七",
    "八",
    "九",
    "十",
    "初",
    "廿"
};
static const char* tianGan[] = {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};
static const char* diZhi[] = {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};

inline unsigned int hasLeapMonth(int year)
{
    if (year & 0xf)
      return year & 0xf;
    else
      return 0;
}

inline unsigned int leapMonthDays(int year)
{
    if(hasLeapMonth(year))
      return (year & 0xf0000) ? 30 : 29;
    else
      return 0;
}

inline unsigned int lunarYearDays(int year)
{
    int totalDays = 0;
    for(int i = 0x8000; i > 0x8; i >>= 1)
    {
      int monthDays = (year & i) ? 30 : 29;
      totalDays += monthDays;
    }
    if (hasLeapMonth(year))
    {
      totalDays += leapMonthDays(year);
    }
    return totalDays;
}

inline std::vector<int> lunarYearMonths(int year)
{
    std::vector<int> months;

    for(int i = 0x8000; i > 0x8; i >>= 1)
    {
        int monthDays = (year & i) ? 30 : 29;
        months.push_back(monthDays);
    }
    if(hasLeapMonth(year))
    {
        months.insert(months.begin() + hasLeapMonth(year), leapMonthDays(year));
    }
    return months;
}

inline const char* getTianGan(int year)
{
    size_t key = (year - 3) % 10;
    if(key == 0) key = 10;
    return tianGan[key - 1];
}

inline const char* getDiZhi(int year)
{
    size_t key = (year - 3) % 12;
    if(key == 0) key = 12;
    return diZhi[key - 1];
}

inline time_t getTime(int year, int month, int day)
{
    auto t = time(nullptr);
    tm *tm = gmtime(&t);
    tm->tm_year = year - 1900;
    tm->tm_mon = month - 1;
    tm->tm_mday = day;
    return mktime(tm);
}

inline LunarDate getLunarDate(int solarYear, int solarMonth, int solarDay)
{
    LunarDate date;
    time_t thisTime = getTime(solarYear, solarMonth, solarDay);
    time_t beginTime = getTime(1970, 2, 6);
    int daySpan = (thisTime - beginTime) / (24 * 60 * 60) + 1;

    int lunarYear;
    for(size_t i = 0; i < std::size(lunarYearArr); i++)
    {
        daySpan -= lunarYearDays(lunarYearArr[i]);
        if (daySpan <= 0)
        {
            lunarYear = 1970 + i;
            daySpan += lunarYearDays(lunarYearArr[i]);
            break;
        }
    }
    date.year = getTianGan(lunarYear);
    date.year.append(getDiZhi(lunarYear));

    size_t lunarYearIdx = lunarYearArr[lunarYear - 1970];
    auto months = lunarYearMonths(lunarYearIdx);
    unsigned int leapMonth = hasLeapMonth(lunarYearIdx);
    for(size_t i = 0; i < months.size(); i++)
    {
        daySpan -= months[i];
        if (daySpan <= 0)
        {
            if(leapMonth && leapMonth <= i)
            {
                if(leapMonth < i)
                    date.month = lunarMonth[i - 1];
                else if(leapMonth == i)
                {
                    date.month = "闰";
                    date.month.append(lunarMonth[i - 1]);
                }
                else
                {
                    date.month = lunarMonth[i];
                }
            }
            else
                date.month = lunarMonth[i];
            daySpan += months[i];
            break;
        }
    }

    if(daySpan < 11)
    {
        date.day = lunarDay[10];
        date.day.append(lunarDay[daySpan - 1]);
    }
    else if(daySpan > 10 && daySpan < 20)
    {
        date.day = lunarDay[9];
        date.day.append(lunarDay[daySpan - 11]);
    }
    else if(daySpan == 20)
    {
        date.day = lunarDay[1];
        date.day.append(lunarDay[9]);
    }
    else if(daySpan < 30)
    {
        date.day = lunarDay[11];
        date.day.append(lunarDay[daySpan - 21]);
    }
    else if(daySpan == 30)
    {
        date.day = lunarDay[2];
        date.day.append(lunarDay[9]);
    }

    return date;
}
#endif
