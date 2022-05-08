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

uint64_t timeSinceEpochMillisec()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

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

void db_add(DBConnector<int, string> *reader, int start, int end)
{
	printf("\n======== Start Adding ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	// Update Protocol with op = add
	for (int i = start; i <= end; i++)
	{
		printf("Add No.%d\n", i);
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		auto word = myClient->ReadDoc(reader, i, fetch_data);

		myClient->SendEncDoc(fetch_data);

		orion->addDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, word);

		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		// do this one to flush doc by doc enclave to flush all documents in OMAP to server
	}
	uint64_t endTime = timeSinceEpochMillisec();
	printf("\n======== It takes %ldms to End  ========\n", endTime - startTime);
}

void db_del(DBConnector<int, string> *reader, int del_no)
{
	// Update Protocol with op = del (id)
	printf("\n======== Deleting doc ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	for (int del_index = 1; del_index <= del_no; del_index++)
	{
		printf("Del No.%d\n", del_index);
		string id_str = to_string(del_index);
		auto word = myClient->Del_GivenDocIndex(reader, del_index);

		if (del_index % 1000 == 0)
		{
			printf("\n-------- Processing deleting docs %d --------\n", del_index);
		}

		orion->delDoc(id_str.c_str(), id_str.length(), del_index, word);

		// later need to free fetch_data
	}
	uint64_t endTime = timeSinceEpochMillisec();
	printf("\n======== It takes %ldms to End  ========\n", endTime - startTime);
}

void doc_addDoc(int start, int end)
{
	printf("\n======== Start Adding doc ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	// Update Protocol with op = add
	for (int i = start; i <= end; i++)
	{
		printf("Add No.%d\n", i);
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		auto keywords = myClient->ReadDoc(i, fetch_data);

		myClient->SendEncDoc(fetch_data);

		for (auto word : keywords)
		{
			orion->addDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, word);
		}
		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		// do this one to flush doc by doc enclave to flush all documents in OMAP to server
	}
	uint64_t endTime = timeSinceEpochMillisec();
	printf("\n======== It takes %ldms to End  ========\n", endTime - startTime);
}

void doc_delDoc(int del_no)
{
	// Update Protocol with op = del (id)
	printf("\n======== Deleting doc ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	for (int del_index = 1; del_index <= del_no; del_index++)
	{
		printf("Del No.%d\n", del_index);
		string id_str = to_string(del_index);
		auto keywords = myClient->Del_GivenDocIndex(del_index);

		for (auto word : keywords)
			orion->delDoc(id_str.c_str(), id_str.length(), del_index, word);

		// later need to free fetch_data
	}
	uint64_t endTime = timeSinceEpochMillisec();
	printf("\n======== It takes %ldms to End  ========\n", endTime - startTime);
}

void search()
{
	printf("\n======== Start To Search ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	// std::string s_keyword[] = {"start", "plan", "work", "set", "bitch"};
	// std::string s_keyword[] = {"BATTERY", "THEFT"};
	std::string s_keyword[] = {"Brand#13", "Brand#11"};

	for (int s_i = 0; s_i < 1; s_i++)
	{
		printf("\nSearching ==> %s\n", s_keyword[s_i].c_str());

		auto res = orion->search(s_keyword[s_i].c_str(), s_keyword[s_i].size());

		printf("result size %ld \n", res.size());
		// for (auto id : res)
		//{
		//	std::cout << id << ' ' << myClient->GetEncDoc(id) << '\n';
		// }
	}
	uint64_t endTime = timeSinceEpochMillisec();
	printf("\n======== It takes %ldms to End  ========\n", endTime - startTime);
}

int main()
{

	initKey(KW, "KW");
	initKey(KC, "KC");
	initKey(KF, "KF");

	myClient = new Client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()), KF);

	uint64_t startTime_1 = timeSinceEpochMillisec();

	printf("\n======== Create Orion ========\n");
	orion = new Orion(myClient, KW, KC, 15);

	uint64_t startTime_2 = timeSinceEpochMillisec();

	// doc_addDoc(1, 100000);
	// doc_delDoc(10000);

	//读取TPCH原始数据的 Host, User, Password, Database
	auto mysql = MysqlConnector::Create_Mysql_Connect("127.0.0.1", "asunalxh", "013043", "tpch");

	//读取数据的 Table，id所在列名，value所在列名
	// 数字表示一次性从数据库读多少数据到本地
	MysqlConnector::CacheReader reader(mysql, 10000, "PART", "P_PARTKEY", "P_BRAND");
	db_add(&reader, 1, 100000);
	db_del(&reader, 10000);
	MysqlConnector::Free_Mysql_Connect(mysql);

	search();

	uint64_t endTime = timeSinceEpochMillisec();
	printf("包括初始化总用时：%ldms\n", endTime - startTime_1);
	printf("插入、删除、查询总用时：%ldms\n", endTime - startTime_2);

	// free omap and client and server
	delete orion;
	delete myClient;

	return 0;
}