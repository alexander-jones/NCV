#pragma once
struct Bit
{
	static unsigned int mask(unsigned int position)
	{
		return 0x80000000 >> bit(position);
	}

	static unsigned int word(unsigned int index)
	{
		return index >> 5;
	}

	static unsigned int bit(unsigned int index)
	{
		return index & 0x1F;
	}
	
	static unsigned int extract(const unsigned int word, unsigned int position)
	{
		return (word << position) & 0x80000000;
	}

	static unsigned int rextract(const unsigned int word, unsigned int position)
	{
		return (word << position) >> 31;
	}
};

