#ifndef _LAV_ARR_ARR_OPS_BUILD_HPP_
#define _LAV_ARR_ARR_OPS_BUILD_HPP_

#include <lav_arr/arr.hpp>

#include <type_traits>
#include <algorithm>
#include <execution>
#include <stdexcept>
#include <string>

namespace lav
{
	template<typename T_Output = void, typename T_Input>
	auto build(const Arr<T_Input>& input, Shape shape, const Shape& strides, long long off_set = 0)
	{
		if constexpr (std::is_void_v<T_Output>)
		{
			return build<T_Input>(input, shape, strides, off_set);
		}
		else if (shape.dims() != 0 && strides.dims() == shape.dims())
		{
			auto output = Arr<T_Output>(shape);

			std::for_each(std::execution::par, output.p_data, output.p_data + output.length, [&](auto& ele)
				{
					auto idx = &ele - output.p_data;
					auto p_data = input.p_data;

					for (auto i = shape.dims() - 1; i >= 0; --i)
					{
						p_data += idx % shape[i] * strides[i];
						idx /= shape[i];
					}

					ele = p_data[off_set];
				}
			);

			return output;
		}
		else
		{
			throw std::runtime_error(std::string(__FUNCTION__) + "::Please provide the correct shape and strides!");
		}
	}
}

#endif