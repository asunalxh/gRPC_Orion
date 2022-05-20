#include "Server.h"
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
// #include "MysqlConnector.h"
#include "../common/Utils.h"
#include <fstream>

Server::Server()
{
	data_search = new RAMStore();
	data_update = new RAMStore();
}

grpc::Status Server::ServerLog(const GeneralMessage *req, GeneralMessage *resp)
{

	printf("调用GetData共耗时 %ld ms\n", GetDataTime);
	printf("调用PutData共耗时 %ld ms\n", PutDataTime);
	printf("接受加密数据文件共耗时 %ld ms\n", ReceiveEncDocTime);

	return grpc::Status::OK;
}

Server::~Server()
{
	R_Doc.clear();

	delete data_search;
	delete data_update;
}

grpc::Status Server::GetData(const OramMessage *req, OramBucketMessage *resp)
{
	uint64_t startTime = timeSinceEpochMillisec();
	int data_structure = req->data_structure();
	size_t pos = req->pos();
	BUCKET bucket;
	if (data_structure == 1)
	{
		bucket = data_search->Read(pos);
	}
	else
	{
		bucket = data_update->Read(pos);
	}
	std::string bucket_str = BucketToString(bucket);

	resp->set_data_structure(data_structure);
	resp->set_pos(pos);
	resp->set_bucket(bucket_str);

	uint64_t endTime = timeSinceEpochMillisec();
	this->GetDataTime += endTime - startTime;

	return grpc::Status::OK;
}

grpc::Status Server::PutData(const OramBucketMessage *req, GeneralMessage *resp)
{
	uint64_t startTime = timeSinceEpochMillisec();
	int data_structure = req->data_structure();
	size_t pos = req->pos();
	std::string bucket_str = req->bucket();
	BUCKET b = StringToBucket(bucket_str);

	if (data_structure == 1)
	{
		data_search->Write(pos, b);
	}
	else
	{
		data_update->Write(pos, b);
	}
	uint64_t endTime = timeSinceEpochMillisec();
	this->PutDataTime += endTime - startTime;
	return grpc::Status::OK;
}

grpc::Status Server::Receive_Encrypted_Doc(const DocMessage *req, GeneralMessage *resp)
{
	uint64_t startTime = timeSinceEpochMillisec();
	int id = req->id();
	std::string enc_content = req->value();

	uint64_t endTime = timeSinceEpochMillisec();
	this->ReceiveEncDocTime += endTime - startTime;
	return grpc::Status::OK;
}
