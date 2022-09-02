#include "RAMStore_data.h"
#include <iostream>
#include <cstring>
#include <string>
#include "../common/Utils.h"
using namespace std;

RAMStore::RAMStore()
{
	this->data.resize(MAX_BUCKET_NUM);
	this->emptyNodes = MAX_BUCKET_NUM;
}

RAMStore::~RAMStore()
{
}

BUCKET RAMStore::Read(size_t pos)
{
	return data.at(pos);
}

void RAMStore::Write(size_t pos, BUCKET b)
{
	data[pos] = b;
}

// void RAMStore::ReduceEmptyNumbers()
//{
//	emptyNodes--;
// }

// size_t RAMStore::GetEmptySize()
//{
//	return emptyNodes;
// }
