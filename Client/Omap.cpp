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
}


OMAP::~OMAP()
{
	delete treeHandler;
}

unsigned int OMAP::find(Bid key)
{
	Bid empty_key;

	if (rootKey == empty_key)
	{
		return 0;
	}

	treeHandler->startOperation(false);
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

	}
	 treeHandler->finishOperation(false,rootKey, rootPos);
}

vector<unsigned int> OMAP::batchSearch(vector<Bid> keys)
{

	vector<unsigned int> result;

	treeHandler->startOperation(false);

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
