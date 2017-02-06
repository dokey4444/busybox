/*
// =====================================================================================
// 
//       Filename:  proc.cpp
// 
//    Description:  proc文件系统(c++11)
// 
//        Version:  1.0
//        Created:  01/19/2017 02:49:38 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#include <cstdlib>
#include <iostream>

#include <sstream>
#include <fstream>
#include <string>

/* 
// ===  FUNCTION  ======================================================================
//         Name:  read_cmdline
//  Description:   
// =====================================================================================
*/
void read_cmdline(int pid)
{
	std::ostringstream oss_filename;
	oss_filename << "/proc/" << pid << "/cmdline";
	std::ifstream ifs_file(oss_filename.str());
	std::string line;
	while (getline(ifs_file, line)) {
		std::cout << line << std::endl;
		std::istringstream iss_line(line);
		std::string tok;
		while (getline(iss_line, tok, '\0')) {
			std::cout << tok << std::endl;
		}
	}
	ifs_file.close();
}		// -----  end of function read_cmdline  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cerr << "Usage: proc $PID\n" << std::endl;
		return EXIT_FAILURE;
	}

	int pid = atoi(argv[1]);
	std::cout << "pid = [" << pid << "]" << std::endl;

	read_cmdline(pid);

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

