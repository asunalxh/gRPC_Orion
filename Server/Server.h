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

class Server final : public CryptoService::Service
{
public:
	Server();
	~Server();

	grpc::Status GetData(ServerContext *context, const OramMessage *req, OramBucketMessage *resp) override;
	grpc::Status PutData(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp) override;
	grpc::Status Receive_Encrypted_Doc(ServerContext *context, const DocMessage *req, GeneralMessage *resp) override;
	grpc::Status ServerLog(ServerContext *context, const GeneralMessage *req, GeneralMessage *resp) override;
	grpc::Status Fill(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp)

	// grpc::Status GetData(const OramMessage *req, OramBucketMessage *resp);
	// grpc::Status PutData(const OramBucketMessage *req, GeneralMessage *resp);

	// grpc::Status Receive_Encrypted_Doc(const DocMessage *req, GeneralMessage *resp);

	void ServerLog();
	void ClearLog();

private:
	uint64_t GetDataTime = 0;
	uint64_t PutDataTime = 0;
	uint64_t ReceiveEncDocTime = 0;

	std::unordered_map<int, std::string> R_Doc;

	RAMStore *data_search;
	RAMStore *data_update;
};

#endif
