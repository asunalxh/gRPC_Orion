#include "Omap.h"
#include <stdio.h>

#include "Oram.h"
#include "../common/data_type.h"
#include "../common/data_type2.h"

const std::string rootkey_id = "rootkey";
const std::string rootpos_id = "rootpos";

OMAP::OMAP(const unsigned char *key, int _numBucketLeaf, int data_structure, Client *client, bool initial)
{
	treeHandler = new AVLTree(key, _numBucketLeaf, data_structure, client, initial);
	this->myClient = client;

	if (!initial)
	{
		rootKey = (const unsigned char *)myClient->ReadInfo(rootkey_id).c_str();
		rootPos = stoi(myClient->ReadInfo(rootpos_id));
	}
}

OMAP::~OMAP()
{
	free(treeHandler);
}

unsigned int OMAP::find(Bid key)
{
	Bid empty_key;

	if (rootKey == empty_key)
	{
		return 0;
	}

	treeHandler->startOperation(false);
	Node *node = new Node();
	node->key = rootKey;
	node->pos = rootPos;
	auto resNode = treeHandler->search(node, key);
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

	treeHandler->startOperation(false);
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

void OMAP::batchInsert(map<Bid, unsigned int> pairs)
{

	treeHandler->startOperation(true);
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
			treeHandler->startOperation(true);

			myClient->WriteInfo(rootkey_id, string((char *)rootKey.key, ENTRY_HASH_KEY_LEN_128));
			myClient->WriteInfo(rootpos_id, to_string(rootPos));
		}
	}
	// treeHandler->finishOperation(false,rootKey, rootPos);
}

vector<unsigned int> OMAP::batchSearch(vector<Bid> keys)
{

	vector<unsigned int> result;

	treeHandler->startOperation(false);

	Node *node = new Node();
	node->key = rootKey;
	node->pos = rootPos;

	vector<Node *> resNodes;

	treeHandler->batchSearch(node, keys, &resNodes);
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
