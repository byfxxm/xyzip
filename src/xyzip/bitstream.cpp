#include "pch.h"
#include "bitstream.h"

bitstream::bitstream(fstream& fs) : __fs(fs)
{
	assert(__fs.is_open());
}

void bitstream::write(bit b)
{
	buff[wcount++] = b[0];

	if (wcount == BIT_SIZE)
	{
		__fs.write(buff.to_string().c_str(), wcount / 8);
		wcount = 0;
		buff.reset();
	}

}

bitstream::bit bitstream::read()
{

}