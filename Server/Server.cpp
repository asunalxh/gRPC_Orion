#include "Server.h"
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
// #include "MysqlConnector.h"
#include "../common/Utils.h"

Server::Server(DBConnector<int, string> *db_update,
			   DBConnector<int, string> *db_search,
			   DBConnector<int, string> *db_raw_data)
{
	this->db_raw_data = db_raw_data;
	data_search = new RAMStore(db_search);
	data_update = new RAMStore(db_update);
}


grpc::Status Server::ServerLog(ServerContext *context, const GeneralMessage *req, GeneralMessage *resp){
	printf("调用GetData共耗时 %ld ms\n",  GetDataTime);
	printf("调用PutData共耗时 %ld ms\n",  PutDataTime);
	printf("接受加密数据文件共耗时 %ld ms\n", ReceiveEncDocTime);
}

Server::~Server()
{
	R_Doc.clear();

	delete data_search;
	delete data_update;
}

grpc::Status Server::GetData(ServerContext *context, const OramMessage *req, OramBucketMessage *resp)
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

grpc::Status Server::PutData(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp)
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

grpc::Status Server::Receive_Encrypted_Doc(ServerContext *context, const DocMessage *req, GeneralMessage *resp)
{
	uint64_t startTime = timeSinceEpochMillisec();
	int id = req->id();
	std::string enc_content = req->value();
	if (db_raw_data == nullptr)
	{
		R_Doc.insert({id, enc_content});
	}
	else
	{
		db_raw_data->Put(id, enc_content);
	}

	uint64_t endTime = timeSinceEpochMillisec();
	this->ReceiveEncDocTime += endTime - startTime;
	return grpc::Status::OK;
}

grpc::Status Server::Retrieve_Encrypted_Doc(ServerContext *context, const DocIdMessage *req, DocMessage *resp)
{
	int key = req->id();
	if (db_raw_data == nullptr)
		resp->set_value(R_Doc.at(key));
	else
	{
		string value;
		db_raw_data->Get(key, value);
		resp->set_value(value);
	}

	return grpc::Status::OK;
}