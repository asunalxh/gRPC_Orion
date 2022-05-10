#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "../common/data_type.h"
#include "../common/crypto.grpc.pb.h"
#include "../common/Utils.h"
#include "../common/DBConnector.h"

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
	Server(DBConnector<int, string> *db_update,
		   DBConnector<int, string> *db_search, DBConnector<int, string> *db_raw_data);
	~Server();

	grpc::Status GetData(ServerContext *context, const OramMessage *req, OramBucketMessage *resp) override;
	grpc::Status PutData(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp) override;

	grpc::Status Receive_Encrypted_Doc(ServerContext *context, const DocMessage *req, GeneralMessage *resp) override;
	grpc::Status Retrieve_Encrypted_Doc(ServerContext *context, const DocIdMessage *req, DocMessage *resp) override;

private:
	std::unordered_map<int, std::string> R_Doc;

	DBConnector<int, string> *db_raw_data = nullptr;

	RAMStore *data_search;
	RAMStore *data_update;
};

#endif
