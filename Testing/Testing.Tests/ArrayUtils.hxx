#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <initializer_list>
#include <vector>
#include <algorithm>

namespace testing
{
	template<typename T>
	AssertionResult IsInRange(T value, T low, T high)
	{
		if (value < low)
		{
			return AssertionFailure() << value << " < lower bound " << low;
		}
		else if (value > high)
		{
			return AssertionFailure() << value << " > upper bound " << high;
		}
		else
		{
			return AssertionSuccess() << value << " is in range [" << low << ", " << high << "]";
		}
	}

	template<typename T>
	AssertionResult IsInArray(T value, const std::vector<T>& arr)
	{
		bool result = false;
		for (auto& elem : arr)
		{
			if (elem == value)
			{
				result = true;
				break;
			}
		}

		AssertionResult to_ret =
			result ?
			AssertionSuccess() << value << " is in array: [ " :
			AssertionFailure() << value << " is not in array: [ ";

		for (auto& sub_elem : arr)
		{
			to_ret << sub_elem << " ";
		}
		to_ret << "]";

		return to_ret;
	}

	template <class T>
	bool is_unique(std::vector<T> &x)
	{
		std::sort(x.begin(), x.end()); // O(N log N)
		return std::adjacent_find(x.begin(), x.end()) == x.end();
	}

	template<typename T>
	AssertionResult IsArrayUnique(std::vector<T> arr)
	{
		AssertionResult to_ret =
			is_unique(arr) ?
			AssertionSuccess() << " array is unique: [ " :
			AssertionFailure() << " array is not unique: [ ";

		for (auto& sub_elem : arr)
		{
			to_ret << sub_elem << " ";
		}
		to_ret << "]";

		return to_ret;
	}
}
