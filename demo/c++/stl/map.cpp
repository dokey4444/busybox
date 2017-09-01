/*
// =====================================================================================
// 
//       Filename:  map.cpp
// 
//    Description:  map属于hash结构，stl内部实现采用红黑树实现
// 
//        Version:  1.0
//        Created:  09/01/2017 05:37:53 PM
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

#include <map>

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	std::map<int, int> map;

	std::cout << "map.size(): " << map.size() << std::endl;
	std::cout << "map.max_size(): "<< map.max_size() << std::endl;
	std::cout << "map.empty(): "<< map.empty() << std::endl;
	std::cout << std::endl;

	// insert
	// 由于map的hash结构，所以没有push一类的前后插入的操作
	// insert可以直接插入，也可以指定位置插入，不过insert指定位置只是给stl一个建议
	// pari形式
	std::cout << "map.insert(): " << std::endl;
	map.insert(std::pair<int, int>(1, 1));
	std::cout << "map.size(): " << map.size() << std::endl;
	// value_type形式
	map.insert(std::map<int, int>::value_type(2, 2));
	std::cout << "map.size(): " << map.size() << std::endl;
	// 相同key会产生覆盖
	map.insert(std::pair<int, int>(1, 1));
	std::cout << "map.size(): " << map.size() << std::endl;
	std::cout << std::endl;

	// erase
	// 即可以使用迭代器，也可以使用key值
	std::cout << "map.erase(): " << std::endl;
	map.erase(2);
	std::cout << "map.size(): " << map.size() << std::endl;
	map.erase(map.begin());
	std::cout << "map.size(): " << map.size() << std::endl;
	std::cout << std::endl;

	// element access
	// 访问未创建的pair会导致程序出core，所以访问之前最好先通过find进行判断
	std::cout << "element access" << std::endl;
	map.insert(std::pair<int, int>(2, 4));
	std::cout << "map[1]: " << map[2] << std::endl;
	std::cout << "map.at(1): " << map.at(2) << std::endl;
	std::cout << std::endl;

	// find
	std::cout << "map.find()" << std::endl;
	std::map<int,int>::iterator it = map.find(2);
	std::cout << "first: " << it->first << std::endl;
	std::cout << "second: " << it->second << std::endl;

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

