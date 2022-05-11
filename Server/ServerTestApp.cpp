#include "Server.h"

#include <iostream>
#include <string>
#include <memory>
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "../common/crypto.grpc.pb.h"
#include "../common/MysqlConnector.h"

using grpc::ServerBuilder;

int main()
{
	//存储加密后原文的数据库 Host, User, Password, Database
	MYSQL *mysql = MysqlConnector::Create_Mysql_Connect("localhost", "asunalxh", "123456", "orion");
	//存储Oram的 Table，id所在列名（nullptr 默认为 id），value所在列名（nullptr 默认为 value）
	MysqlConnector::IntMapper db_raw(mysql, "ciphertext", nullptr, nullptr),
		db_search(mysql, "oram_search", nullptr, nullptr),
		db_update(mysql, "oram_update", nullptr, nullptr);

	// RocksDBConnector::IntMapper db_search("database/search"), db_update("database/update");

	std::string server_address("0.0.0.0:50052");
	Server service(&db_update, &db_search, &db_raw);

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();

	MysqlConnector::Free_Mysql_Connect(mysql);
}