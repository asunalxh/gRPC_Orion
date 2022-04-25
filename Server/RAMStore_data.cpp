#include "RAMStore_data.h"
#include <iostream>
#include <cstring>
#include <string>
#include "../common/Utils.h"
using namespace std;

RAMStore::RAMStore(size_t count)
	: data(count), emptyNodes(count)
{
}

RAMStore::RAMStore(DBConnector<int,string> *conn)
{
	this->conn = conn;
}

RAMStore::~RAMStore()
{
}

BUCKET RAMStore::Read(size_t pos)
{
	if (conn == NULL)
		return data.at(pos);

	string value = conn->Get(pos);
	return StringToBucket(value);
}

void RAMStore::Write(size_t pos, BUCKET b)
{
	if (conn == NULL)
		data[pos] = b;
	conn->Put(pos, BucketToString(b));
}

// void RAMStore::ReduceEmptyNumbers()
//{
//	emptyNodes--;
// }

// size_t RAMStore::GetEmptySize()
//{
//	return emptyNodes;
// }
