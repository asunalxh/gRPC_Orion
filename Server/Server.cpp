#include "Server.h"
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
// #include "MysqlConnector.h"

Server::Server()
{
	R_Doc.clear();
	M_I.clear();
	M_c.clear();
}

Server::~Server()
{
	R_Doc.clear();
	M_I.clear();
	M_c.clear();

	delete data_search;
	delete data_update;
}

void Server::InitData(size_t blockNum)
{
	data_search = new RAMStore(blockNum);
	data_update = new RAMStore(blockNum);
}

Status Server::GetData(ServerContext *context, const OramMessage *req, BytesMessage *resp)
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

	return Status::OK;
}

Status Server::PutData(ServerContext *context, const OramBucketMessage *req, GeneralMessage *resp)
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
	return Status::OK;
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
