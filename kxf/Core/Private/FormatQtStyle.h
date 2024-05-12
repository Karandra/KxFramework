#pragma once
#include "../Common.h"
#include "../String.h"
#include "../RegEx.h"

namespace kxf
{
	template<class... Args>
	String FormatQtStyle(String format, Args&&... arg)
	{
		String result = std::move(format);
		auto FormatArg = [&result, buffer = String(), index = size_t(1)]<class T>(T&& arg) mutable
		{
			if (RegEx regEx(Format("%({})", index)); regEx.Matches(result))
			{
				buffer.clear();
				buffer.Format("{}", std::forward<T>(arg));

				regEx.ReplaceAll(result, buffer);
			}
			++index;
		};
		std::initializer_list<int>{(FormatArg(std::forward<Args>(arg)), 0) ...};

		return result;
	}
}
