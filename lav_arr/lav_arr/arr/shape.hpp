#ifndef _LAV_ARR_ARR_SHAPE_HPP_
#define _LAV_ARR_ARR_SHAPE_HPP_

#include <type_traits>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <vector>
#include <string>

namespace lav
{
	template<typename T>
	class Arr;

	class Shape
	{
		template<typename T>
		friend class Arr;

		std::vector<size_t> sizes;
		std::vector<size_t> strides;

		template<class Iter>
		auto copy(Iter first, Iter last, bool check = true)
		{
			if (check)
			{
				if (auto another_eles = std::reduce(first, last, 1ull, std::multiplies()),
					own_eles = std::reduce(sizes.begin(), sizes.end(), 1ull, std::multiplies());
					another_eles != own_eles)
				{
					throw std::runtime_error(std::string(__FUNCTION__) + "::The total number of elements is different. Shape change is not supported!");
				}
			}

			sizes.assign(first, last);
			init_strides();
		}

		auto init_strides(void)
		{
			strides = std::vector{ 1ull };

			for (auto i = long(sizes.size()) - 1; i; --i)
			{
				strides.push_back(strides.back() * sizes[i]);
			}

			std::reverse(strides.begin(), strides.end());
		}

	public:

		Shape() = default;

		Shape(const Shape& another)
		{
			sizes = another.sizes;
			strides = another.strides;
		}

		Shape(Shape&& another) noexcept
		{
			sizes = std::move(another.sizes);
			strides = std::move(another.strides);
		}

		template<class... Args>
		requires (std::is_integral_v<Args> && ...)
		Shape(Args... args)
		{
			const auto& list = { size_t(args)... };

			copy(list.begin(), list.end(), false);
		}

		template<typename T>
		requires std::is_integral_v<T>
		Shape(const std::vector<T>& list)
		{
			copy(list.begin(), list.end(), false);
		}

		auto dims(void) const
		{
			return long(sizes.size());
		}

		auto operator[](int idx) const
		{
			if (idx < 0)
			{
				idx = sizes.size() + idx;
			}

			return sizes[idx];
		}

		auto& operator=(const Shape& another)
		{
			copy(another.sizes.begin(), another.sizes.end());

			return *this;
		}

		auto operator==(const Shape& another)
		{
			return sizes == another.sizes;
		}

		friend auto& operator<<(std::ostream& out, const Shape& shape)
		{
			for (auto iter = shape.sizes.begin(); iter != shape.sizes.end(); ++iter)
			{
				out << " ("[iter == shape.sizes.begin()] << *iter << ",)"[iter == shape.sizes.end() - 1];
			}

			return out << (shape.sizes.empty() ? "()" : "");
		}
	};
}

#endif