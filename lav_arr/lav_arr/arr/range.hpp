#ifndef _LAV_ARR_ARR_RANGE_HPP_
#define _LAV_ARR_ARR_RANGE_HPP_

#include <stdexcept>
#include <string>

namespace lav
{
	class Range
	{
		long long l = all;
		long long r = all;

	public:

		static const long long all = LLONG_MAX;
		static const long long none = LLONG_MAX - 1;

		Range(void) = default;

		Range(long long l, long long r) :l(l), r(r) {}

		Range(long long l) :l(l), r(l + 1)
		{
			if (l == all || l == none)
			{
				this->l = this->r = all;
			}
		}

		auto left(void)
		{
			return l;
		}

		auto right(void)
		{
			return r;
		}

		void adjust(size_t size)
		{
			if (l < 0)
			{
				l = size + l;
			}

			if (r < 0)
			{
				r = size + r;
			}

			if (l == all && r == all)
			{
				l = 0;
				r = size;
			}

			if (l == none)
			{
				l = 0;
			}

			if (r == none)
			{
				r = size;
			}

			if (l == r || r > size)
			{
				throw std::runtime_error(std::string(__FUNCTION__) + "::The specified range is illegal!");
			}
		}
	};
}

#endif