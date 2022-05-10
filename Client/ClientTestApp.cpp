#include <string>
#include "stdio.h"
#include "stdlib.h"

#include "../common/MysqlConnector.h"
#include "../common/data_type.h"
#include "../common/Utils.h"
#include "Client.h"
#include "Orion.h"

// for measurement

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

uint64_t addDoc(int add_no)
{
	printf("\n======== Start Adding ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	// Update Protocol with op = add
	for (int i = 1; i <= add_no; i++)
	{
		printf("Add No.%d\n", i);
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		myClient->ReadNextPair(fetch_data);

		myClient->SendEncDoc(fetch_data);

		orion->addDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, fetch_data->content);

		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		// do this one to flush doc by doc enclave to flush all documents in OMAP to server
	}
	uint64_t endTime = timeSinceEpochMillisec();
	return endTime - startTime;
}

uint64_t delDoc(int del_no)
{
	// Update Protocol with op = del (id)
	printf("\n======== Deleting doc ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	for (int del_index = 1; del_index <= del_no; del_index++)
	{
		printf("Del No.%d\n", del_index);
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		myClient->ReadNextPair(fetch_data);

		orion->delDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, fetch_data->content);

		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		// later need to free fetch_data
	}
	uint64_t endTime = timeSinceEpochMillisec();
	return endTime - startTime;
}

void search()
{
	printf("\n======== Start To Search ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	// std::string s_keyword[] = {"start", "plan", "work", "set", "bitch"};
	std::string s_keyword[] = {"THEFT"};
	// std::string s_keyword[] = {"Brand#13", "Brand#11"};

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
	orion = new Orion(myClient, KW, KC, 5);

	uint64_t startTime_2 = timeSinceEpochMillisec();

	myClient->openFile("/backup/luna-dataset/crimePtype.txt");
	addDoc(100);
	myClient->closeFile();

	search();

	myClient->openFile("/backup/luna-dataset/crimePtype.txt");
	delDoc(10);

	search();

	uint64_t endTime = timeSinceEpochMillisec();
	printf("包括初始化总用时：%ldms\n", endTime - startTime_1);
	printf("插入、删除、查询总用时：%ldms\n", endTime - startTime_2);

	// free omap and client and server
	delete orion;
	delete myClient;

	return 0;
}