#ifndef _LAV_ARR_HPP_
#define _LAV_ARR_HPP_

#include "lav_arr/arr.hpp"
#include "lav_arr/cv.hpp"

#include <iostream>

static auto print = [](const auto&... args)
{
	auto first = true;
	((std::cout << (first ? "" : " ") << args, first = false), ...);
	std::cout << std::endl;
};

#endif