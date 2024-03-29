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

std::vector<string> Client::ReadDoc(int id, docContent *content)
{
	std::ifstream inFile;
	std::stringstream strStream;
	// docContent content;

	std::string id_str = std::to_string(id);
	/** convert fileId to char* and record length */
	int doc_id_size = id_str.length();

	content->id.doc_id = (char *)malloc(doc_id_size + 1);
	memcpy(content->id.doc_id, id_str.c_str(), doc_id_size + 1);
	content->id.id_length = doc_id_size;

	content->id.doc_int = id;

	// read the file content
	inFile.open(raw_doc_dir + id_str + ".txt");
	strStream << inFile.rdbuf();
	inFile.close();

	/** convert document content to char* and record length */
	std::string str = strStream.str();
	int plaintext_len;
	plaintext_len = str.length();

	content->content = (char *)malloc(plaintext_len + 1);
	memcpy(content->content, str.c_str(), plaintext_len + 1);

	content->content_length = plaintext_len;

	strStream.clear();

	std::vector<string> keyword;
	int word_num = 0;
	inFile.open(keyword_dir + id_str + ".txt");
	inFile >> word_num;

	while (word_num--)
	{
		string word;
		inFile >> word;
		keyword.push_back(word);
	}
	inFile.close();

	return keyword;
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

std::vector<string> Client::Del_GivenDocIndex(const int del_index)
{

	std::ifstream inFile;

	std::string fileName;
	fileName = std::to_string(del_index);

	std::vector<string> keyword;
	int word_num = 0;
	inFile.open(keyword_dir + fileName + ".txt");
	inFile >> word_num;

	while (word_num--)
	{
		string word;
		inFile >> word;
		keyword.push_back(word);
	}
	inFile.close();

	return keyword;
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
	ClientContext context;
	OramMessage req;
	OramBucketMessage resp;

	req.set_data_structure(data_structure);
	req.set_pos(index);

	stub_->GetData(&context, req, &resp);

	std::string bucket_str = resp.bucket();

	memcpy(bucket, bucket_str.c_str(), bucket_str.length());
}

void Client::PutData(int data_structure, size_t index,
					 const unsigned char *data, size_t data_size)
{
	ClientContext context;
	OramBucketMessage req;
	GeneralMessage resp;

	req.set_data_structure(data_structure);
	req.set_pos(index);
	req.set_bucket(std::string((const char *)data, data_size));

	stub_->PutData(&context, req, &resp);
}

void Client::SendEncDoc(const docContent *data)
{

	unsigned char message[data->content_length + AESGCM_MAC_SIZE + AESGCM_IV_SIZE];
	int message_len = enc_aes_gcm(
		KF, (unsigned char *)data->content, data->content_length,
		(unsigned char *)message);

	int len;
	auto str = enc_base64(message, message_len, &len);

	ClientContext context;
	DocMessage req;
	GeneralMessage resp;

	req.set_id(data->id.doc_int);
	// req.set_value(std::string(entry->second.message, entry->second.message_length));
	req.set_value(str);
	delete[] str;

	stub_->Receive_Encrypted_Doc(&context, req, &resp);
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
