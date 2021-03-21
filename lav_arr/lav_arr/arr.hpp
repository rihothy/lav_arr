#ifndef _LAV_ARR_ARR_ARR_HPP
#define _LAV_ARR_ARR_ARR_HPP

#include <lav_arr/arr/shape.hpp>
#include <lav_arr/arr/range.hpp>

#include <type_traits>
#include <iostream>
#include <vector>

namespace lav
{
	template<typename T>
	class Arr
	{
		template<typename T_Another>
		friend class Arr;

		size_t length = 0;
		T* p_data = nullptr;

	public:

		Shape shape;

		using value_type = T;

	public:

		Arr(void) = default;

		template<typename T_Another>
		Arr(const Arr<T_Another>& another);
		Arr(const Arr& another);

		Arr(Arr&& another) noexcept;

		Arr(const Shape& shape);

		~Arr();


		template<typename T_Another>
		auto& operator=(const Arr<T_Another>& another);
		auto& operator=(const Arr& another);

		auto& operator=(Arr&& another) noexcept;


		template<class... Idxes>
		requires (std::is_integral_v<Idxes>&& ...)
		auto& operator()(Idxes... idxes);
		template<class... Idxes>
		requires (std::is_integral_v<Idxes>&& ...)
		auto operator()(Idxes... idxes) const;

		auto operator()(std::vector<Range> ranges) const;

		auto squeeze(void);

		inline auto data(void) const;

		inline auto dims(void) const;

		inline auto size(void) const;

		auto get_strides(void) const;

		auto reshape(const Shape& shape);


		template<typename T>
		friend auto squeeze(const Arr<T>& arr);

		template<typename T>
		friend auto reshape(const Arr<T>& arr, const Shape& shape);

		template<typename T>
		friend auto& operator<<(std::ostream& out, const Arr<T>& arr);


		template<typename T, class Func>
		friend auto op(const Arr<T>& input, Func func);

		template<typename T1, typename T2, class Func>
		friend auto op(const Arr<T1>& input1, const Arr<T2>& input2, Func func);


		template<typename T, class Func>
		friend auto reduce(const Arr<T>& input, Func func);


		template<typename T>
		friend auto pad(const Arr<T>& arr, std::vector<Shape> sides, T val);

		template<typename T_Output, typename T_Input>
		friend auto build(const Arr<T_Input>& input, Shape shape, const Shape& strides, long long off_set);
	};
}

#include <lav_arr/arr/ops/self_base_ops.hpp>
#include <lav_arr/arr/ops/exte_base_ops.hpp>
#include <lav_arr/arr/ops/arrs_ops.hpp>
#include <lav_arr/arr/ops/build.hpp>
#include <lav_arr/arr/arr_init.hpp>

#endif