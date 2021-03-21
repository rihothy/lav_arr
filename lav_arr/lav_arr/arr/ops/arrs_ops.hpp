#ifndef _LAV_ARR_ARR_OPS_ARRS_OPS_HPP_
#define _LAV_ARR_ARR_OPS_ARRS_OPS_HPP_

#include <lav_arr/arr.hpp>

#include <type_traits>
#include <functional>
#include <stdexcept>
#include <execution>
#include <algorithm>
#include <thread>
#include <string>

namespace lav
{
	namespace _private
	{
		inline auto _can_be_broadcast(const Shape& shape1, const Shape& shape2)
		{
			auto i = 1, j = 1;

			for (; i <= shape1.dims() && j <= shape2.dims();)
			{
				if (shape1[-i] == 1)
					++i;
				else if (shape2[-j] == 1)
					++j;
				else if (shape1[-i] == shape2[-j])
					++i, ++j;
				else
					return false;
			}

			return true;
		}
	}

	template<typename T, class Func>
	auto op(const Arr<T>& input, Func func)
	{
		using ret_type = std::remove_reference_t<decltype(func(*input.p_data))>;

		if constexpr (std::is_void_v<ret_type>)
		{
			std::for_each(std::execution::par, input.p_data, input.p_data + input.length, func);
		}
		else
		{
			auto output = Arr<ret_type>(input.shape);

			std::transform(std::execution::par, input.p_data, input.p_data + input.length, output.p_data, func);

			return std::move(output);
		}
	}

	template<typename T1, typename T2, class Func>
	auto op(const Arr<T1>& input1, const Arr<T2>& input2, Func func)
	{
		auto broadcast_supported = _private::_can_be_broadcast(input1.shape, input2.shape);
		using ret_type = std::remove_reference_t<decltype(func(*input1.p_data, *input2.p_data))>;

		if (broadcast_supported)
		{
			if constexpr (std::is_void_v<ret_type>)
			{
				if (input1.length >= input2.length)
				{
					std::for_each(std::execution::par, input1.p_data, input1.p_data + input1.length, [&](auto& ele)
						{
							auto idx = &ele - input1.p_data;

							func(ele, input2.p_data[idx % input2.length]);
						}
					);
				}
				else
				{
					std::for_each(std::execution::par, input2.p_data, input2.p_data + input2.length, [&](auto& ele)
						{
							auto idx = &ele - input2.p_data;

							func(input1.p_data[idx % input1.length], ele);
						}
					);
				}
			}
			else
			{
				auto output = Arr<ret_type>(input1.length >= input2.length ? input1.shape : input2.shape);

				if (input1.length >= input2.length)
				{
					std::for_each(std::execution::par, input1.p_data, input1.p_data + input1.length, [&](auto& ele)
						{
							auto idx = &ele - input1.p_data;

							output.p_data[idx] = func(ele, input2.p_data[idx % input2.length]);
						}
					);
				}
				else
				{
					std::for_each(std::execution::par, input2.p_data, input2.p_data + input2.length, [&](auto& ele)
						{
							auto idx = &ele - input2.p_data;

							output.p_data[idx] = func(input1.p_data[idx % input1.length], ele);
						}
					);
				}

				return std::move(output);
			}
		}
		else
		{
			throw std::runtime_error(std::string(__FUNCTION__) + "::The shapes of the two arrs are incompatible!");
		}
	}

	template<typename T, class Func>
	auto reduce(const Arr<T>& input, Func func)
	{
		using ret_type = std::remove_reference_t<decltype(func(*input.p_data, *input.p_data))>;
		auto run = std::function<void(decltype(input.p_data), decltype(input.p_data), int, std::add_lvalue_reference_t<ret_type>)>();

		run = [&](auto first, auto last, int depth, auto& ret)
		{
			if (depth < 10)
			{
				if (last - first == 1)
				{
					ret = *first;
				}
				else if (last - first == 2)
				{
					ret = func(*first, *(first + 1));
				}
				else
				{
					ret_type a, b;
					auto mid = first + (last - first) / 2;

					auto t1 = std::thread(run, first, mid, depth + 1, std::ref(a));
					auto t2 = std::thread(run, mid, last, depth + 1, std::ref(b));

					t1.join();
					t2.join();

					ret = func(a, b);
				}
			}
			else
			{
				ret = *first;

				for (auto iter = first + 1; iter != last; ++iter)
				{
					ret = func(ret, *iter);
				}
			}
		};

		ret_type ret;

		run(input.p_data, input.p_data + input.length, 0, ret);

		return ret;
	}
}

#endif