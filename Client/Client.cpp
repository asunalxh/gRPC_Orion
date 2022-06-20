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

Client::Client(Server *server, const unsigned char *KF) : stub_(server)
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

	printf("调用GetData %lld 次 共耗时 %lld ns 发送 %lld 返回 %lld\n", GetDataCount, GetDataTime, GetDataReqBytes, GetDataRespBytes);
	printf("调用PutData %lld 次，共耗时 %lld ns 发送 %lld 返回 %lld\n", PutDataCount, PutDataTime, PutDataReqBytes, PutDataRespBytes);
	printf("传送加密后文件共耗时 %lld ns 发送 %lld 返回 %lld\n", SendEncDocTime, SendEncDocReqBytes, SendEncDocRespBytes);
}

void Client::ClearLog()
{
	GetDataCount = 0;
	GetDataTime = 0;
	GetDataReqBytes = 0;
	GetDataRespBytes = 0;
	PutDataCount = 0;
	PutDataTime = 0;
	PutDataReqBytes = 0;
	PutDataRespBytes = 0;
	SendEncDocTime = 0;
	SendEncDocReqBytes = 0;
	SendEncDocRespBytes = 0;
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
	// int plaintext_len = str.length() - 1;
	int plaintext_len = str.length();

	content->content = (char *)malloc(plaintext_len + 1);
	memcpy(content->content, str.c_str(), plaintext_len);
	content->content[plaintext_len] = '\0';

	content->content_length = plaintext_len;
}

void Client::GetData(int data_structure, size_t index,
					 unsigned char *bucket, size_t bucket_size)
{

	OramMessage req;
	OramBucketMessage resp;

	req.set_data_structure(data_structure);
	req.set_pos(index);
	GetDataReqBytes += req.ByteSizeLong();

	uint64_t startTime = timeSinceEpochMillisec();
	stub_->GetData(&req, &resp);
	// usleep(1000);
	uint64_t endTime = timeSinceEpochMillisec();

	GetDataRespBytes += resp.ByteSizeLong();

	std::string bucket_str = resp.bucket();

	int len;
	auto bytes = dec_base64(bucket_str.c_str(), bucket_str.length(), &len);

	memcpy(bucket, bytes, len);
	delete[] bytes;

	GetDataCount++;
	GetDataTime += endTime - startTime;

	// memcpy(bucket, bucket_str.c_str(), bucket_str.length());
}

void Client::PutData(int data_structure, size_t index,
					 const unsigned char *data, size_t data_size)
{

	OramBucketMessage req;
	GeneralMessage resp;

	req.set_data_structure(data_structure);
	req.set_pos(index);

	int len;
	auto base64_str = enc_base64(data, data_size, &len);
	req.set_bucket(base64_str);
	delete[] base64_str;

	PutDataReqBytes += req.ByteSizeLong();

	uint64_t startTime = timeSinceEpochMillisec();
	stub_->PutData(&req, &resp);
	uint64_t endTime = timeSinceEpochMillisec();

	PutDataRespBytes += resp.ByteSizeLong();

	PutDataCount++;
	PutDataTime += endTime - startTime;
}

void Client::SendEncDoc(const docContent *data)
{

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

	uint64_t startTime = timeSinceEpochMillisec();
	stub_->Receive_Encrypted_Doc(&req, &resp);
	uint64_t endTime = timeSinceEpochMillisec();

	SendEncDocRespBytes += resp.ByteSizeLong();
	SendEncDocTime += endTime - startTime;
}
