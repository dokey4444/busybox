/*
// =====================================================================================
// 
//       Filename:  elib_log.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  11/26/2013 01:21:31 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include "elib_log.hpp"

EL_Log::EL_Log()
{
	log_fp = NULL;
}

EL_Log::~EL_Log()
{
	destroy();
}

int EL_Log::init(const char *dir, const char *name_base, long file_size, long maintain)
{
	if (strlen(dir) <= 0 || strlen(name_base) <= 0)
		return -1;

	if (strlen(dir) >= 256 || strlen(name_base) >= 256)
		return -1;

	strcpy(this->dir, dir);
	strcpy(this->name_base, name_base);
	this->file_size = file_size;
	this->maintain = maintain;

	log_fp = NULL;
	current_num = 0;

	if (find_exist_file() < 0) {
		if (create_new_file() < 0) {
			return -1;
		}
	}

	config(FATAL, STDERR | LOGFILE);
	config(ERROR, STDERR | LOGFILE);
	config(WARN, STDOUT | LOGFILE);
	config(INFO, STDOUT);
	config(DEBUG, STDOUT);

	return 0;
}		// -----  end of function init  -----

void EL_Log::destroy()
{
	if (log_fp) {
		fclose(log_fp);
		log_fp = NULL;
	}
}		// -----  end of function destroy  -----

int EL_Log::config(int level, int style)
{
	if (level < 0 || level > 4)
		return -1;
	if (style < 0 || style > 7)
		return -1;

	output_style[level] = style;

	return 0;
}		// -----  end of function config  -----

void EL_Log::reset()
{
	return;
}		// -----  end of function reset  -----

int EL_Log::ifatal(const char *message, int line, const char *file, const char *func, ...)
{
	char buf[BUFSIZ];
	time_t t = 0;
	struct tm tm;

	time(&t);
	localtime_r(&t, &tm);

	sprintf(buf, "[fatal][%04d-%02d-%02d %02d:%02d:%02d][%s:%d (%s)]:%s\n",
			tm.tm_year+1900, tm.tm_mon, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			file, line, func,
			message);

	va_list argp;
	int ret = -1;

	if (output_style[FATAL] & STDOUT) {
		va_start(argp, func);
		ret = vfprintf(stdout, buf, argp);
	}
	if (output_style[FATAL] & STDERR) {
		va_start(argp, func);
		ret = vfprintf(stderr, buf, argp);
	}
	if (output_style[FATAL] & LOGFILE) {
		locker.lock();

		if (current_size > file_size) {
			if (create_new_file() < 0)
				return -1;
		}

		va_start(argp, func);
		ret = vfprintf(log_fp, buf, argp);
		fflush(log_fp);
		current_size += ret;

		locker.unlock();
	}

	va_end(argp);

	return ret;
}		// -----  end of function fatal  -----

int EL_Log::ierror(const char *message, int line, const char *file, const char *func, ...)
{
	char buf[BUFSIZ];
	time_t t = 0;
	struct tm tm;

	time(&t);
	localtime_r(&t, &tm);

	sprintf(buf, "[error][%04d-%02d-%02d %02d:%02d:%02d][%s:%d (%s)]:%s\n",
			tm.tm_year+1900, tm.tm_mon, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			file, line, func,
			message);

	va_list argp;
	int ret = -1;

	if (output_style[ERROR] & STDOUT) {
		va_start(argp, func);
		ret = vfprintf(stdout, buf, argp);
	}
	if (output_style[ERROR] & STDERR) {
		va_start(argp, func);
		ret = vfprintf(stderr, buf, argp);
	}
	if (output_style[ERROR] & LOGFILE) {
		locker.lock();

		if (current_size > file_size) {
			if (create_new_file() < 0)
				return -1;
		}

		va_start(argp, func);
		ret = vfprintf(log_fp, buf, argp);
		fflush(log_fp);
		current_size += ret;

		locker.unlock();
	}

	va_end(argp);

	return ret;
}		// -----  end of function error  -----

int EL_Log::iwarn(const char *message, int line, const char *file, const char *func, ...)
{
	char buf[BUFSIZ];
	time_t t = 0;
	struct tm tm;

	time(&t);
	localtime_r(&t, &tm);

	sprintf(buf, "[warn][%04d-%02d-%02d %02d:%02d:%02d][%s:%d (%s)]:%s\n",
			tm.tm_year+1900, tm.tm_mon, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			file, line, func,
			message);

	va_list argp;
	int ret = -1;

	if (output_style[WARN] & STDOUT) {
		va_start(argp, func);
		ret = vfprintf(stdout, buf, argp);
	}
	if (output_style[WARN] & STDERR) {
		va_start(argp, func);
		ret = vfprintf(stderr, buf, argp);
	}
	if (output_style[WARN] & LOGFILE) {
		locker.lock();

		if (current_size > file_size) {
			if (create_new_file() < 0)
				return -1;
		}

		va_start(argp, func);
		ret = vfprintf(log_fp, buf, argp);
		fflush(log_fp);
		current_size += ret;

		locker.unlock();
	}

	va_end(argp);

	return ret;
}		// -----  end of function warn  -----

int EL_Log::iinfo(const char *message, ...)
{
	char buf[BUFSIZ];
	time_t t = 0;
	struct tm tm;

	time(&t);
	localtime_r(&t, &tm);

	sprintf(buf, "[info][%04d-%02d-%02d %02d:%02d:%02d]:%s\n",
			tm.tm_year+1900, tm.tm_mon, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			message);

	va_list argp;
	int ret = -1;

	if (output_style[INFO] & STDOUT) {
		va_start(argp, message);
		ret = vfprintf(stdout, buf, argp);
	}
	if (output_style[INFO] & STDERR) {
		va_start(argp, message);
		ret = vfprintf(stderr, buf, argp);
	}
	if (output_style[INFO] & LOGFILE) {
		locker.lock();

		if (current_size > file_size) {
			if (create_new_file() < 0)
				return -1;
		}

		va_start(argp, message);
		ret = vfprintf(log_fp, buf, argp);
		fflush(log_fp);
		current_size += ret;

		locker.unlock();
	}

	va_end(argp);

	return ret;
}		// -----  end of function info  -----

int EL_Log::idebug(const char *message, const char *func, ...)
{
	char buf[BUFSIZ];
	time_t t = 0;
	struct tm tm;

	time(&t);
	localtime_r(&t, &tm);

	sprintf(buf, "[debug][%04d-%02d-%02d %02d:%02d:%02d][(%s)]:%s\n",
			tm.tm_year+1900, tm.tm_mon, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			func, message);

	va_list argp;
	int ret = -1;

	if (output_style[DEBUG] & STDOUT) {
		va_start(argp, func);
		ret = vfprintf(stdout, buf, argp);
	}
	if (output_style[DEBUG] & STDERR) {
		va_start(argp, func);
		ret = vfprintf(stderr, buf, argp);
	}
	if (output_style[DEBUG] & LOGFILE) {
		locker.lock();

		if (current_size > file_size) {
			if (create_new_file() < 0)
				return -1;
		}

		va_start(argp, func);
		ret = vfprintf(log_fp, buf, argp);
		fflush(log_fp);
		current_size += ret;

		locker.unlock();
	}

	va_end(argp);

	return ret;
}		// -----  end of function debug  -----

// ==================== PRIVATE METHOD =======================================

int EL_Log::mkdirs(const char *path, mode_t mode)
{
	const char *org = path;
	const char *p = org;
	char tmp[256];
	char *q = tmp;
	mode_t pre_mode;

	if (path == NULL)
		return -1;

	if (mode == (unsigned)~0) {
		mode = umask(0);
		umask(mode);
		mode = 0777 - mode;
	}

	pre_mode = mode | S_IRWXU;

	while (*p == '/')
		*q++ = *p++;
	org = p;
	p = strchr(p, '/');

	while (p && p[1]) {	/* make previous dir */
		long step = p-org;
		memcpy(q, org, step);
		q[step] = 0;
		if (access(tmp, F_OK) < 0) {
			if (mkdir(tmp, pre_mode) < 0) {
				return -1;
			}
		}
		q += step;

		while (*p == '/') *q++ = *p++;
		org = p;
		p = strchr(p, '/');
	}

	if (access(path, F_OK) < 0) {	/* make the last dir */
		if (mkdir(path, mode) < 0) {
			return -1;
		}
	}

	return 0;
}

