#pragma once

constexpr int BIT_SIZE = 32;

class bitstream
{
public:
	using bit = bitset<1>;

	bitstream(fstream&);
	bitstream() = delete;
	void write(bit b);
	bit read();

private:
	fstream& __fs;
	int wcount = 0;
	int rcount = 0;
	//unsigned buff = 0;
	bitset<BIT_SIZE> buff;
};