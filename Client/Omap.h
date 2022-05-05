#ifndef OMAP_H
#define OMAP_H

#include "AVLTree.h"
#include "Client.h"
using namespace std;

class OMAP
{
private:
	AVLTree *treeHandler;
	Bid rootKey;
	unsigned int rootPos;
	Client *myClient;
	int data_structure;

public:
	OMAP(const unsigned char *key, int _numBucketLeaf, int data_structure, Client *client, bool initial);
	~OMAP();
	void insert(Bid key, unsigned int value);
	unsigned int find(Bid key);

	void batchInsert(map<Bid, unsigned int> pairs);
	vector<unsigned int> batchSearch(vector<Bid> keys);

	void storeInfo();
	void debug(const unsigned char *key, int _numBucketLeaf, int data_structure, Client *client, bool initial);
};

#endif /* OMAP_H */
