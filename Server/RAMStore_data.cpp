#include "RAMStore_data.h"
#include <iostream>
#include <cstring>
#include <string>
#include "../common/Utils.h"
using namespace std;

RAMStore::RAMStore(DBConnector<int, string> *conn)
{
	this->conn = conn;
	if(conn == nullptr){
		this->data.resize(MAX_BUCKET_NUM);
		this->emptyNodes = MAX_BUCKET_NUM;
	}
}

RAMStore::~RAMStore()
{
}

BUCKET RAMStore::Read(size_t pos)
{
	if (conn == NULL){
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
