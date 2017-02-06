/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  leveldb的文档参见leveldb目录下doc目录
// 
//        Version:  1.0
//        Created:  05/26/2016 02:55:25 PM
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

#include "leveldb/db.h"

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	// options.error_if_exists = true;
	leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);
	if (!status.ok()) {
		std::cerr << status.ToString() << std::endl;
		return EXIT_FAILURE;
	}

	leveldb::Status s;
	std::string key("key");
	std::string value("v");
	s = db->Put(leveldb::WriteOptions(), key, "value"); assert(s.ok());
	s = db->Get(leveldb::ReadOptions(), key, &value); assert(s.ok());
	std::cout << key << ": " << value << std::endl;
	s = db->Delete(leveldb::WriteOptions(), key); assert(s.ok());
	s = db->Get(leveldb::ReadOptions(), key, &value); assert(!s.ok());

	delete db;

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

