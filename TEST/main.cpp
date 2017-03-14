#include <map>
#include <vector>
#include <string>
#include <algorithm>
typedef unsigned long UInt32;
///typedef std::map<UInt32, float>				SettingListFloat;

struct SettingListInt : public std::map < UInt32, UInt32 >
{
	SettingListInt::mapped_type GetMapValue(const SettingListInt::key_type& index)  ///read-only 
	{
		SettingListInt::mapped_type result = 1;
		auto it = std::find_if(begin(), end(), [index, &result](const SettingListInt::value_type& param)->bool{
			if (param.first == index)
			{
				result = param.second;
				return true;
			}
			else if ((index > param.first) && (param.first > result))
				result = param.second;
			return false;
		});
		return result;
	}
};

struct SettingListFloat : public std::map < UInt32, float >
{
	SettingListFloat::mapped_type GetMapValue(const SettingListFloat::key_type& index)  ///read-only 
	{
		SettingListFloat::mapped_type result = 1.0f;
		auto it = std::find_if(begin(), end(), [index, &result](const SettingListFloat::value_type& param)->bool{
			if (param.first == index)
			{
				result = param.second;
				return true;
			}
			else if ((index > param.first) && (param.first > result))
				result = param.second;
			return false;
		});
		return result;
	}
};

void main()
{

	SettingListInt  test;

	test.insert({ 1, 3 });
	test.insert({ 4, 2 });
	test.insert({ 10, 5 });
	test.insert({ 20, 7 });
	int m = test.GetMapValue(7);
	int n = test.GetMapValue(14);
	printf("m=%d, n=%d\n", m, n);

	system("pause");
}