/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  protobuf用法
//                  官方文档：https://developers.google.com/protocol-buffers/docs/proto
// 
//        Version:  1.0
//        Created:  08/23/2015 11:16:26 PM
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
#include <fstream>
#include <sstream>
#include <string>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/type_resolver.h>                                                     
#include <google/protobuf/util/type_resolver_util.h>

#include <google/protobuf/io/zero_copy_stream.h>                                                    

#include "pb2sample.pb.h"

static const char kTypeUrlPrefix[] = "type.googleapis.com";

/* 
// ===  FUNCTION  ======================================================================
//         Name:  get_type_url
//  Description:   
// =====================================================================================
*/
static std::string get_type_url(const google::protobuf::Descriptor* message)
{
	return std::string(kTypeUrlPrefix) + "/" + message->full_name();
}		// -----  end of static function get_type_url  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  pb_to_json
//  Description:   
// =====================================================================================
*/
bool pb_to_json(const google::protobuf::Message &message, std::string *json, bool pretty)
{
	google::protobuf::scoped_ptr<google::protobuf::util::TypeResolver> resolver;
	resolver.reset(google::protobuf::util::NewTypeResolverForDescriptorPool(
				kTypeUrlPrefix, google::protobuf::DescriptorPool::generated_pool()));

	google::protobuf::util::JsonOptions options;
	options.add_whitespace = pretty;
	if (!BinaryToJsonString(resolver.get(),
				get_type_url(message.GetDescriptor()),
				message.SerializeAsString(),
				json, options).ok()) {
		return false;
	} else {
		return true;
	}
}		// -----  end of function pb_to_json  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  json_to_pb
//  Description:   
// =====================================================================================
*/
bool json_to_pb(const std::string &json, google::protobuf::Message *message)
{
	google::protobuf::scoped_ptr<google::protobuf::util::TypeResolver> resolver;
	resolver.reset(google::protobuf::util::NewTypeResolverForDescriptorPool(
				kTypeUrlPrefix, google::protobuf::DescriptorPool::generated_pool()));

	std::string binary;
	if (!JsonToBinaryString(resolver.get(),
				get_type_url(message->GetDescriptor()),
				json, &binary).ok()) {
		return false;
	}

	return message->ParseFromString(binary);
}		// -----  end of function json_to_pb  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// 1、protobuf类型的set get方法
	// 详细的用法，可以参照'*.pb.h'文件中的生命进行使用
	sample::BasicType basic_type;
	basic_type.set_b(true);
	basic_type.set_i32(32);
	basic_type.set_i64(64);
	basic_type.set_s("string");

	// 2、protobuf的基本读写方式
	//
	// PB读写文件的方式有很多，其中最最基本的方式有：
	//
	// Heavy I/O
	// bool	ParseFromIstream(istream * input) [message.h]
	// bool	SerializePartialToOstream(ostream * output) const [message.h]
	// bool	ParseFromFileDescriptor(int file_descriptor) [message.h]
	// bool	SerializeToFileDescriptor(int file_descriptor) const [message.h]
	// 详见：https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.message
	//
	// Lite I/O
	// bool	ParseFromString(const string & data) [message_lite.h]
	// bool	SerializeToString(string * output) const [message_lite.h]
	// 详见：https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.message_lite

	std::ofstream output("pb2sample.data");
	if (!basic_type.SerializeToOstream(&output)) {
		std::cout << "failed to serialize file." << std::endl;
		return EXIT_FAILURE;
	}
	output.flush();

	basic_type.Clear();	//Message的Clear方法用于清空这个Message下所有数据（注意Clear首字母C要大写）
	std::ifstream input("pb2sample.data");
	if (input) {
		if (!basic_type.ParseFromIstream(&input)) {
			std::cout << "failed to parse file." << std::endl;
			return EXIT_FAILURE;
		}
	} else {
		std::cout << "file not found." << std::endl;
		return EXIT_FAILURE;
	}
	assert(basic_type.b() == true);
	assert(basic_type.i32() == 32);
	assert(basic_type.i64() == 64);
	assert(basic_type.s() == "string");

	// protobuf和json的转换
	std::string json;
	pb_to_json(basic_type, &json, true);
	std::cout << json << std::endl;
	basic_type.Clear();	//Message的Clear方法用于清空这个Message下所有数据（注意Clear首字母C要大写）
	json_to_pb(json, &basic_type);
	assert(basic_type.b() == true);
	assert(basic_type.i32() == 32);
	assert(basic_type.i64() == 64);
	assert(basic_type.s() == "string");

	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