int EL_Log::create_new_file()
{
	char filename[256];
	if (access(dir, F_OK) < 0) {
		if (mkdirs(dir) < 0)
			return -1;
	}

	if (current_num == maintain) {
		current_num = 0;
	}

	if (log_fp) {
		fclose(log_fp);
	}

	++current_num;
	sprintf(filename ,"%s/%s_%02ld.log", dir, name_base, current_num);
	log_fp = fopen(filename, "w");
	if (log_fp == NULL)
		return -1;

	current_size = 0;

	return 0;
}		// -----  end of function create_new_file  -----

int EL_Log::find_exist_file()
{
	char filename[256];
	struct stat info;
	long last_file = -1;
	time_t last_mtime = 0;

	for (long i=0; i<maintain; ++i) {
		sprintf(filename ,"%s/%s_%02ld.log", dir, name_base, i+1);
		if (access(filename, F_OK) == 0) {
			if (stat(filename, &info) < 0)
				return -1;
			if (info.st_mtime > last_mtime) {
				last_mtime = info.st_mtime;
				last_file = i+1;
			}
		}
	}

	if (last_file == -1)
		return -1;

	sprintf(filename ,"%s/%s_%02ld.log", dir, name_base, last_file);

	if (stat(filename, &info) < 0)
		return -1;

	log_fp = fopen(filename, "a");
	if (log_fp == NULL)
		return -1;

	current_num = last_file;
	current_size = info.st_size;

	return 0;
}		// -----  end of function find_exist_file  -----

