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
	// RocksDBConnector::IntMapper db_search("database/search"), db_update("database/update");

	std::string server_address("0.0.0.0:50052");
	Server service;

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
}
