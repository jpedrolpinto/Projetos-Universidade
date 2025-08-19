#include <string.h>
#include <stdio.h>

int yisleap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int get_yday(int year, int mon, int day)
{
    static const int days[2][13] = {
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}};
    int leap = yisleap(year);

    return days[leap][mon] + day;
}

int get_year(char *date)
{
    int year = 0;

    year = (date[0] - '0') * 1000; // ascii table 0 = 48
    year += (date[1] - '0') * 100;
    year += (date[2] - '0') * 10;
    year += (date[3] - '0');

    return year;
}

int get_month(char *date)
{
    int month = 0;

    month = (date[5] - '0') * 10;
    month += (date[6] - '0');

    return month;
}

int get_day(char *date)
{
    int day = 0;

    day = (date[8] - '0') * 10;
    day += (date[9] - '0');

    return day;
}

int get_hour(char *date)
{
    int hour = 0;

    if (strlen(date) > 10)
    {
        hour = (date[11] - '0') * 10;
        hour += (date[12] - '0');
    }

    return hour;
}

int get_minute(char *date)
{
    int minute = 0;

    if (strlen(date) > 10)
    {
        minute = (date[14] - '0') * 10;
        minute += (date[15] - '0');
    }

    return minute;
}

int get_second(char *date)
{
    int second = 0;

    if (strlen(date) > 10)
    {
        second = (date[17] - '0') * 10;
        second += (date[18] - '0');
    }

    return second;
}

// https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_16
long date_to_epoch(char *date)
{
    int tm_sec = get_second(date);
    int tm_min = get_minute(date);
    int tm_hour = get_hour(date);

    int date_year = get_year(date);

    int tm_yday = get_yday(date_year, get_month(date), get_day(date)) - 1;
    int tm_year = date_year - 1900;

    long time = tm_sec + tm_min * 60 + tm_hour * 3600 + tm_yday * 86400 +
                (tm_year - 70) * 31536000 + ((tm_year - 69) / 4) * 86400 -
                ((tm_year - 1) / 100) * 86400 + ((tm_year + 299) / 400) * 86400;

    return time;
}

int time_diff_days(long t1, long t2)
{
    return (int)((t1 - t2) / 86400);
}

// Indicates the difference between date1 and date2 in their numerical representations.
long date_num_diff(char *date1, char *date2)
{
    long date1_epoch = date_to_epoch(date1);
    long date2_epoch = date_to_epoch(date2);

    return date2_epoch - date1_epoch;
}

// Verifies if the character is a digit
int is_digit(char character) { return (character >= '0' && character <= '9'); }

// Verifies if the date follows the YYYY/MM/DD format
int verify_date_format(char *date_string)
{
    // Not valid if it's empty or length not equal 10
    if (!date_string || strlen(date_string) != 10)
        return 0;
    int temp;

    // Verify separators
    if (date_string[4] != '/' || date_string[7] != '/')
        return 0;

    // Validate year
    if (!(is_digit(date_string[0]) &&
          is_digit(date_string[1]) &&
          is_digit(date_string[2]) &&
          is_digit(date_string[3])))
        return 0;

    // Validate month (>= 1, <= 12)
    if (is_digit(date_string[5]))
        temp = (date_string[5] - '0') * 10;
    else
        return 0;
    if (is_digit(date_string[6]))
        temp += (date_string[6] - '0');
    else
        return 0;
    if (temp < 1 || temp > 12)
        return 0;

    // Validate day (>= 1, <= 31)
    if (is_digit(date_string[8]))
        temp = (date_string[8] - '0') * 10;
    else
        return 0;
    if (is_digit(date_string[9]))
        temp += (date_string[9] - '0');
    else
        return 0;
    if (temp < 1 || temp > 31)
        return 0;

    return 1;
}

// Verifies if the date follows the YYYY/MM/DD HH:mm:ss format
int verify_date_time_format(char *date_string)
{
    // Not valid if it's empty or length not equal 19
    if (!date_string || strlen(date_string) != 19)
        return 0;
    int temp;

    // Validate date part
    char date_part[11];
    strncpy(date_part, date_string, 10);
    date_part[10] = '\0';
    if (!verify_date_format(date_part))
        return 0;

    // Validate time part
    // Verify separators
    if (date_string[10] != ' ' || date_string[13] != ':' || date_string[16] != ':')
        return 0;

    // Validate hour (>= 0, <= 23)
    if (is_digit(date_string[11]))
        temp = (date_string[11] - '0') * 10;
    else
        return 0;
    if (is_digit(date_string[12]))
        temp += (date_string[12] - '0');
    else
        return 0;
    if (temp < 0 || temp > 23)
        return 0;

    // Validate minutes (>= 0, <= 59)
    if (is_digit(date_string[14]))
        temp = (date_string[14] - '0') * 10;
    else
        return 0;
    if (is_digit(date_string[15]))
        temp += (date_string[15] - '0');
    else
        return 0;
    if (temp < 0 || temp > 59)
        return 0;

    // Validate seconds (>= 0, <= 59)
    if (is_digit(date_string[17]))
        temp = (date_string[17] - '0') * 10;
    else
        return 0;
    if (is_digit(date_string[18]))
        temp += (date_string[18] - '0');
    else
        return 0;
    if (temp < 0 || temp > 59)
        return 0;

    return 1;
}
