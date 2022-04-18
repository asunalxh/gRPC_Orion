/***
 * Demonstrate Client
 * maintain a current Kf
 * read documents in a given directory and give one by one to App.cpp with <fileId, array of words>
 * develop utility to enc and dec file with a given key kf
 * issue a random update operation (op,in) to App
 * issue a random keyword search
 */
#ifndef CLIENT_H
#define CLIENT_H

#include "../common/data_type.h"
#include "../common/Utils.h"
#include <vector>
#include <memory>
#include "../common/crypto.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

using namespace crypto;
using grpc::Channel;

class Client
{
public:
    Client(std::shared_ptr<Channel> channel);
    void ReadNextDoc(docContent *fetch_data);
    void Del_GivenDocIndex(const int del_index, docContent *fetch_data);

    void getKFValue(unsigned char *outKey);
    void EncryptDoc(const docContent *data, entry *encrypted_doc);
    void DecryptDocCollection(std::vector<std::string> Res);

    void GetData(int data_structure, size_t index,
                 unsigned char *bucket, size_t bucket_size);
    void PutData(int data_structure, size_t index,
                 const unsigned char *data, size_t data_size);

	void SendEncDoc(entry* entry);

private:
    unsigned char KF[ENC_KEY_SIZE];
    int file_reading_counter;
    std::unique_ptr<CryptoService::Stub> stub_;
};

#endif