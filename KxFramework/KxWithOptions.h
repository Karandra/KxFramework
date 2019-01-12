/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"

template<class T, T t_DefaultOptions = (T)0>
class KxWithOptions
{
	public:
		using TEnum = typename T;
		using TInt = typename std::underlying_type_t<TEnum>;

	private:
		TEnum m_Value = t_DefaultOptions;

	protected:
		static bool DoIsOptionEnabled(TEnum value, TEnum option)
		{
			return static_cast<TInt>(value) & static_cast<TInt>(option);
		}
		static TEnum DoSetOptionEnabled(TEnum value, TEnum option, bool enable = true)
		{
			if (enable)
			{
				return static_cast<TEnum>(static_cast<TInt>(value) | static_cast<TInt>(option));
			}
			else
			{
				return static_cast<TEnum>(static_cast<TInt>(value) & ~static_cast<TInt>(option));
			}
		}

	public:
		TEnum GetOptionsValue() const
		{
			return m_Value;
		}
		void SetOptionsValue(TEnum options)
		{
			m_Value = options;
		}
		void SetOptionsValue(TInt options)
		{
			m_Value = static_cast<TEnum>(options);
		}

		bool IsOptionEnabled(TEnum option) const
		{
			return DoIsOptionEnabled(m_Value, option);
		}
		void SetOptionEnabled(TEnum option, bool enable = true)
		{
			m_Value = DoSetOptionEnabled(m_Value, option, enable);
		}
};
