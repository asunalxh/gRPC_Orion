#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <string>
using std::string;

template <typename _Id, typename _Value>
class DBConnector
{
public:
	virtual bool Get(_Id key, _Value &value) = 0;
	virtual bool Put(_Id key, _Value value) = 0;
	virtual bool Delete(_Id key) = 0;
};

template <typename _Id, typename _Value>
class DBMap : public DBConnector<_Id, _Value>
{
public:
	class Iterator
	{
	public:
		Iterator(_Id id, DBConnector<_Id, _Value> *conn);
		operator _Value();
		Iterator operator++(int);
		Iterator operator--(int);
		Iterator operator=(_Value x);
		_Value operator+(_Value x);
		_Value operator-(_Value x);
		bool operator>(_Value x);
		bool operator<(_Value x);
		bool operator==(_Value x);

	private:
		DBConnector<_Id, _Value> *conn;
		_Id id;
		_Value value;
	};

	virtual Iterator operator[](_Id id);

	virtual int count(_Id id);
	virtual void erase(_Id id);
};

#define _TEMPLATE template <typename _Id, typename _Value>
#define _DBMAP DBMap<_Id, _Value>

_TEMPLATE
typename _DBMAP::Iterator _DBMAP::operator[](_Id id)
{
	return _DBMAP::Iterator(id, this);
}

_TEMPLATE
int _DBMAP::count(_Id id)
{
	_Value value;
	if (this->Get(id, value))
	{
		return 1;
	}
	return 0;
}

_TEMPLATE
void _DBMAP::erase(_Id id)
{
	this->Delete(id);
}

_TEMPLATE
_DBMAP::Iterator::Iterator(_Id id, DBConnector<_Id, _Value> *conn)
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
_Value _DBMAP::Iterator::operator+(_Value x)
{
	return this->value + x;
}

_TEMPLATE
_Value _DBMAP::Iterator::operator-(_Value x)
{
	return this->value - x;
}

_TEMPLATE
typename _DBMAP::Iterator _DBMAP::Iterator::operator=(_Value x)
{
	this->value = x;
	conn->Put(id, value);
	return *this;
}

_TEMPLATE
bool _DBMAP::Iterator::operator>(_Value x)
{
	return this->value > x;
}

_TEMPLATE
bool _DBMAP::Iterator::operator<(_Value x)
{
	return this->value < x;
}

_TEMPLATE
bool _DBMAP::Iterator::operator==(_Value x)
{
	return this->value == x;
}

#endif