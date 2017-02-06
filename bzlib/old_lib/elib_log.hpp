/*
// =====================================================================================
// 
//       Filename:  elib_log.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  11/26/2013 01:21:28 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_LOG_H_
#define  _ELIB_LOG_H_

#include "elib_util.hpp"
#include "elib_lock.hpp"

// =====================================================================================
//        Class:  EL_Log
//  Description:  
// =====================================================================================
class EL_Log
{
public:
	// ====================  INNER TYPE    =======================================
	enum level {
		FATAL = 0,	// 程序发生极端严重的错误，有可能已经导致程序异常退出，需要立即人为干预
		ERROR = 1,	// 程序已经发生错误，虽然不会异常退出但已经对使用者产生影响，需要立即排查错误
		WARN = 2,	// 程序有可能发生错误，虽然不会对使用者产生影响，但隐患已经出现，需要告知开发者
		INFO = 3,	// 程序运行时的正常输出，用于表示程序运行的过程
		DEBUG = 4	// 程序调试信息，只用于调试时输出使用
	};

	enum output_style {
		LOGNULL= 0,	// 不输出
		STDOUT = 1,	// 输出到标准输出（默认）
		STDERR = 2,	// 输出到标准错误输出
		LOGFILE= 4	// 输出到日志文件
	};

	// ====================  LIFECYCLE     =======================================
	EL_Log();
	~EL_Log();

	// ====================  INTERFACE     =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  init
	//  Description:   
	//   Parameters:  name_base: 日志文件的前缀名
	//                file_size: 每个日志文件允许的大小
	//                maintain: 最多保留多少个日志文件
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int init(const char *dir = "./", const char *name_base = "proj", long file_size = 64<<20, long maintain = 8);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  destroy
	//  Description:  
	// =====================================================================================
	*/
	void destroy();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  config
	//  Description:  根据level设置输出方式
	//   Parameters:  level: 输出级别：FATAL, ERROR, WARN, INFO, DEBUG其中之一
	//                style: 输出类型：LOGNULL, STDOUT, STDERR, LOGFILE其中一个或两个（用按位或表示）
	//  ReturnValue:  成功返回0，失败返回-1
	// =====================================================================================
	*/
	int config(int level, int style);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  reset
	//  Description:  清除所有日志文件
	// =====================================================================================
	*/
	void reset();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  fatal
	//  Description:  输出fatal日志，参数与返回值与printf一样
	// =====================================================================================
	*/
#ifndef LOG_NO_MACRO
#ifndef LOG_REMOVE_FATAL
#define fatal(message, ...) ifatal(message, __LINE__, __FILE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define fatal(message, ...) ((void)0)
#endif	// ----- #ifndef LOG_REMOVE_FATAL  -----
#endif	// ----- #ifndef LOG_NO_MACRO  -----
	int ifatal(const char *message, int line, const char *file, const char *func, ...);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  error
	//  Description:  输出error日志，参数与返回值与printf一样
	// =====================================================================================
	*/
#ifndef LOG_NO_MACRO
#ifndef LOG_REMOVE_ERROR
#define error(message, ...) ierror(message, __LINE__, __FILE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define error(message, ...) ((void)0)
#endif	// ----- #ifndef LOG_REMOVE_ERROR  -----
#endif	// ----- #ifndef LOG_NO_MACRO  -----
	int ierror(const char *message, int line, const char *file, const char *func, ...);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  warn
	//  Description:  输出warn日志，参数与返回值与printf一样
	// =====================================================================================
	*/
#ifndef LOG_NO_MACRO
#ifndef LOG_REMOVE_WARN
#define warn(message, ...) iwarn(message, __LINE__, __FILE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define warn(message, ...) ((void)0)
#endif	// ----- #ifndef LOG_REMOVE_WARN  -----
#endif	// ----- #ifndef LOG_NO_MACRO  -----
	int iwarn(const char *message, int line, const char *file, const char *func, ...);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  info
	//  Description:  输出info日志，参数与返回值与printf一样
	// =====================================================================================
	*/
#ifndef LOG_NO_MACRO
#ifndef LOG_REMOVE_INFO
#define info(message, ...) iinfo(message, ##__VA_ARGS__)
#else
#define info(message, ...) ((void)0)
#endif	// ----- #ifndef LOG_REMOVE_INFO  -----
#endif	// ----- #ifndef LOG_NO_MACRO  -----
	int iinfo(const char *message, ...);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  debug
	//  Description:  输出debug日志，参数与返回值与printf一样
	// =====================================================================================
	*/
#ifndef LOG_NO_MACRO
#ifndef LOG_REMOVE_DEBUG
#define debug(message, ...) idebug(message, __FUNCTION__, ##__VA_ARGS__)
#else
#define debug(message, ...) ((void)0)
#endif	// ----- #ifndef LOG_REMOVE_DEBUG  -----
#endif	// ----- #ifndef LOG_NO_MACRO  -----
	int idebug(const char *message, const char *func, ...);

private:
	// ==================== PRIVATE METHOD =======================================

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  mkdirs
	//  Description:  递归生成目录
	// =====================================================================================
	*/
	int mkdirs(const char *path, mode_t mode = ~0);

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  create_new_file
	//  Description:  创建新的日志文件
	// =====================================================================================
	*/
	int create_new_file();

	/* 
	// ===  FUNCTION  ======================================================================
	//         Name:  find_exist_file
	//  Description:  查找已经存在的文件，用于继续记录日志
	// =====================================================================================
	*/
	int find_exist_file();

	// ====================  DATA MEMBERS  =======================================
	char dir[256];
	char name_base[256];
	long current_size;
	long file_size;
	long current_num;
	long maintain;
	FILE *log_fp;
	EL_Mutex_Lock locker;

	int output_style[5];
};		// -----  end of class EL_Log  -----

#endif   // ----- #ifndef _ELIB_LOG_H_  -----
