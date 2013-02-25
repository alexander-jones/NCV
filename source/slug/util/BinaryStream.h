#pragma once

#include <fstream>
#include <string>
#include <iostream>

namespace slug
{

namespace util
{

class Unique;

/**
	Like fstream, but overloads the << operator to write out binary data
	instead.
*/
template<std::fstream::openmode mode>
class BinaryStream : public std::fstream
{
public:
	BinaryStream(const std::string& path)
		: std::fstream(path.c_str(), mode)
	{
		if (fail())
		{
			std::cerr << "Failed to open " << path << std::endl;
		}
	}

	BinaryStream()
	{
	}

	void open(const char* path)
	{
		std::fstream::open(path, mode);
	}

	/**
		Writes the value out in binary.
	*/
	template<typename T>
	BinaryStream& operator<<(const T& v)
	{
		write((const char*)&v, sizeof(T));
		return *this;
	}

	/**
		String overload. Writes the length followed by the contents without
		the terminating zero.
	*/
	BinaryStream& operator<<(const std::string& v)
	{
		unsigned int length = v.length();
		write((const char*)&length, sizeof(unsigned int));
		write(v.c_str(), length);
		return *this;
	}

	BinaryStream& operator<<(const char* v)
	{
		return *this << std::string(v);
	}

	/**
		util::Unique overload. Converts the value into an unsigned int.
	*/
	BinaryStream& operator<<(const util::Unique& v)
	{
		unsigned int u = v;
		write((const char*)&u, sizeof(unsigned int));
		return *this;
	}

	/**
		Reads a value in in binary.
	*/
	template<typename T>
	BinaryStream& operator>>(T& v)
	{
		read((char*)&v, sizeof(T));
		return *this;
	}

	/**
		String overload. Reads the length followed by the contents without
		the terminating zero.
	*/
	BinaryStream& operator>>(std::string& v)
	{
		unsigned int length;
		*this >> length;
		char* s = new char[length + 1];
		s[length] = 0;
		read(s, length);
		v = s;
		delete [] s;
		return *this;
	}

	/**
		util::Unique overload.
	*/
	BinaryStream& operator>>(Unique& v)
	{
		unsigned int value;
		*this >> value;
		v = Unique(value);
	}
};

typedef BinaryStream<std::fstream::out> OutputStream;
typedef BinaryStream<std::fstream::in> InputStream;

} //namespace spec

} //namespace slug
