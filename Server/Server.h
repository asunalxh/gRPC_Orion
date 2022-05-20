#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "../common/data_type.h"
#include "../common/crypto.grpc.pb.h"
#include "../common/Utils.h"

#include "RAMStore_data.h"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

using namespace crypto;
using grpc::ServerContext;

class Server
{
public:
	Server();
	~Server();

	grpc::Status GetData(const OramMessage *req, OramBucketMessage *resp);
	grpc::Status PutData(const OramBucketMessage *req, GeneralMessage *resp);

	grpc::Status Receive_Encrypted_Doc(const DocMessage *req, GeneralMessage *resp);
	grpc::Status ServerLog(const GeneralMessage *req, GeneralMessage *resp);

private:
	uint64_t GetDataTime = 0;
	uint64_t PutDataTime = 0;
	uint64_t ReceiveEncDocTime = 0;

	std::unordered_map<int, std::string> R_Doc;

	RAMStore *data_search;
	RAMStore *data_update;
};

#endif
