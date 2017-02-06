/*
// =====================================================================================
// 
//       Filename:  elib_time.cpp
// 
//    Description:  时间处理模块，包括：
//                  日期、时间的格式化输出
//                  计数器
// 
//        Version:  1.0
//        Created:  10/25/2013 09:13:02 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_time.hpp"

const char* EL_Time::en_month[13] = {"Nan", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
const char* EL_Time::cn_month[13] = {"零月", "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"};
const char* EL_Time::en_day[32] = {"th",  "st",  "nd",  "rd",  "th",  "th",  "th",  "th",  "th",  "th"};
const char* EL_Time::simp_number[60] = {
	"零", "一", "二", "三", "四", "五", "六", "七", "八", "九",
	"十", "十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九",
	"二十", "二十一", "二十二", "二十三", "二十四", "二十五", "二十六", "二十七", "二十八", "二十九",
	"三十", "三十一", "三十二", "三十三", "三十四", "三十五", "三十六", "三十七", "三十八", "三十九",
	"四十", "四十一", "四十二", "四十三", "四十四", "四十五", "四十六", "四十七", "四十八", "四十九",
	"五十", "五十一", "五十二", "五十三", "五十四", "五十五", "五十六", "五十七", "五十八", "五十九"}; 
const char* EL_Time::trad_number[60] = {
	"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖",
	"壹拾", "壹拾壹", "壹拾贰", "壹拾叁", "壹拾肆", "壹拾伍", "壹拾陆", "壹拾柒", "壹拾捌", "壹拾玖",
	"贰拾", "贰拾壹", "贰拾贰", "贰拾叁", "贰拾肆", "贰拾伍", "贰拾陆", "贰拾柒", "贰拾捌", "贰拾玖",
	"叁拾", "叁拾壹", "叁拾贰", "叁拾叁", "叁拾肆", "叁拾伍", "叁拾陆", "叁拾柒", "叁拾捌", "叁拾玖",
	"肆拾", "肆拾壹", "肆拾贰", "肆拾叁", "肆拾肆", "肆拾伍", "肆拾陆", "肆拾柒", "肆拾捌", "肆拾玖",
	"伍拾", "伍拾壹", "伍拾贰", "伍拾叁", "伍拾肆", "伍拾伍", "伍拾陆", "伍拾柒", "伍拾捌", "伍拾玖"}; 
const char* EL_Time::en_week[8] = {"Nan", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat", "Sun"};
const char* EL_Time::cn_week[8] = {"零", "一", "二", "三", "四", "五", "六", "日"};
const char* EL_Time::simp_time_division[24] = {
	"午夜", "凌晨", "凌晨", "凌晨", "凌晨", "早晨", "早晨", "早晨", "上午", "上午", "上午", "上午", 
	"中午", "下午", "下午", "下午", "下午", "下午", "下午", "晚上", "晚上", "晚上", "晚上", "夜间"};
const char* EL_Time::trad_time_division[24] = {
	"午夜", "淩晨", "淩晨", "淩晨", "淩晨", "早晨", "早晨", "早晨", "上午", "上午", "上午", "上午", 
	"中午", "下午", "下午", "下午", "下午", "下午", "下午", "晚上", "晚上", "晚上", "晚上", "夜間"};

char* EL_Time::date_string(time_t user_time, char *user_buf)
{
	time_t t;
	char *buf;

	user_time<0 ? (t=time(NULL)) : (t=user_time);
	user_buf==NULL ? (buf=time_buf) : (buf=user_buf);
	
	ctime_r(&t, buf);
	buf[25] = 0;

	return buf;
}

char* EL_Time::date_format(const char *format, time_t user_time, int zone_amend)
{
	char *p = do_date_format(format, NULL, user_time, zone_amend);
	if (p == NULL)
		return NULL;
	assert(p - time_buf < buf_len);

	return time_buf;
}		// -----  end of function date_format  -----

long EL_Time::date_format(const char *format, char *user_buf, time_t user_time, int zone_amend)
{
	char *p = do_date_format(format, user_buf, user_time, zone_amend);
	if (p == NULL)
		return -1;

	return p - user_buf;
}		// -----  end of function date_format  -----

char* EL_Time::time_format(time_t user_time, const char *format)
{
	char *p = do_time_format(user_time, format);
	if (p == NULL)
		return NULL;
	assert(p - time_buf < buf_len);

	return time_buf;
}		// -----  end of function time_format  -----

long EL_Time::time_format(time_t user_time, const char *format, char *user_buf)
{
	char *p = do_time_format(user_time, format, user_buf);
	if (p == NULL)
		return -1;

	return p - user_buf;
}		// -----  end of function time_format  -----

int EL_Time::set_time_begin()
{
	return gettimeofday(&time_begin, NULL);
}

int EL_Time::set_time_end()
{
	return gettimeofday(&time_end, NULL);
}

long EL_Time::diff_time_ms()
{
	long diff_s = time_end.tv_sec - time_begin.tv_sec;
	long diff_ms = time_end.tv_usec - time_begin.tv_usec;

	return diff_s * 1000 + diff_ms;
}

char* EL_Time::diff_time_format()
{
	long diff_s = time_end.tv_sec - time_begin.tv_sec;
	long diff_ms = time_end.tv_usec - time_begin.tv_usec;

	sprintf(time_buf, "%04ldh:%02ldm:%02lds:%03ldms", diff_s/3600, diff_s/60%60, diff_s%60, diff_ms);

	return time_buf;
}

void EL_Time::reset()
{
	memset(&time_begin, 0, sizeof(time_begin));
	memset(&time_end, 0, sizeof(time_end));
}

// ==================== PRIVATE METHOD =======================================

void EL_Time::write_date(char *&pbuf, int digit, int digit_type, bool align)
{
	if (digit_type == 0) {	// just use digit
		pbuf += sprintf(pbuf, align?"%02d":"%d", digit);
	} else {
		pbuf += sprintf(pbuf, "%s", digit_type>0 ? simp_number[digit] : trad_number[digit]);
	}
}		// -----  end of function write_date  -----

char* EL_Time::do_date_format(const char *format, char *user_buf, time_t user_time, int zone_amend)
{
	time_t t;
	struct tm tm;

	if (format == NULL)
		return NULL;

	char *buf;

	user_time<0 ? (t=time(NULL)) : (t=user_time);
	user_buf==NULL ? (buf=time_buf) : (buf=user_buf);

	const char *pfmt = format;
	char *pbuf = buf;

	int digit_type = 0;	// -1表示繁体、0表示数字、1表示简体
	bool align = false;	// 数字对齐输出
	const char *p = NULL;
	int year = 0;
	int hour = 0;
	int ret = 0;

	localtime_r(&t, &tm);
	++tm.tm_mon;	// linux的奇怪设定，月份加一等于实际月份

	while (*pfmt) {
		switch (*pfmt) {
		case 'h':	// hour
			++pfmt;	// skip 'h'

			if (-tm.tm_hour > zone_amend)
				tm.tm_hour += 24;
			tm.tm_hour += zone_amend;
			tm.tm_hour %= 24;

			align = false;
			if (*pfmt == 'h') {
				align = true;
				++pfmt;	// skip the second 'h'
			} else if (*pfmt == 'm'){	// hour mark
				++pfmt;	// skip 'm'
				if (*(uint16_t*)pfmt == *(uint16_t*)"en") {	// english suffix
					pbuf += sprintf(pbuf, "%s", tm.tm_hour<12 ? "AM" : "PM");
					pfmt += 2;	// skip 'en'
				} else if (*(uint16_t*)pfmt == *(uint16_t*)"cn") {	// chinese suffix
					p = digit_type>=0 ? simp_time_division[tm.tm_hour] : trad_time_division[tm.tm_hour];
					pbuf += sprintf(pbuf, "%s", p);
					pfmt += 2;	// skip 'cn'
				}
				break;
			}

			hour = tm.tm_hour;
			if (*(uint16_t*)pfmt == *(uint16_t*)"12") {
				pfmt += 2;	// skip '12'
				hour %= 13;	// 不能把中午12点写成中午0点
			}

			write_date(pbuf, hour, digit_type, align);

			break;

		case 'm':	// minute
			++pfmt;	// skip 'm'

			align = false;
			if (*pfmt == 'm') {
				align = true;
				++pfmt;	// skip the second 'm'
			}

			write_date(pbuf, tm.tm_min, digit_type, align);

			break;

		case 's':	// second
			++pfmt;	// skip 's'

			align = false;
			if (*pfmt == 's') {
				align = true;
				++pfmt;	// skip the second 's'
			}

			write_date(pbuf, tm.tm_sec, digit_type, align);

			break;

		case 'Y':	// year
			p = pfmt;
			++pfmt;	// skip 'Y'
			if (*pfmt != 'Y') {
				*pbuf++ = 'Y';
				break;
			}
			++pfmt;	//skip the second 'Y'

			year = tm.tm_year + 1900;
			if (*(uint16_t*)pfmt != *(uint16_t*)"YY") {
				year %= 100;
			} else {
				pfmt += 2;	// skip the second 'YY'
			}

			if (digit_type == 0) {	// just use digit
				ret = sprintf(pbuf, "%02d", year);
				pbuf += ret;
			} else {
				int map[5] = {0, 1, 10, 100, 1000};
				int n;
				for (int i=(int)(pfmt-p); i; --i) {
					n = year/map[i];
					pbuf += sprintf(pbuf, "%s", digit_type>0 ? simp_number[n] : trad_number[n]);
					year %= map[i];
				}
			}

			break;

		case 'M':	// month
			++pfmt;	// skip 'M'

			align = false;
			if (*pfmt == 'M') {
				align = true;
				++pfmt;	// skip the second 'M'
			}

			if (*(uint16_t*)pfmt == *(uint16_t*)"EN") {
				pfmt += 2;	// skip 'EN'
				pbuf += sprintf(pbuf, "%s", en_month[tm.tm_mon]);
			} else {
				write_date(pbuf, tm.tm_mon, digit_type, align);
			}

			break;

		case 'D':	// day
			++pfmt;	// skip 'D'

			align = false;
			if (*pfmt == 'D') {
				align = true;
				++pfmt;	// skip the second 'D'
			}

			if (*(uint16_t*)pfmt == *(uint16_t*)"EN") {
				pfmt += 2;	// skip 'EN'
				pbuf += sprintf(pbuf, "%d%s", tm.tm_mday, en_day[tm.tm_mday%10]);
			} else {
				write_date(pbuf, tm.tm_mday, digit_type, align);
			}

			break;

		case 'W':	// week
			++pfmt;	// skip 'W'

			if (*(uint16_t*)pfmt == *(uint16_t*)"EN") {
				pfmt += 2;	// skip 'EN'
				pbuf += sprintf(pbuf, "%s", en_week[tm.tm_wday]);
			} else if (*(uint16_t*)pfmt == *(uint16_t*)"CN") {
				pfmt += 2;	// skip 'EN'
				pbuf += sprintf(pbuf, "%s", cn_week[tm.tm_wday]);
			} else {
				write_date(pbuf, tm.tm_wday, digit_type, false);
			}

			break;

		case 'S':	// show style
			++pfmt;	// skip 'S'
			if (*(uint32_t*)pfmt == *(uint32_t*)"DGT|") {
				digit_type = 0;
				pfmt += 4;
			} else if (*(uint32_t*)pfmt == *(uint32_t*)"CNS|") {
				digit_type = 1;
				pfmt += 4;
			} else if (*(uint32_t*)pfmt == *(uint32_t*)"CNT|") {
				digit_type = -1;
				pfmt += 4;
			} else {
				*pbuf++ = 'S';
			}
			break;

		default:	// 其他字符，原样拷贝
			*pbuf++ = *pfmt++;
			break;

		}
	}

	*pbuf = 0;
	return pbuf;
}		// -----  end of function do_date_format  -----

char* EL_Time::do_time_format(time_t user_time, const char *format, char *user_buf)
{
	time_t t;
	char *buf;

	user_time<0 ? (t=time(NULL)) : (t=user_time);
	user_buf==NULL ? (buf=time_buf) : (buf=user_buf);

	const char *pfmt = format;
	char *pbuf = buf;
	int align = 0;
	char tmp[16];

	bool all_mark = false;
	bool nz_mark = false;
	int  nz_num = 0;

	long second = user_time;
	long minute = second / 60;
	long hour = minute / 60;
	long day = hour / 24;

	while (*pfmt) {

		switch (*pfmt) {
		case 'd':	// day
			++pfmt;	// skip 'd'

			if (*pfmt != 'd') {
				*pbuf++ = 'd';
				break;
			}
			++pfmt;	// skip the second 'd'

			if (nz_mark && day == 0) {
				for (int i=0; i<nz_num && *pfmt; ++i, ++pfmt);
				all_mark = false; nz_mark = false; nz_num = 0;
				break;
			}

			align = 1;
			if (isdigit(*pfmt)) {
				align = atoi(pfmt);
				pfmt += sprintf(tmp, "%%0%dd", align) - 3;
			} else {
				sprintf(tmp, "%%0%dd", align);
			}
			pbuf += sprintf(pbuf, tmp, day);

			all_mark = false; nz_mark = false; nz_num = 0;
			break;

		case 'h':	// hour
			++pfmt;	// skip 'h'

			if (*pfmt != 'h') {
				*pbuf++ = 'h';
				break;
			}
			++pfmt;	// skip the second 'h'

			if (nz_mark && hour == 0) {
				for (int i=0; i<nz_num && *pfmt; ++i, ++pfmt);
				all_mark = false; nz_mark = false; nz_num = 0;
				break;
			}

			align = 1;
			if (isdigit(*pfmt)) {
				align = atoi(pfmt);
				pfmt += sprintf(tmp, "%%0%dd", align) - 3;
			} else {
				sprintf(tmp, "%%0%dd", align);
			}
			pbuf += sprintf(pbuf, tmp, all_mark ? hour : hour%24);

			all_mark = false; nz_mark = false; nz_num = 0;
			break;

		case 'm':	// minute
			++pfmt;	// skip 'm'

			if (*pfmt != 'm') {
				*pbuf++ = 'm';
				break;
			}
			++pfmt;	// skip the second 'm'

			if (nz_mark && minute == 0) {
				for (int i=0; i<nz_num && *pfmt; ++i, ++pfmt);
				all_mark = false; nz_mark = false; nz_num = 0;
				break;
			}

			align = 1;
			if (isdigit(*pfmt)) {
				align = atoi(pfmt);
				pfmt += sprintf(tmp, "%%0%dd", align) - 3;
			} else {
				sprintf(tmp, "%%0%dd", align);
			}
			pbuf += sprintf(pbuf, tmp, all_mark ? minute : minute%60);

			all_mark = false; nz_mark = false; nz_num = 0;
			break;

		case 's':	// second
			++pfmt;	// skip 's'

			if (*pfmt != 's') {
				*pbuf++ = 's';
				break;
			}
			++pfmt;	// skip the second 's'

			if (nz_mark && second == 0) {
				for (int i=0; i<nz_num && *pfmt; ++i, ++pfmt);
				all_mark = false; nz_mark = false; nz_num = 0;
				break;
			}

			align = 1;
			if (isdigit(*pfmt)) {
				align = atoi(pfmt);
				pfmt += sprintf(tmp, "%%0%dd", align) - 3;
			} else {
				sprintf(tmp, "%%0%dd", align);
			}
			pbuf += sprintf(pbuf, tmp, all_mark ? second : second%60);

			all_mark = false; nz_mark = false; nz_num = 0;
			break;

		case 'N':	// prefix of NZn
			++pfmt;	// skip 'N'
			if (*pfmt != 'Z') {
				*pbuf++ = 'N';
				break;
			}
			++pfmt;	// skip 'Z'
			nz_mark = true;

			if (*pfmt == '(') {
				++pfmt;	// skip '('
				nz_num = atoi(pfmt);
				const char *p = strchr(pfmt, ')');
				if (p == NULL)
					return NULL;
				pfmt += p - pfmt + 1;	// skip number and ')'
			} else {
				nz_num  = 0;
			}

			break;

		case 'A':	// prefix of ALL
			++pfmt;	// skip 'A'
			if (*(uint16_t*)pfmt != *(uint16_t*)"LL") {
				*pbuf++ = 'A';
				break;
			}
			pfmt += 2;	// skip 'LL'

			all_mark = true;

			break;

		default:
			*pbuf++ = *pfmt++;
			break;
		}

	}

	*pbuf = 0;
	return pbuf;
}		// -----  end of function do_time_format  -----

