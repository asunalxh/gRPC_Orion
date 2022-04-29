#include "Omap.h"
#include <stdio.h>

#include "Oram.h"
#include "../common/data_type.h"
#include "../common/data_type2.h"
#include <fstream>

OMAP::OMAP(const unsigned char *key, int _numBucketLeaf, int data_structure, Client *client, bool initial)
{
	treeHandler = new AVLTree(key, _numBucketLeaf, data_structure, client, initial);
	this->myClient = client;
	this->data_structure = data_structure;

	if (!initial)
	{
		rootKey = (const unsigned char *)myClient->ReadInfo("rootkey", data_structure).c_str();
		rootPos = stoi(myClient->ReadInfo("rootpos", data_structure));

		// std::ifstream in("OmapInfo_" + to_string(data_structure));
		// string base64_str;
		// in >> this->rootPos >> base64_str;
		// in.close();

		// int len;
		// auto temp = dec_base64(base64_str.c_str(), base64_str.length(), &len);
		// this->rootKey = temp;
		// delete[] temp;
	}
}

void OMAP::storeInfo()
{
	myClient->WriteInfo("rootkey", string((char *)rootKey.key, ENTRY_HASH_KEY_LEN_256), data_structure);
	myClient->WriteInfo("rootpos", to_string(rootPos), data_structure);
	// std::ofstream out("OmapInfo_" + to_string(data_structure));
	// int len;
	// auto temp = dec_base64((const char*)rootKey.key,ENTRY_HASH_KEY_LEN_256,&len);
	// out << rootPos << '\n' << temp;
	// delete[] temp;
}

OMAP::~OMAP()
{
	this->storeInfo();
	delete treeHandler;
}

unsigned int OMAP::find(Bid key)
{
	Bid empty_key;

	if (rootKey == empty_key)
	{
		return 0;
	}

	treeHandler->startOperation(false, true);
	auto resNode = treeHandler->search(rootKey, rootPos, key);
	unsigned int res = 0;
	if (resNode != NULL)
	{
		res = resNode->value;
	}
	treeHandler->finishOperation(true, rootKey, rootPos);
	return res;
}

void OMAP::insert(Bid key, unsigned int value)
{

	treeHandler->startOperation(false, true);
	Bid empty_key;
	if (rootKey == empty_key)
	{
		// printf("OMAP::insert insert empty Key");
		rootKey = treeHandler->insert(empty_key, rootPos, key, value);
	}
	else
	{
		// printf("OMAP::insert not insert empty Key");
		rootKey = treeHandler->insert(rootKey, rootPos, key, value);
	}
	treeHandler->finishOperation(false, rootKey, rootPos);
	// printf("OMAP::insert finish finalisation");
}

void OMAP::batchInsert(map<Bid, unsigned int> pairs, bool isWarmStart)
{

	treeHandler->startOperation(true, isWarmStart);
	Bid empty_key;

	int count = 0;

	for (auto pair : pairs)
	{

		if (rootKey == empty_key)
		{
			rootKey = treeHandler->insert(empty_key, rootPos, pair.first, pair.second);
		}
		else
		{
			rootKey = treeHandler->insert(rootKey, rootPos, pair.first, pair.second);
		}

		count++;

		if (count % 100000 == 0 || count == pairs.size())
		{
			printf("Processing search batch %d\n", count);
			treeHandler->finishOperation(false, rootKey, rootPos);
			treeHandler->startOperation(true, isWarmStart);
		}
	}
	// treeHandler->finishOperation(false,rootKey, rootPos);
}

vector<unsigned int> OMAP::batchSearch(vector<Bid> keys)
{

	vector<unsigned int> result;

	treeHandler->startOperation(false, true);

	vector<Node *> resNodes;

	treeHandler->batchSearch(rootKey, rootPos, keys, &resNodes);
	for (Node *n : resNodes)
	{
		unsigned int res;
		if (n != NULL)
		{
			result.push_back(n->value);
		}
		else
		{
			result.push_back(0);
		}
	}

	treeHandler->finishOperation(true, rootKey, rootPos);
	return result;
}
