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
using grpc::Status;

class Server final : public CryptoService::Service
{
public:
    Server();
    ~Server();
    void Display_Repo();
    void Display_M_I();
    void Display_M_c();


    void InitData(size_t blockNum);

    Status GetData(ServerContext *context, const OramMessage *req, BytesMessage *resp) override;
    Status PutData(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp) override;
	Status Receive_Encrypted_Doc(ServerContext *context, const BytesPairMessage *req, GeneralMessage *resp) override;
	Status Retrieve_Encrypted_Doc(ServerContext* context, const BytesMessage* req, BytesMessage* resp) override;

private:
    std::unordered_map<std::string, std::string> M_I;
    std::unordered_map<std::string, std::string> M_c;
    std::unordered_map<std::string, std::string> R_Doc;

    RAMStore *data_search;
    RAMStore *data_update;
};

#endif
