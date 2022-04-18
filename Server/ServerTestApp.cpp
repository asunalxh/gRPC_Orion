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

using grpc::ServerBuilder;

int main()
{
    std::string server_address("0.0.0.0:50051");
    Server service(MAX_BUCKET_NUM);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}