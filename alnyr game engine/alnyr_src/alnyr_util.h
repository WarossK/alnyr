#pragma once

namespace alnyr
{
	namespace util
	{
		template<class HashCompositedType>
		struct alnyr_less
		{
			constexpr bool operator()(const HashCompositedType& _Left, const HashCompositedType& _Right) const
			{
				return (_Left.hash() < _Right.hash());
			}
		};
	}
}