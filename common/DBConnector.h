#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <string>
using std::string;

template <typename _Id, typename _Value>
class DBConnector
{
public:
	virtual bool Get(const _Id& key, _Value &value) = 0;
	virtual bool Put(const _Id& key, const _Value& value) = 0;
	virtual bool Delete(const _Id& key) = 0;
};

template <typename _Id, typename _Value>
class DBMap : public DBConnector<_Id, _Value>
{
public:
	class Iterator
	{
	public:
		Iterator(const _Id& id, DBConnector<_Id, _Value> *conn);
		operator _Value();
		Iterator operator++(int);
		Iterator operator--(int);
		Iterator operator=(const _Value& x);
		_Value operator+(const _Value& x);
		_Value operator-(const _Value& x);
		bool operator>(const _Value& x);
		bool operator<(const _Value& x);
		bool operator==(const _Value& x);

	private:
		DBConnector<_Id, _Value> *conn;
		_Id id;
		_Value value;
	};

	virtual Iterator operator[](const _Id& id);

	virtual int count(const _Id& id);
	virtual void erase(const _Id& id);
};

#define _TEMPLATE template <typename _Id, typename _Value>
#define _DBMAP DBMap<_Id, _Value>

_TEMPLATE
typename _DBMAP::Iterator _DBMAP::operator[](const _Id& id)
{
	return _DBMAP::Iterator(id, this);
}

_TEMPLATE
int _DBMAP::count(const _Id& id)
{
	_Value value;
	if (this->Get(id, value))
	{
		return 1;
	}
	return 0;
}

_TEMPLATE
void _DBMAP::erase(const _Id& id)
{
	this->Delete(id);
}

_TEMPLATE
_DBMAP::Iterator::Iterator(const _Id& id, DBConnector<_Id, _Value> *conn)
{
	this->id = id;
	this->conn = conn;

	conn->Get(id, this->value);
}

_TEMPLATE
_DBMAP::Iterator::operator _Value()
{
	return this->value;
}

_TEMPLATE
typename _DBMAP::Iterator _DBMAP::Iterator::operator++(int)
{
	this->value++;
	conn->Put(id, value);
	return *this;
}
_TEMPLATE
typename _DBMAP::Iterator _DBMAP::Iterator::operator--(int)
{
	this->value--;
	conn->Put(id, value);
	return *this;
}

_TEMPLATE
_Value _DBMAP::Iterator::operator+(const _Value& x)
{
	return this->value + x;
}

_TEMPLATE
_Value _DBMAP::Iterator::operator-(const _Value& x)
{
	return this->value - x;
}

_TEMPLATE
typename _DBMAP::Iterator _DBMAP::Iterator::operator=(const _Value& x)
{
	this->value = x;
	conn->Put(id, value);
	return *this;
}

_TEMPLATE
bool _DBMAP::Iterator::operator>(const _Value& x)
{
	return this->value > x;
}

_TEMPLATE
bool _DBMAP::Iterator::operator<(const _Value& x)
{
	return this->value < x;
}

_TEMPLATE
bool _DBMAP::Iterator::operator==(const _Value& x)
{
	return this->value == x;
}

#endif