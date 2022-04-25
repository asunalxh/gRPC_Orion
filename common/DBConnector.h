#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <string>
using std::string;

template<typename _Id,typename _Value>
class DBConnector
{
public:
	virtual _Value Get(_Id key) = 0;
	virtual void Put(_Id key, _Value value) = 0;
};

#endif