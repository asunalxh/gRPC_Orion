#include "Client.h"

#include <string>
//#include <string.h> // memset(KF, 0, sizeof(KF));
#include "stdio.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
#include <vector>
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
#include <cstring>

using grpc::ClientContext;

Client::Client(std::shared_ptr<Channel> channel, const unsigned char *KF) : stub_(CryptoService::NewStub(channel))
{
	file_reading_counter = 0;
	memcpy(this->KF, KF, ENC_KEY_SIZE);
}

void Client::openFile(const char *addr)
{
	file_counter = 0;
	inFile.open(addr);
}
void Client::closeFile()
{
	inFile.close();
}

void Client::ClientLog()
{

	printf("调用GetData %ld 次 共耗时 %ld ms 发送 %ld 返回 %ld\n", GetDataCount, GetDataTime, GetDataReqBytes, GetDataRespBytes);
	printf("调用PutData %ld 次，共耗时 %ld ms 发送 %ld 返回 %ld\n", PutDataCount, PutDataTime, PutDataReqBytes, PutDataRespBytes);
	printf("传送加密后文件共耗时 %ld ms 发送 %ld 返回 %ld\n", SendEncDocTime, SendEncDocReqBytes, SendEncDocRespBytes);
}

void Client::ServerLog()
{
	ClientContext context;
	GeneralMessage req, resp;
	stub_->ServerLog(&context, req, &resp);
}
void Client::ReadNextPair(docContent *content)
{
	// file_counter++;
	inFile >> file_counter;

	content->id.doc_int = file_counter;

	std::string id_str = std::to_string(file_counter);
	/** convert fileId to char* and record length */
	int doc_id_size = id_str.length();

	content->id.doc_id = (char *)malloc(doc_id_size + 1);
	memcpy(content->id.doc_id, id_str.c_str(), doc_id_size + 1);
	content->id.id_length = doc_id_size;

	std::string str;
	inFile >> str;
	int plaintext_len = str.length();

	content->content = (char *)malloc(plaintext_len + 1);
	memcpy(content->content, str.c_str(), plaintext_len);
	content->content[plaintext_len] = '\0';

	content->content_length = plaintext_len;
}

string Client::ReadDoc(DBConnector<int, string> *conn, int id, docContent *content)
{
	std::ifstream inFile;
	// docContent content;

	std::string id_str = std::to_string(id);
	/** convert fileId to char* and record length */
	int doc_id_size = id_str.length();

	content->id.doc_id = (char *)malloc(doc_id_size + 1);
	memcpy(content->id.doc_id, id_str.c_str(), doc_id_size + 1);
	content->id.id_length = doc_id_size;

	content->id.doc_int = id;

	string value;
	if (conn->Get(id, value))
	{
		content->content_length = value.length();
		content->content = (char *)malloc(content->content_length + 1);
		memcpy(content->content, value.c_str(), content->content_length + 1);
	}

	return value;
}

string Client::Del_GivenDocIndex(DBConnector<int, string> *conn, const int del_index)
{

	string value;
	conn->Get(del_index, value);
	return value;
}

void Client::GetData(int data_structure, size_t index,
					 unsigned char *bucket, size_t bucket_size)
{
	uint64_t startTime = timeSinceEpochMillisec();
	OramMessage req;
	OramBucketMessage resp;

	req.set_data_structure(data_structure);
	req.set_pos(index);
	GetDataReqBytes += req.ByteSizeLong();

	ClientContext context;
	stub_->GetData(&context, req, &resp);
	usleep(1000);

	GetDataRespBytes += resp.ByteSizeLong();

	std::string bucket_str = resp.bucket();

	int len;
	auto bytes = dec_base64(bucket_str.c_str(), bucket_str.length(), &len);

	memcpy(bucket, bytes, len);
	delete[] bytes;

	GetDataCount++;
	uint64_t endTime = timeSinceEpochMillisec();

	GetDataTime += endTime - startTime;

	// memcpy(bucket, bucket_str.c_str(), bucket_str.length());
}

void Client::PutData(int data_structure, size_t index,
					 const unsigned char *data, size_t data_size)
{
	uint64_t startTime = timeSinceEpochMillisec();
	OramBucketMessage req;
	GeneralMessage resp;

	req.set_data_structure(data_structure);
	req.set_pos(index);

	int len;
	auto base64_str = enc_base64(data, data_size, &len);
	req.set_bucket(base64_str);
	delete[] base64_str;

	PutDataReqBytes += req.ByteSizeLong();

	ClientContext context;
	stub_->PutData(&context, req, &resp);
	usleep(1000);

	PutDataRespBytes += resp.ByteSizeLong();

	PutDataCount++;
	uint64_t endTime = timeSinceEpochMillisec();
	PutDataTime += endTime - startTime;
}

void Client::SendEncDoc(const docContent *data)
{
	uint64_t startTime = timeSinceEpochMillisec();
	unsigned char message[data->content_length + AESGCM_MAC_SIZE + AESGCM_IV_SIZE];
	int message_len = enc_aes_gcm(
		KF, (unsigned char *)data->content, data->content_length,
		(unsigned char *)message);

	int len;
	auto str = enc_base64(message, message_len, &len);

	DocMessage req;
	GeneralMessage resp;

	req.set_id(data->id.doc_int);
	// req.set_value(std::string(entry->second.message, entry->second.message_length));
	req.set_value(str);
	delete[] str;

	SendEncDocReqBytes += req.ByteSizeLong();

	ClientContext context;
	stub_->Receive_Encrypted_Doc(&context, req, &resp);

	SendEncDocRespBytes += resp.ByteSizeLong();
	uint64_t endTime = timeSinceEpochMillisec();
	SendEncDocTime += endTime - startTime;
}

string Client::GetEncDoc(int id)
{
	ClientContext context;
	DocIdMessage req;
	DocMessage resp;
	req.set_id(id);

	stub_->Retrieve_Encrypted_Doc(&context, req, &resp);

	int len;
	string base64_str = resp.value();

	// std::cout << base64_str << '\n';

	auto value = dec_base64(base64_str.c_str(), base64_str.length(), &len);

	char *message = new char[len];
	int message_len = dec_aes_gcm(KF, (uint8_t *)value, len, (uint8_t *)message);

	string ans(message, message_len);

	delete[] message;
	delete[] value;
	return ans;
}
