#pragma once

#include "core_types.h"
#include "containers.h"
#include "templates/const_ref.h"
#include "templates/is_trivially_copyable.h"
#include "templates/functional.h"

namespace Container
{
	/**
	 * Sorting algorithms
	 */
	enum SortingAlg
	{
		/**
		 * This algorithm has an average complexity of O(n^2)
		 * but doesn't require extra space and only requires
		 * forward iterators
		 */
		MINSORT = 0,

		/**
		 * Insertion sort algorithm is more efficient than
		 * minsort but it has the same time complexity O(n^2).
		 * It also requires a bidirectional iterator
		 */
		INSERTION,

		/**
		 * Divide-and-conquer sorting algorithm with time
		 * complexity O(n log(n)) and linear space complexity
		 */
		MERGESORT,

		/**
		 * Versatile sorting algorithm with average time
		 * complexity O(n log(n)) and constant space complexity.
		 */
		QUICKSORT
	};

	/**
	 * @class SortingClass containers/sorting.h
	 * 
	 * Unfortunately C++ doesn't support function
	 * templates partial specialization, thus we
	 * have to 'hack' our way around with a wrapper
	 * class
	 */
	template<SortingAlg = QUICKSORT>
	struct SortingClass
	{
		/**
		 * Sort in-place using provided compare funcion (accepts lambdas)
		 * 
		 * @param [in] begin,end range to sort
		 * @param [in] cmpfun compare function
		 */
		template<typename CompareT, typename It>
		static void sort(It begin, It end, CompareT && cmpfun);

		/**
		 * Sort in dest using provided compare function
		 * 
		 * @param [in] begin,end source range
		 * @param [out] dest destination iterator
		 * @param [in] cmpfun compare function
		 */
		template<typename CompareT, typename A, typename B>
		FORCE_INLINE static void sort(A begin, A end, B dest, CompareT && cmpfun)
		{
			// First deep copy container, O(n)
			A i; B j;
			for (i = begin, j = dest; i != end; ++i, ++j)
				moveOrCopy(*j, *i);
			
			// Then sort destination container
			sort<CompareT, B>(dest, j, (CompareT&&)cmpfun);
		}
	};

	/**
	 * Perform sorting in place on iterator range
	 * 
	 * @param [in] begin,end begin and end iterators
	 * @param [in] cmpfun provided compare function
	 * @{
	 */
	template<SortingAlg alg = QUICKSORT, typename CompareT, typename It>
	FORCE_INLINE void sort(It begin, It end, CompareT && cmpfun)
	{
		SortingClass<alg>::template sort<CompareT, It>(begin, end, (CompareT&&)cmpfun);
	}
	template<SortingAlg alg = QUICKSORT, typename It>
	FORCE_INLINE void sort(It begin, It end)
	{
		sort(begin, end, Compare<decltype(*begin)>());
	}
	/// @}

	/**
	 * Perform sorting and save result in destination iterator
	 * 
	 * @param [in] begin,end begin and end source iterators
	 * @param [out] dest destination iterator
	 * @param [in] cmpfun provided compare function
	 * @{
	 */
	template<SortingAlg alg = QUICKSORT, typename CompareT, typename A, typename B>
	FORCE_INLINE void sort(A begin, A end, B dest, CompareT && cmpfun)
	{
		SortingClass<alg>::template sort<CompareT, A, B>(begin, end, dest, (CompareT&&)cmpfun);
	}
	// Not usable unless we use an iterator base class or sfinae out the conflicting declaration
	/* template<SortingAlg alg = QUICKSORT, typename CompareT, typename A, typename B>
	FORCE_INLINE void sort(A begin, A end, B dest)
	{
		sort(begin, end, dest, Compare<decltype(*begin)>());
	} */
	/// @}
} // Container

//////////////////////////////////////////////////
// Minsort
//////////////////////////////////////////////////

template<>
template<typename CompareT, typename It>
void Container::SortingClass<Container::MINSORT>::sort(It begin, It end, CompareT && cmpfun)
{
	It i, j, min;
	for (i = begin; i != end; ++i)
	{
		// Find sub-minimum
		min = j = i;
		for (++j; j != end; ++j)
			if (cmpfun(*j, *min) < 0) min = j;
		
		// Swap with i-th
		swap(*i, *min);
	}
}

//////////////////////////////////////////////////
// Insertion sort
//////////////////////////////////////////////////

template<>
template<typename CompareT, typename It>
void Container::SortingClass<Container::INSERTION>::sort(It begin, It end, CompareT && cmpfun)
{
	It i, j; i = j = begin;
	for (++i; i != end; ++i, ++j)
		// Insert i-th element, going back
		for (It k = j, u = i; u != begin && cmpfun(*u, *k) < 0; --k, --u) swap(*u, *k);
}

//////////////////////////////////////////////////
// Quicksort
//////////////////////////////////////////////////

template<>
template<typename CompareT, typename It>
void Container::SortingClass<Container::QUICKSORT>::sort(It begin, It end, CompareT && cmpfun)
{
	// Partition array around pivot value
	It first = begin; if (first == end | ++begin == end) return;

	// Partition code was limiting me
	It pivot = first;
	for (It i = begin; i != end; ++i)
		if (cmpfun(*i, *first) < 0) swap(*i, *(pivot = begin)), ++begin;
	
	// Swap pivot
	swap(*first, *pivot);

	// Divide and conquer
	sort<CompareT, It>(first, pivot, (CompareT&&)cmpfun);
	sort<CompareT, It>(begin, end, (CompareT&&)cmpfun);
}