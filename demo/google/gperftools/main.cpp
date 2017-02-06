/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  06/12/2016 09:11:57 PM
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

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	int *p = new int(0);
	for(int i = 0; i < 100000000; ++i) {
		*p = i;
	}
	delete p;
	int *q = new int(0);
	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

