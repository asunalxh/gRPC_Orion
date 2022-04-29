#include <string>
#include "stdio.h"
#include "stdlib.h"

#include "../common/MysqlConnector.h"
#include "../common/data_type.h"
#include "../common/Utils.h"
#include "Client.h"
#include "Orion.h"

// for measurement
#include <cstdint>
#include <chrono>
#include <iostream>
#include <fstream>

// end for measurement

Client *myClient;
Orion *orion;

unsigned char KW[ENC_KEY_SIZE];
unsigned char KC[ENC_KEY_SIZE];
unsigned char KF[ENC_KEY_SIZE];

void initKey(unsigned char *key, string filename)
{
	ifstream in(filename, ios::binary);

	if (in.good())
		in.read((char *)key, ENC_KEY_SIZE);
	else
	{
		ofstream out(filename, ios::binary);
		read_rand(key, ENC_KEY_SIZE);
		out.write((char *)key, ENC_KEY_SIZE);
		out.close();
	}

	in.close();
}

void search()
{
	printf("\n======== Start To Search ========\n");
	// std::string s_keyword[10]= {"the","of","and","to","a","in","for","is","on","that"};
	// std::string s_keyword[] = {"start", "plan", "work", "set", "bitch"};
	// std::string s_keyword[] = {"BATTERY", "THEFT"};
	std::string s_keyword[] = {"Brand#13", "Brand#11"};

	for (int s_i = 0; s_i < 2; s_i++)
	{
		printf("\nSearching ==> %s\n", s_keyword[s_i].c_str());

		auto res = orion->search(s_keyword[s_i].c_str(), s_keyword[s_i].size());

		for (auto id : res)
		{
			std::cout << id << ' ' << myClient->GetEncDoc(id) << '\n';
		}
	}
}

void db_add(DBConnector<int, string> *reader, int start, int end)
{
	printf("\n======== Start Adding ========\n");

	// Update Protocol with op = add
	for (int i = start; i <= end; i++)
	{
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		auto word = myClient->ReadDoc(reader, i, fetch_data);

		myClient->SendEncDoc(fetch_data);

		orion->batch_addDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, word);

		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		// do this one to flush doc by doc enclave to flush all documents in OMAP to server
		if (i % 1000 == 0)
		{
			printf("\n-------- Processing insertion %d --------\n", i);
		}
	}
	printf("\n======== Finished Adding ========\n");
}

void db_del(DBConnector<int, string> *reader, int del_no)
{
	// Update Protocol with op = del (id)
	printf("\n======== Deleting doc ========\n");

	for (int del_index = 1; del_index <= del_no; del_index++)
	{
		string id_str = to_string(del_index);
		auto word = myClient->Del_GivenDocIndex(reader, del_index);

		if (del_index % 1000 == 0)
		{
			printf("\n-------- Processing deleting docs %d --------\n", del_index);
		}

		orion->batch_delDoc(id_str.c_str(), id_str.length(), del_index, word);

		// later need to free fetch_data
	}
	printf("\n======== Finish deleting all docs ========\n");
}

void doc_addDoc(int start, int end)
{
	printf("\n======== Start Adding doc ========\n");

	// Update Protocol with op = add
	for (int i = start; i <= end; i++)
	{
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		auto keywords = myClient->ReadDoc(i, fetch_data);

		myClient->SendEncDoc(fetch_data);

		for (auto word : keywords)
			orion->batch_addDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, word);

		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		// do this one to flush doc by doc enclave to flush all documents in OMAP to server
		if (i % 1000 == 0)
		{
			printf("\n-------- Processing insertion %d --------\n", i);
		}
	}
	printf("\n======== Finished Adding Doc ========\n");
}

void doc_delDoc(int del_no)
{
	// Update Protocol with op = del (id)
	printf("\n======== Deleting doc ========\n");

	for (int del_index = 1; del_index <= del_no; del_index++)
	{
		string id_str = to_string(del_index);
		auto keywords = myClient->Del_GivenDocIndex(del_index);

		if (del_index % 1000 == 0)
		{
			printf("\n-------- Processing deleting docs %d --------\n", del_index);
		}
		for (auto word : keywords)
			orion->batch_delDoc(id_str.c_str(), id_str.length(), del_index, word);

		// later need to free fetch_data
	}
	printf("\n======== Finish deleting all docs ========\n");
}

int main()
{

	initKey(KW, "KW");
	initKey(KC, "KC");
	initKey(KF, "KF");

	myClient = new Client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()), KF);

	printf("\n======== Create Orion ========\n");
	orion = new Orion(myClient, KW, KC, 10);

	// doc_addDoc(1, 20);
	// doc_delDoc(3);

	auto mysql = MysqlConnector::Create_Mysql_Connect("127.0.0.1", "asunalxh", "013043", "tpch");
	MysqlConnector::CacheReader reader(mysql, 10, "PART", "P_PARTKEY", "P_BRAND");
	db_add(&reader, 1, 20);
	db_del(&reader, 3);
	MysqlConnector::Free_Mysql_Connect(mysql);

	printf("\n======== Flushing ========\n");
	orion->flush(false);

	search();

	// free omap and client and server
	delete orion;
	delete myClient;

	return 0;
}