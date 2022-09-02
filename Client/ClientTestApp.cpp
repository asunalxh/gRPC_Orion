#include <string>
#include "stdio.h"
#include "stdlib.h"

#include "../common/data_type.h"
#include "../common/Utils.h"
#include "Client.h"
#include "Orion.h"

// for measurement

#include <iostream>
#include <fstream>

// end for measurement

Client *myClient;
Server *myServer;
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

inline uint64_t addDoc(int add_no)
{
	uint64_t startTime = timeSinceEpochMillisec();
	// Update Protocol with op = add
	for (int i = 1; i <= add_no; i++)
	{
		// printf("Add No.%d\n", i);
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		myClient->ReadNextPair(fetch_data);

		myClient->SendEncDoc(fetch_data);

		orion->addDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, fetch_data->content);

		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		if (i % 10000 == 0)
		{
			printf("\n-------- add %d files--------\n", i);
		}

		// do this one to flush doc by doc enclave to flush all documents in OMAP to server
	}
	uint64_t endTime = timeSinceEpochMillisec();
	return endTime - startTime;
}

inline uint64_t delDoc(int del_no)
{
	// Update Protocol with op = del (id)
	printf("\n======== Deleting doc ========\n");
	uint64_t startTime = timeSinceEpochMillisec();
	for (int del_index = 1; del_index <= del_no; del_index++)
	{
		// printf("Del No.%d\n", del_index);
		docContent *fetch_data;
		fetch_data = (docContent *)malloc(sizeof(docContent));

		myClient->ReadNextPair(fetch_data);

		orion->delDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int, fetch_data->content);

		// free memory
		free(fetch_data->content);
		free(fetch_data->id.doc_id);
		free(fetch_data);

		if (del_index % 10000 == 0)
		{

			// printf("\n-------- del %d files --------\n", del_index);
			uint64_t time_comsume = timeSinceEpochMillisec() - startTime;
			printf("\n--------del %d files Time: %ld ms --------\n", del_index, time_comsume);
			myClient->ClientLog();
			myServer->ServerLog();
		}

		// later need to free fetch_data
	}
	uint64_t endTime = timeSinceEpochMillisec();
	return endTime - startTime;
}

inline uint64_t search(const string keyword)
{
	printf("\n======== Start To Search ========\n");
	uint64_t startTime = timeSinceEpochMillisec();

	auto res = orion->search(keyword.c_str(), keyword.size());

	printf("result size %ld \n", res.size());

	uint64_t endTime = timeSinceEpochMillisec();
	return endTime - startTime;
}

int main(int argc, char *argv[])
{
	// int del_percent = stoi(argv[1]);

	// char *file = argv[1];
	// int num_leaf = stoi(argv[2]);
	// int add_num = stoi(argv[3]);
	// printf("%s\n", file);
	//  sprintf(file, "/home/asunalxh/data/delete-search/wiki-10w_delete-%d%%_refer-1k.txt", del_percent);

	initKey(KW, "KW");
	initKey(KC, "KC");
	initKey(KF, "KF");

	// const char* file = "/home/asunalxh/data/lineshipdate.txt";
	// const char *file = "/home/asunalxh/data/crimePtype.txt";
	const char *file = "/home/asunalxh/data/enron.txt";
	// const char *file = "/home/asunalxh/data/wiki.txt";

	// myClient = new Client(grpc::CreateChannel("182.92.127.18:50052", grpc::InsecureChannelCredentials()), KF);
	//  myClient = new Client(grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials()), KF);
	// myServer = new Server();
	// myClient = new Client(myServer, KF);
	myClient = new Client(grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials()), KF);
	myClient->openFile(file);

	uint64_t startTime = timeSinceEpochMillisec();

	printf("\n======== Start Addition ========\n");
	orion = new Orion(myClient, KW, KC, 14);

	uint64_t endTime = timeSinceEpochMillisec();

	printf("初始化： %ld ns\n", endTime - startTime);
	myClient->ClientLog();
	myClient->ServerLog();
	myClient->ClearLog();

	startTime = timeSinceEpochMillisec();
	uint64_t calculate_time = addDoc(100);

	endTime = timeSinceEpochMillisec();
	printf("插入总用时： %ld ns\n", calculate_time);
	myClient->ClientLog();
	myClient->ServerLog();

	search("includ");

	// myClient->ClearLog();
	// myServer->ClearLog();

	// myClient->closeFile();
	// myClient->openFile(file);

	// delDoc(90000);
	//  delDoc(1000 * del_percent);

	// myClient->ClearLog();
	// myServer->ClearLog();
	// myClient->closeFile();
	// for (int i = 2; i <= 7; i++)
	//{
	//	char *keyword = argv[i];
	//	uint64_t time_comsume = search(keyword);
	//	printf("搜索共耗时 %lld ns\n", time_comsume);
	//	myClient->ClientLog();
	//	myServer->ServerLog();
	//	myClient->ClearLog();
	//	myServer->ClearLog();
	// }

	// free omap and client and server
	delete orion;
	delete myClient;
	// delete myServer;

	return 0;
}