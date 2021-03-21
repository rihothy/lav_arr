#ifndef _LAV_ARR_ARR_OPS_PAD_HPP_
#define _LAV_ARR_ARR_OPS_PAD_HPP_

#include <lav_arr/arr.hpp>

#include <stdexcept>
#include <algorithm>
#include <execution>
#include <vector>
#include <string>

namespace lav
{
	//Î´Íê´ýÐø...
	template<typename T>
	auto pad(const Arr<T>& arr, std::vector<Shape> sides, T val)
	{
		if (sides.size() > arr.dims())
		{
			throw std::runtime_error(std::string(__FUNCTION__) + "::");
		}

		while (sides.size() != arr.dims())
		{
			sides.push_back({ 0, 0 });
		}

		auto new_shape = std::vector<size_t>();

		for (auto i = 0; i < arr.dims(); ++i)
		{
			if (sides[i].shape.size() != 2)
			{
				throw std::runtime_error(std::string(__FUNCTION__) + "::");
			}

			new_shape.push_back(sides[i][0] + arr.shape[i] + sides[i][1]);
		}

		auto output = Arr<T>(new_shape);

		std::for_each(std::execution::par, output.p_data, output.p_data + output.length, [&](auto& ele)
			{
				//auto idx = &ele - output.p_data;
			}
		);

		memset(output.p_data, 0, output.length * sizeof(T));

		std::for_each(std::execution::seq, arr.p_data, arr.p_data + arr.length, [&](auto& ele)
			{
				auto idx = &ele - arr.p_data;

				auto p_data = output.p_data;
				auto off_set = 0;
				auto stride = 1;

				for (auto i = arr.dims() - 1; i < arr.dims(); --i)
				{
					auto size = idx % arr.shape[i] + sides[i][0];
					idx /= arr.shape[i];

					off_set += size * stride;
					stride *= output.shape[i];
				}

				p_data[off_set] = ele;
			}
		);

		return std::move(output);
	}
}

#endif