#include <map>
#include <vector>
#include <string>
#include <algorithm>
typedef unsigned long UInt32;
///typedef std::map<UInt32, float>				SettingListFloat;

template<typename T>
struct SettingList : public std::map < UInt32, T>
{
	typename SettingList::mapped_type GetValue(const typename SettingList::key_type& index)
	{
		typename SettingList::mapped_type result = NULL;
		for (auto it = rbegin(); it != rend(); ++it)
		{
			if (it->first <= index)
			{
				result = it->second;
				break;
			}
		}
		return result;
	}

	float GetDecimal(const typename SettingList::key_type& index)
	{
		float decimal = 0.00f;
		for (auto iterator = rbegin(); iterator != rend(); ++iterator)
		{
			if (iterator->first < index)
			{
				if (iterator != --rend())
				{
					decimal += (index - iterator->first) * (iterator->second);
					auto it = iterator;
					++it;
					for (; it != rend(); ++it)
					{
						auto nextIterator = it;
						--nextIterator;
						if (it != --rend())
							decimal += (nextIterator->first - it->first) * (it->second);
						else
							decimal += (nextIterator->first - it->first - 1) * (it->second);
					}
				}
				else
					decimal += (index - iterator->first - 1) * (iterator->second);
				break;
			}
		}
		return decimal - static_cast<UInt32>(decimal);
	}

};

int sum(const int& a, const int& b)
{
	printf("function:%s, a = %d, c =%d", __FUNCTION__, a, b);
	return a + b;
}


void main()
{

	//SettingList<float>  test;

	//test.insert({ 1, 1.13 });
	//test.insert({ 4, 2.28 });
	//test.insert({ 10, 5.46 });
	//test.insert({ 20, 7.35 });
	//for (unsigned long i = 0; i < 1000; ++i)
	//{
	//	printf("index: %d, n: %.2f\n", i, test.GetDecimal(i));
	//}

	int a = 10, b = 11;

	typedef int(*_SUMREF)(int&, int&);
	_SUMREF sum1 = (_SUMREF)sum;

	typedef int(*_SUMPOT)(int*, int*);
	_SUMPOT sum2 = (_SUMPOT)sum;

	sum1(a, b);
	sum2(&a, &b);


	system("pause");
}