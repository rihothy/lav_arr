#ifndef _LAV_ARR_CV_HPP
#define _LAV_ARR_CV_HPP

#include "arr.hpp"

#include <fstream>
#include <cstring>
#include <string>

inline auto imread(const std::string& path)
{
	auto file = std::ifstream(path, std::ios::binary);
	auto buff = new char[40];

	file.read(buff, 14);
	file.read(buff, 40);

	auto w = *(int*)(buff + 4);
	auto h = *(int*)(buff + 8);
	auto real_w = (w * 3 + 3) / 4 * 4;

	auto img = lav::Arr<uint8_t>({ h, w, 3 });

	for (auto i = 0; i < h; ++i)
	{
		file.read((char*)(img.data() + i * w * 3), w * 3);
		file.read(buff, real_w - w * 3);
	}

	delete[] buff;

	return img;
}

template<typename T>
inline auto imwrite(const std::string& path, const lav::Arr<T>& img)
{
	auto file = std::ofstream(path, std::ios::binary);
	auto buff = new char[40](0);
	auto h = img.shape[0];
	auto w = img.shape[1];
	auto real_w = (w * 3 + 3) / 4 * 4;

	buff[0] = 'B', buff[1] = 'M';
	*(int*)(buff + 2) = h * real_w + 54;
	*(int*)(buff + 10) = 54;

	file.write(buff, 14);

	*(int*)(buff) = 40;
	*(int*)(buff + 4) = w;
	*(int*)(buff + 8) = h;
	*(short*)(buff + 12) = 1;
	*(short*)(buff + 14) = 24;
	*(int*)(buff + 16) = 0;
	*(int*)(buff + 20) = h * real_w;
	*(int*)(buff + 24) = 0;
	*(int*)(buff + 28) = 0;
	*(int*)(buff + 32) = 0;
	*(int*)(buff + 36) = 0;

	file.write(buff, 40);
	memset(buff, 0, 40);

	for (auto i = 0; i < h; ++i)
	{
		file.write((char*)(img.data() + i * w * 3), w * 3);
		file.write(buff, real_w - w * 3);
	}

	delete[] buff;
}

#endif