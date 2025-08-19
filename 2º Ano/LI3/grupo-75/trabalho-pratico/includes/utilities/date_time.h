#ifndef DATE_TIME_H
#define DATE_TIME_H

#define CURRENT_DATE "2023/10/01"

int get_year(char *date);

int get_month(char *date);

int get_day(char *date);

long date_to_epoch(char* date);

long date_num_diff(char *date1, char *date2);

int verify_date_format(char *date_string);

int verify_date_time_format(char *date_string);

int time_diff_days(long t1, long t2);

#endif