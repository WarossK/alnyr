#pragma once

#include <tuple>

namespace alnyr
{
	extern void* alnyr_enabler;

	namespace util
	{
		template
			<
			class HashCompositedType,
			typename SFINAE = typename std::enable_if<std::is_invocable_v<decltype(&HashCompositedType::hash), HashCompositedType*>>::type
			>
			struct alnyr_less
		{
			constexpr bool operator()(const HashCompositedType& _Left, const HashCompositedType& _Right) const
			{
				return (_Left.hash() < _Right.hash());
			}
		};

		//============================================================================
		//usage :
		//	int val[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		//	size_t size;
		//	int* ptr;
		//	
		//	std::tie(ptr, size) = get_raw_array_size_and_pointer(val);
		//
		//	ptr -> val;	size -> 10;
		//============================================================================
		template<class Type, size_t N> constexpr std::tuple<Type*, size_t> get_raw_array_size_and_pointer(Type(&arr)[N])
		{
			return std::make_tuple(arr, N);
		}
	}
}