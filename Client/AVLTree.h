#ifndef AVLTREE_H
#define AVLTREE_H

#include "Oram.h"
#include "../common/Utils.h"
#include "string.h"
#include "Client.h"

class AVLTree
{

private:
	Bid empty_key;
	Oram *oram;
	int height(Bid N, unsigned int &leaf);		//
	int max(int a, int b);						//
	Node *newNode(Bid key, unsigned int value); //
	Node *rightRotate(Node *y);					//
	Node *leftRotate(Node *x);					//
	int getBalance(Node *N);					//

public:
	AVLTree(const unsigned char *key, int _numBucketLeaf, int data_structure, Client *client, bool initial); //

	~AVLTree(); //

	Bid insert(Bid rootKey, unsigned int &pos, Bid key, unsigned int value); //
	Node *search(Bid rootKey, unsigned int rootPos, Bid key);				 //
	void batchSearch(Bid rootKey, unsigned int rootPos, vector<Bid> keys, vector<Node *> *results);
	void startOperation(bool batchWrite = false, bool isWarmStart = true);
	void finishOperation(bool find, Bid &rootKey, unsigned int &rootPos);
};

#endif