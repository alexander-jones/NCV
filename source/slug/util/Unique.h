/*
	Unique.h
	Dispenser of unique unsigned int IDs.
*/
#pragma once

namespace slug
{

namespace util
{

/**
	A helper class that hands out unique unsigned integer IDs.
*/
class Unique
{
public:
	///Constructor
	Unique()
		: _value(0)
	{
	}

	///Constructor using unsigned int. Users should be careful using this.
	Unique(unsigned int v)
		: _value(v)
	{
	}

	///Assignment to another unique
	Unique& operator=(const Unique& rhs)
	{
		_value = rhs._value;
		return *this;
	}

	///Prevent assignment to unsigned int
	Unique& operator=(const unsigned int& rhs) = delete;

	///Convert to an unsigned int
	inline operator unsigned int() const
	{
		return _value;
	}

	inline bool operator==(const Unique& rhs) const
	{
		return _value == rhs._value;
	}

	inline bool operator<(const Unique& rhs) const
	{
		return _value < rhs._value;
	}

	/**
		For each call, returns a unique unsigned integer. Reentrant.
	*/
	static Unique generate();

	/**
		Forces all Uniques generated after this call to be higher than the
		input value.
	*/
	static void setMinimum(unsigned int value);
	
private:
	unsigned int _value;
};

}

}
