#include "Server.h"
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
// #include "MysqlConnector.h"
#include "RocksDBConnector.h"

Server::Server(size_t blockNum)
{
	R_Doc.clear();
	M_I.clear();
	M_c.clear();

	db_search = new RocksDBConnector("./database/search");
	db_update = new RocksDBConnector("./database/update");

	data_search = new RAMStore(blockNum, db_search);
	data_update = new RAMStore(blockNum, db_update);
}

Server::~Server()
{
	R_Doc.clear();
	M_I.clear();
	M_c.clear();

	delete data_search;
	delete data_update;
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

// display utilities
void Server::Display_Repo()
{

	printf("Display data in Repo\n");
	for (auto it = R_Doc.begin(); it != R_Doc.end(); ++it)
	{
		printf("Cipher\n");
		printf("%s\n", (it->first).c_str());
		print_bytes((uint8_t *)(it->second).c_str(), (uint32_t)it->second.length());
	}
}

void Server::Display_M_I()
{

	std::unordered_map<std::string, std::string>::iterator it;
	printf("Print data in M_I\n");
	for (it = M_I.begin(); it != M_I.end(); ++it)
	{
		printf("u \n");
		print_bytes((uint8_t *)(it->first).c_str(), (uint32_t)it->first.length());
		printf("v \n");
		print_bytes((uint8_t *)(it->second).c_str(), (uint32_t)it->second.length());
	}
}

void Server::Display_M_c()
{
	std::unordered_map<std::string, std::string>::iterator it;
	printf("Print data in M_c\n");
	for (it = M_c.begin(); it != M_c.end(); ++it)
	{
		printf("u \n");
		print_bytes((uint8_t *)(it->first).c_str(), (uint32_t)it->first.length());
		printf("v \n");
		print_bytes((uint8_t *)(it->second).c_str(), (uint32_t)it->second.length());
	}
}

grpc::Status Server::Receive_Encrypted_Doc(ServerContext *context, const BytesPairMessage *req, GeneralMessage *resp)
{

	std::string id = req->key();
	std::string enc_content = req->value();
	R_Doc.insert(std::pair<std::string, std::string>(id, enc_content));

	// MysqlConnector mysql;
	// mysql.insertValue(id.c_str(),enc_content.c_str());

	return grpc::Status::OK;
}

grpc::Status Server::Retrieve_Encrypted_Doc(ServerContext *context, const BytesMessage *req, BytesMessage *resp)
{
	std::string key = req->byte();
	resp->set_byte(R_Doc.at(key));
	return grpc::Status::OK;
}