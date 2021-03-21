#ifndef _LAV_ARR_ARR_OPS_EXTE_BASE_OPS_HPP_
#define _LAV_ARR_ARR_OPS_EXTE_BASE_OPS_HPP_

#include <lav_arr/arr/arr_init.hpp>
#include <lav_arr/arr.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace lav
{
	template<typename T>
	auto squeeze(const Arr<T>& arr)
	{
		auto new_shape = std::vector<size_t>();

		for (auto i = 0; i < arr.dims(); ++i)
		{
			if (arr.shape[i] != 1)
			{
				new_shape.push_back(arr.shape[i]);
			}
		}

		if (new_shape.empty())
		{
			new_shape.push_back(1);
		}

		return reshape(arr, new_shape);
	}

	template<typename T>
	auto reshape(const Arr<T>& arr, const Shape& shape)
	{
		auto output = arr;

		output.reshape(shape);

		return std::move(output);
	}

	template<typename T>
	auto& operator<<(std::ostream& out, const Arr<T>& arr)
	{
		auto p_data = arr.data();

		auto run = [&](auto self, auto dim, auto blank)->void
		{
			if (dim < arr.dims() - 1)
			{
				for (auto i = 0; i < arr.shape[dim]; ++i)
				{
					out << std::string(bool(i) * blank, ' ') << '[';
					self(self, dim + 1, blank + 1);
					out << (i == arr.shape[dim] - 1 ? "]" : "]," + std::string(arr.dims() - dim - 1, '\n'));
				}
			}
			else if (dim < arr.dims())
			{
				for (auto i = 0; i < arr.shape[dim]; ++i)
				{
					out << *p_data++ << (i == arr.shape[dim] - 1 ? "" : ",");
				}
			}
		};

		out << '[';
		run(run, 0, 1);
		out << ']';

		return out;
	}
}

#endif