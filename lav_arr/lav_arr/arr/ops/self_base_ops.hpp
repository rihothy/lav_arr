#ifndef _LAV_ARR_ARR_OPS_SELF_BASE_OPS_HPP_
#define _LAV_ARR_ARR_OPS_SELF_BASE_OPS_HPP_

#include <lav_arr/arr/arr_init.hpp>
#include <lav_arr/arr.hpp>

#include <type_traits>
#include <stdexcept>
#include <numeric>
#include <vector>
#include <string>

namespace lav
{
	namespace _private
	{
		template<typename T, class... Idxes>
		inline auto _get_off_set_from_idxes(const Shape& shape, const std::vector<T>& strides, Idxes... idxes)
		{
			if (sizeof...(Idxes) != shape.dims())
			{
				throw std::runtime_error(std::string(__FUNCTION__) + "::Does not match the arr dimension!");
			}

			auto&& list = std::vector{ (long long)(idxes)... };

			for (auto i = 0; i < shape.dims(); ++i)
			{
				if (list[i] < 0)
				{
					list[i] = shape[i] + list[i];
				}

				if (list[i] >= shape[i])
				{
					throw std::runtime_error(std::string(__FUNCTION__) + "::Subscript out of range!");
				}
			}

			return std::inner_product(strides.begin(), strides.end(), list.begin(), 0ull);
		}
	}

	template<typename T>
	template<class... Idxes>
	requires (std::is_integral_v<Idxes>&& ...)
	auto& Arr<T>::operator()(Idxes... idxes)
	{
		return p_data[_private::_get_off_set_from_idxes(shape, shape.strides, idxes...)];
	}

	template<typename T>
	template<class... Idxes>
	requires (std::is_integral_v<Idxes>&& ...)
	auto Arr<T>::operator()(Idxes... idxes) const
	{
		return p_data[_private::_get_off_set_from_idxes(shape, shape.strides, idxes...)];
	}

	template<typename T>
	auto Arr<T>::operator()(std::vector<Range> ranges) const
	{
		if (ranges.size() > dims())
		{
			throw std::runtime_error(std::string(__FUNCTION__) + "The number of ranges provided exceeds the dimensions!");
		}

		while (ranges.size() < dims())
		{
			ranges.push_back(Range(Range::all));
		}

		auto new_shape = std::vector<size_t>();

		for (auto i = 0; i < dims(); ++i)
		{
			ranges[i].adjust(shape[i]);
			new_shape.push_back(ranges[i].right() - ranges[i].left());
		}

		auto off_set = 0ull;

		for (auto i = 0; i < dims(); ++i)
		{
			off_set += ranges[i].left() * shape.strides[i];
		}

		return build(*this, new_shape, shape.strides, off_set);
	}

	template<typename T>
	auto Arr<T>::squeeze(void)
	{
		auto new_shape = std::vector<size_t>();

		for (auto i = 0; i < dims(); ++i)
		{
			if (shape[i] != 1)
			{
				new_shape.push_back(shape[i]);
			}
		}

		if (new_shape.empty())
		{
			new_shape.push_back(1);
		}

		reshape(new_shape);
	}

	template<typename T>
	auto Arr<T>::data(void) const
	{
		return p_data;
	}

	template<typename T>
	auto Arr<T>::dims(void) const
	{
		return long(shape.dims());
	}

	template<typename T>
	auto Arr<T>::size(void) const
	{
		return length;
	}

	template<typename T>
	auto Arr<T>::get_strides(void) const
	{
		return shape.strides;
	}

	template<typename T>
	auto Arr<T>::reshape(const Shape& shape)
	{
		this->shape = shape;
	}
}

#endif