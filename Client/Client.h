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
#include "../common/DBConnector.h"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

using namespace crypto;
using grpc::Channel;
using std::string;

class Client
{
public:
	Client(std::shared_ptr<Channel> channel, const unsigned char *KF);
	std::vector<string> ReadDoc(int id, docContent *fetch_data);
	string ReadDoc(DBConnector<int,string> * conn,int id, docContent *fetch_data);

	std::vector<string> Del_GivenDocIndex(const int del_index);
	string Del_GivenDocIndex(DBConnector<int, string> *conn,const int del_index);


	void GetData(int data_structure, size_t index,
				 unsigned char *bucket, size_t bucket_size);
	void PutData(int data_structure, size_t index,
				 const unsigned char *data, size_t data_size);

	void SendEncDoc(const docContent *data);
	string GetEncDoc(int id);

private:
	unsigned char KF[ENC_KEY_SIZE];
	int file_reading_counter;
	std::unique_ptr<CryptoService::Stub> stub_;
};

#endif