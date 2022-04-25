#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <string>
using std::string;

template<class K,class V>
class DBConnector
{
public:
	virtual V Get(K key) = 0;
	virtual void Put(K key, V value) = 0;
};

#endif