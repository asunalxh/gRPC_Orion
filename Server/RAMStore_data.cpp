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

RAMStore::RAMStore(DBConnector<int, string> *conn)
{
	this->conn = conn;
}

RAMStore::~RAMStore()
{
}

BUCKET RAMStore::Read(size_t pos)
{
	if (conn == NULL){
		cout << "read with map\n";
		return data.at(pos);
	}
		
	string value;
	if (conn->Get(pos, value))
	{
		return StringToBucket(value);
	}
	return BUCKET();
}

void RAMStore::Write(size_t pos, BUCKET b)
{
	if (conn == NULL){
		cout << "write with map\n";
		data[pos] = b;
	}
		
	else
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
