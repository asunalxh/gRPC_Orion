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

void Client::getKFValue(unsigned char *outKey)
{
	memcpy(outKey, KF, ENC_KEY_SIZE);
}

void Client::ReadNextDoc(docContent *content)
{
	std::ifstream inFile;
	std::stringstream strStream;
	// docContent content;

	// increase counter
	file_reading_counter += 1;

	std::string fileName;
	fileName = std::to_string(file_reading_counter);
	/** convert fileId to char* and record length */
	int doc_id_size = fileName.length() + 1;

	content->id.doc_id = (char *)malloc(doc_id_size);
	memcpy(content->id.doc_id, fileName.c_str(), doc_id_size);
	content->id.id_length = doc_id_size;

	content->id.doc_int = file_reading_counter;

	// read the file content
	inFile.open(raw_doc_dir + fileName);
	strStream << inFile.rdbuf();
	inFile.close();

	/** convert document content to char* and record length */
	std::string str = strStream.str();
	int plaintext_len;
	plaintext_len = str.length() + 1;

	content->content = (char *)malloc(plaintext_len);
	memcpy(content->content, str.c_str(), plaintext_len);

	content->content_length = plaintext_len;

	strStream.clear();
}

void Client::Del_GivenDocIndex(const int del_index, docContent *content)
{

	std::ifstream inFile;
	std::stringstream strStream;

	std::string fileName;
	fileName = std::to_string(del_index);
	/** convert fileId to char* and record length */
	int doc_id_size = fileName.length() + 1;

	content->id.doc_id = (char *)malloc(doc_id_size);
	memcpy(content->id.doc_id, fileName.c_str(), doc_id_size);
	content->id.id_length = doc_id_size;

	content->id.doc_int = del_index;

	// read the file content
	inFile.open(raw_doc_dir + fileName);
	strStream << inFile.rdbuf();
	inFile.close();

	/** convert document content to char* and record length */
	std::string str = strStream.str();
	int plaintext_len;
	plaintext_len = str.length() + 1;

	content->content = (char *)malloc(plaintext_len);
	memcpy(content->content, str.c_str(), plaintext_len);

	content->content_length = plaintext_len;

	strStream.clear();
}

void Client::EncryptDoc(const docContent *data, entry *encrypted_doc)
{

	memcpy(encrypted_doc->first.content, data->id.doc_id, data->id.id_length);
	encrypted_doc->second.message_length = enc_aes_gcm((unsigned char *)data->content,
													   data->content_length, KF,
													   (unsigned char *)encrypted_doc->second.message);
}

void Client::DecryptDocCollection(std::vector<std::string> Res)
{

	for (auto &&enc_doc : Res)
	{

		int original_len;
		unsigned char *plaintext = (unsigned char *)malloc((enc_doc.size() - AESGCM_MAC_SIZE - AESGCM_IV_SIZE) * sizeof(unsigned char));
		original_len = dec_aes_gcm((unsigned char *)enc_doc.c_str(), enc_doc.size(), KF, plaintext);

		// std::string doc_i((char*)plaintext,original_len);
		// printf("Plain doc ==> %s\n",doc_i.c_str());
	}
}

void Client::GetData(int data_structure, size_t index,
					 unsigned char *bucket, size_t bucket_size)
{
	ClientContext context;
	OramMessage req;
	BytesMessage resp;

	req.set_data_structure(data_structure);
	req.set_pos(index);

	stub_->GetData(&context, req, &resp);

	std::string bucket_str = resp.byte();
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

void Client::SendEncDoc(entry *entry)
{
	ClientContext context;
	BytesPairMessage req;
	GeneralMessage resp;
	req.set_key(std::string(entry->first.content, entry->first.content_length));
	req.set_value(std::string(entry->second.message, entry->second.message_length));

	stub_->Receive_Encrypted_Doc(&context, req, &resp);
}

string Client::ReadDB(string key)
{
	ClientContext context;
	BytesMessage req, resp;
	req.set_byte(key);

	stub_->ReadDB(&context, req, &resp);

	return resp.byte();
}
void Client::WriteDB(string key, string value)
{
	ClientContext context;
	BytesPairMessage req;
	GeneralMessage resp;
	req.set_key(key);
	req.set_value(value);

	stub_->WriteDB(&context, req, &resp);
}