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
#include "../common/RocksDBConnector.h"
#include "../common/MysqlConnector.h"

using grpc::ServerBuilder;

int main()
{
	MYSQL *mysql = MysqlConnector::Create_Mysql_Connect("127.0.0.1", "asunalxh", "013043", "crime");
	MysqlConnector::IntMapper db_raw(mysql, "ciphertext", nullptr, nullptr);
	RocksDBConnector::StringMapper db_search("database/search"), db_update("database/update");

	std::string server_address("0.0.0.0:50051");
	Server service(&db_update, &db_search, &db_raw);

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();

	MysqlConnector::Free_Mysql_Connect(mysql);
}