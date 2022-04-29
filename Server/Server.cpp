#include "Server.h"
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
// #include "MysqlConnector.h"

Server::Server(DBConnector<string, string> *db_update,
			   DBConnector<string, string> *db_search, DBConnector<int, string> *db_raw_data)
{
	this->db_search = db_search;
	this->db_update = db_update;
	this->db_raw_data = db_raw_data;
	data_search = new RAMStore(db_search);
	data_update = new RAMStore(db_update);
}

Server::~Server()
{
	R_Doc.clear();

	delete data_search;
	delete data_update;
}

grpc::Status Server::ReadInfo(ServerContext *context, const BytesMessage *req, BytesMessage *resp)
{
	std::string key = req->byte();
	int data_structure = req->data_structure();
	std::string value;
	if (data_structure == 1)
		db_search->Get(key, value);
	else
		db_update->Get(key, value);

	resp->set_byte(value);

	return grpc::Status::OK;
}

grpc::Status Server::WriteInfo(ServerContext *context, const BytesPairMessage *req, GeneralMessage *resp)
{
	std::string key = req->key();
	std::string value = req->value();
	int data_structure = req->data_structure();

	if (data_structure == 1)
		db_search->Put(key, value);
	else
		db_update->Put(key, value);

	return grpc::Status::OK;
}

grpc::Status Server::GetData(ServerContext *context, const OramMessage *req, BytesMessage *resp)
{
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
	resp->set_byte(bucket_str);

	return grpc::Status::OK;
}

grpc::Status Server::PutData(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp)
{
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
	return grpc::Status::OK;
}

grpc::Status Server::Receive_Encrypted_Doc(ServerContext *context, const DocMessage *req, GeneralMessage *resp)
{

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