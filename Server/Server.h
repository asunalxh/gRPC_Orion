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
	Server(size_t blockNum);
	~Server();
	void Display_Repo();
	void Display_M_I();
	void Display_M_c();


	grpc::Status ReadDB(ServerContext *context, const BytesMessage *req, BytesMessage *resp) override;
	grpc::Status WriteDB(ServerContext *context, const BytesPairMessage *req, GeneralMessage *resp) override;
	grpc::Status GetData(ServerContext *context, const OramMessage *req, BytesMessage *resp) override;
	grpc::Status PutData(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp) override;
	grpc::Status Receive_Encrypted_Doc(ServerContext *context, const BytesPairMessage *req, GeneralMessage *resp) override;
	grpc::Status Retrieve_Encrypted_Doc(ServerContext *context, const BytesMessage *req, BytesMessage *resp) override;

private:
	std::unordered_map<std::string, std::string> M_I;
	std::unordered_map<std::string, std::string> M_c;
	std::unordered_map<std::string, std::string> R_Doc;

	RAMStore *data_search;
	RAMStore *data_update;

	DBConnector *db_update;
	DBConnector *db_search;
	DBConnector *db_info;
};

#endif
