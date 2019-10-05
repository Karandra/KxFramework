/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <array>

namespace KxUtility
{
	bool ClearDC(const wxWindow* window, wxDC& dc);
	bool DrawParentBackground(const wxWindow* window, wxDC& dc, const wxRect& rect);
	bool DrawParentBackground(const wxWindow* window, wxDC& dc, const wxPoint& point1, const wxPoint& point2);
	bool DrawThemeBackground(const wxWindow* window, const wxString& className, wxDC& dc, int iPartId, int iStateId, const wxRect& rect);

	bool DrawLabel(const wxWindow* window, wxDC& dc, const wxString& label, const wxBitmap& icon, wxRect rect, int alignment = wxALIGN_LEFT|wxALIGN_TOP, int accelIndex = -1, wxRect* boundingRect = nullptr);
	bool DrawGripper(const wxWindow* window, wxDC& dc, const wxRect& rect);

	KxColor GetThemeColor(const wxWindow* window, const wxString& className, int iPartId, int iStateId, int iPropId, const wxColour& defaultColor = wxNullColour);
	KxColor GetThemeColor_Caption(const wxWindow* window);
}

namespace KxUtility
{
	HMODULE GetAppHandle();
	const wxString LoadResource(const wxString& name, const wxString& typeName = L"STRING");
	const wxString LoadResource(int id, const wxString& typeName = L"STRING");
	void ToggleWindowStyle(HWND hWnd, int index, LONG style, bool enable);

	wxString GetStandardLocalizedString(int id, bool* isSuccess = nullptr);
	bool StringToBool(const wxString& value, bool* isUnknown = nullptr);

	inline void CopyRECTToRect(const RECT& r, wxRect& rect)
	{
		rect.y = r.top;
		rect.x = r.left;
		rect.width = r.right - r.left;
		rect.height = r.bottom - r.top;
	}
	inline wxRect CopyRECTToRect(const RECT& rc)
	{
		return wxRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	}
	inline void CopyRectToRECT(const wxRect& rect, RECT& rc)
	{
		rc.top = rect.y;
		rc.left = rect.x;
		rc.right = rect.x + rect.width;
		rc.bottom = rect.y + rect.height;
	}
	inline RECT CopyRectToRECT(const wxRect& rect)
	{
		RECT winRect;
		CopyRectToRECT(rect, winRect);
		return winRect;
	}

	template<class TOut, class TSource, class TFunc>
	void ConvertVector(const std::vector<TSource>& oldVector, std::vector<TOut>& newVector, TFunc&& func)
	{
		newVector.reserve(oldVector.size());
		for (const TSource& item: oldVector)
		{
			newVector.emplace_back(func(item));
		}
	}
		
	template<class TOut, class TSource, class TFunc>
	std::vector<TOut> ConvertVector(const std::vector<TSource>& oldVector, TFunc&& func)
	{
		std::vector<TOut> newVector;
		ConvertVector(oldVector, newVector, std::forward<TFunc>(func));

		return newVector;
	}

	template<class... Args>
	constexpr size_t SizeOfParameterPackValues()
	{
		const constexpr size_t count = sizeof...(Args);
		const constexpr std::array<size_t, count> sizes = {sizeof(Args)...};

		size_t sum = 0;
		for (const size_t& size: sizes)
		{
			sum += size;
		}
		return sum;
	}

	template<class TPointer, class TValue>
	constexpr void SetIfNotNull(TPointer* ptr, TValue value)
	{
		if (ptr)
		{
			*ptr = static_cast<TPointer>(value);
		}
	}
};

namespace KxUtility
{
	namespace Internal
	{
		template<class TOUT, class TIN>
		TOUT GetIntPart(TIN value, bool highPart)
		{
			static_assert(std::is_integral<TIN>::value && std::is_integral<TOUT>::value, "only integral types allowed");
			static_assert(sizeof(TIN) == 2 * sizeof(TOUT), "sizeof(TIN) not equal to 2 * sizeof(TOUT)");

			union IntU
			{
				struct
				{
					TOUT High;
					TOUT Low;
				};
				TIN Int;
			};
			IntU v;
			v.Int = value;

			return highPart ? v.High : v.Low;
		}
	}

	template<class TOUT, class TIN1, class TIN2>
	TOUT MakeInt(TIN1 high, TIN2 low)
	{
		static_assert(std::is_integral_v<TIN1> && std::is_integral_v<TIN2> && std::is_integral_v<TOUT>, "only integral types allowed");
		static_assert(sizeof(TIN1) == sizeof(TIN2), "sizeof(TIN1) not equal to sizeof(TIN2)");
		static_assert(sizeof(TOUT) == sizeof(TIN1) + sizeof(TIN2), "sizeof(TIN1) + sizeof(TIN2) not equal to sizeof(TOUT)");

		using TIN = TIN1;
		using TUIN = typename std::make_unsigned<TIN>::type;

		constexpr TUIN mask = static_cast<TUIN>(-1);
		constexpr TUIN inputBits = 8 * sizeof(high);
		return (TOUT(high & mask) << inputBits) | TOUT(low & mask);
	}

	template<class TOUT, class TIN>
	TOUT GetIntHighPart(TIN value)
	{
		return Internal::GetIntPart<TOUT>(value, true);
	}

	template<class TOUT, class TIN>
	TOUT GetIntLowPart(TIN value)
	{
		return Internal::GetIntPart<TOUT>(value, false);
	}
}

namespace KxUtility
{
	template<class TFlag, class TFlagMod>
	constexpr TFlag ModFlag(TFlag flag, TFlagMod flagMod, bool set)
	{
		if (set)
		{
			flag = static_cast<TFlag>(flag|static_cast<TFlag>(flagMod));
		}
		else
		{
			flag = static_cast<TFlag>(flag & ~static_cast<TFlag>(flagMod));
		}
		return flag;
	}
	
	template<class TFlag, class TFlagMod>
	constexpr void ModFlagRef(TFlag& flag, TFlagMod flagMod, bool set)
	{
		flag = ModFlag(flag, flagMod, set);
	}

	template<class TFlagLeft, class TFlagRight>
	constexpr bool HasFlag(TFlagLeft left, TFlagRight right)
	{
		static_assert(std::is_enum_v<TFlagLeft>, "left value must be an enum type");
		static_assert(std::is_enum_v<TFlagRight>, "right value must be an enum type");

		using TIntLeft = std::underlying_type_t<TFlagLeft>;
		using TIntRight = std::underlying_type_t<TFlagRight>;

		return static_cast<TIntLeft>(left)& static_cast<TIntRight>(right);
	}

	template<class TFlag>
	constexpr bool HasFlag(TFlag left, TFlag right)
	{
		if constexpr (std::is_enum_v<TFlag>)
		{
			using TInt = std::underlying_type_t<TFlag>;
			return static_cast<TInt>(left)& static_cast<TInt>(right);
		}
		else
		{
			return left & right;
		}
	}
}
