#include "Server.h"
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
// #include "MysqlConnector.h"

Server::Server(size_t blockNum)
{
	R_Doc.clear();
	M_I.clear();
	M_c.clear();

	db_search = new RocksDBConnector::IntMapper("./database/search");
	db_update = new RocksDBConnector::IntMapper("./database/update");
	db_info = new RocksDBConnector::StringMapper("./database/info");

	MYSQL *mysql = MysqlConnector::Create_Mysql_Connect(
		"127.0.0.1",
		"user",
		"123456",
		"test");
	db_raw_data = new MysqlConnector::StringMapper(mysql, "test");
	//db_raw_data = new RocksDBConnector::StringMapper("./database/raw");

	data_search = new RAMStore(db_search);
	data_update = new RAMStore(db_update);
}

Server::~Server()
{
	R_Doc.clear();
	M_I.clear();
	M_c.clear();

	delete data_search;
	delete data_update;
}

grpc::Status Server::ReadInfo(ServerContext *context, const BytesMessage *req, BytesMessage *resp)
{
	std::string key = req->byte();

	std::string value = db_info->Get(key);
	resp->set_byte(value);

	return grpc::Status::OK;
}

grpc::Status Server::WriteInfo(ServerContext *context, const BytesPairMessage *req, GeneralMessage *resp)
{
	std::string key = req->key();
	std::string value = req->value();
	db_info->Put(key, value);

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
	if (db_raw_data == nullptr)
	{
		R_Doc.insert(std::pair<std::string, std::string>(id, enc_content));
	}
	else
	{
		//int len;
		//auto str = enc_base64((uint8_t *)enc_content.c_str(), enc_content.length(), &len);
		//db_raw_data->Put(id, str);

		db_raw_data->Put(id, enc_content);
	}

	return grpc::Status::OK;
}

grpc::Status Server::Retrieve_Encrypted_Doc(ServerContext *context, const BytesMessage *req, BytesMessage *resp)
{
	std::string key = req->byte();
	if (db_raw_data == nullptr)
		resp->set_byte(R_Doc.at(key));
	else
	{
		//int len;
		//std::string base64_str = db_raw_data->Get(key);
		//auto value = dec_base64(base64_str.c_str(), base64_str.length(), &len);
		//resp->set_byte(std::string((char *)value, len));

		resp->set_byte(db_raw_data->Get(key));
	}

	return grpc::Status::OK;
}