#ifndef _LAV_ARR_ARR_ARR_INIT_HPP
#define _LAV_ARR_ARR_ARR_INIT_HPP

#include <lav_arr/arr/ops/arrs_ops.hpp>
#include <lav_arr/arr.hpp>

#include <type_traits>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <fstream>
#include <cstring>
#include <limits>
#include <string>

namespace lav
{
	namespace _private
	{
		template<typename T1, typename T2>
		inline auto _clip_T2_2_T1 = [](auto& a, auto& b)
		{
			if constexpr (std::is_arithmetic<T1>() && std::is_arithmetic<T2>())
			{
				if (b > std::numeric_limits<T1>::max())
				{
					a = std::numeric_limits<T1>::max();
				}
				else if (b < std::numeric_limits<T1>::min())
				{
					a = std::numeric_limits<T1>::min();
				}
			}
			else
			{
				a = b;
			}
		};
	}

	template<typename T>
	template<typename T_Another>
	Arr<T>::Arr(const Arr<T_Another>& another)
	{
		*this = another;
	}

	template<typename T>
	Arr<T>::Arr(const Arr<T>& another)
	{
		*this = another;
	}

	template<typename T>
	Arr<T>::Arr(Arr<T>&& another) noexcept
	{
		*this = std::move(another);
	}

	template<typename T>
	Arr<T>::Arr(const Shape& shape) :shape(shape)
	{
		length = std::reduce(shape.sizes.begin(), shape.sizes.end(), 1ull, std::multiplies());
		p_data = new T[length];
	}

	template<typename T>
	Arr<T>::~Arr()
	{
		delete[] p_data;
	}

	template<typename T>
	template<typename T_Another>
	auto& Arr<T>::operator=(const Arr<T_Another>& another)
	{
		if (another.length != length)
		{
			delete[] p_data;
			p_data = new T[another.length];
		}

		length = another.length;
		shape.sizes = another.shape.sizes;
		shape.strides = another.shape.strides;

		op_inplace(*this, another, _private::_clip_T2_2_T1<T, T_Another>);

		return *this;
	}

	template<typename T>
	auto& Arr<T>::operator=(const Arr<T>& another)
	{
		if (&another != this)
		{
			if (another.length != length)
			{
				delete[] p_data;
				p_data = new T[another.length];
			}

			length = another.length;
			shape.sizes = another.shape.sizes;
			shape.strides = another.shape.strides;

			memcpy(p_data, another.p_data, length * sizeof(T));
		}

		return *this;
	}

	template<typename T>
	auto& Arr<T>::operator=(Arr<T>&& another) noexcept
	{
		if (&another != this)
		{
			delete[] p_data;

			p_data = another.p_data;
			length = another.length;
			shape.sizes = std::move(another.shape.sizes);
			shape.strides = std::move(another.shape.strides);

			another.length = 0;
			another.p_data = nullptr;
		}

		return *this;
	}
}

#endif