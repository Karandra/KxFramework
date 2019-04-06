/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <array>

class KX_API KxUtility
{
	private:
		static const wxString LoadResourceAux(HRSRC resourceHandle, HMODULE moduleHandle);
		template<class TOUT, class TIN> static TOUT GetIntPart(TIN vInt, bool highPart)
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
			v.Int = vInt;

			return highPart ? v.High : v.Low;
		}

	public:
		static bool ClearDC(const wxWindow* window, wxDC& dc)
		{
			return DrawParentBackground(window, dc, wxRect(wxPoint(0, 0), dc.GetSize()));
		}
		static bool DrawParentBackground(const wxWindow* window, wxDC& dc, const wxRect& rect);
		static bool DrawParentBackground(const wxWindow* window, wxDC& dc, const wxPoint& tPoint1, const wxPoint& tPoint2)
		{
			wxRect rect(tPoint1, tPoint2);
			return DrawParentBackground(window, dc, rect);
		}
		static bool DrawThemeBackground(const wxWindow* window, const wxString& className, wxDC& dc, int iPartId, int iStateId, const wxRect& rect);

		static bool DrawLabel(const wxWindow* window, wxDC& dc, const wxString& label, const wxBitmap& icon, wxRect rect, int alignment = wxALIGN_LEFT|wxALIGN_TOP, int accelIndex = -1, wxRect* boundingRect = nullptr);
		static bool DrawGripper(const wxWindow* window, wxDC& dc, const wxRect& rect);
		
		static KxColor GetThemeColor(const wxWindow* window, const wxString& className, int iPartId, int iStateId, int iPropId, const wxColour& defaultColor = wxNullColour);
		static KxColor GetThemeColor_Caption(const wxWindow* window);

		static HMODULE GetAppHandle();
		static const wxString LoadResource(const wxString& name, const wxString& typeName = L"STRING");
		static const wxString LoadResource(int id, const wxString& typeName = L"STRING");
		static void ToggleWindowStyle(HWND hWnd, int index, LONG style, bool enable);
		static wxString GetStandardLocalizedString(int id, bool* isSuccess = nullptr);
		template<class TPointer, class TValue> static void SetIfNotNull(TPointer* ptr, TValue value)
		{
			if (ptr)
			{
				*ptr = static_cast<TPointer>(value);
			}
		}
		template<class TFlag, class TFlagMod> static TFlag ModFlag(TFlag flag, TFlagMod flagMod, bool set)
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
		template<class TFlag, class TFlagMod> static void ModFlagRef(TFlag& flag, TFlagMod flagMod, bool set)
		{
			flag = ModFlag(flag, flagMod, set);
		}
		
		template<class TFlagLeft, class TFlagRight> static bool HasFlag(TFlagLeft left, TFlagRight right)
		{
			static_assert(std::is_enum_v<TFlagLeft>, "left value must be an enum type");
			static_assert(std::is_enum_v<TFlagRight>, "right value must be an enum type");

			using TIntLeft = std::underlying_type_t<TFlagLeft>;
			using TIntRight = std::underlying_type_t<TFlagRight>;

			return static_cast<TIntLeft>(left) & static_cast<TIntRight>(right);
		}
		template<class TFlag> static bool HasFlag(TFlag left, TFlag right)
		{
			if constexpr(std::is_enum_v<TFlag>)
			{
				using TInt = std::underlying_type_t<TFlag>;
				return static_cast<TInt>(left) & static_cast<TInt>(right);
			}
			else
			{
				return left & right;
			}
		}

		inline static void CopyRECTToRect(const RECT& r, wxRect& rect)
		{
			rect.y = r.top;
			rect.x = r.left;
			rect.width = r.right - r.left;
			rect.height = r.bottom - r.top;
		}
		inline static wxRect CopyRECTToRect(const RECT& rc)
		{
			return wxRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		}
		inline static void CopyRectToRECT(const wxRect& rect, RECT& rc)
		{
			rc.top = rect.y;
			rc.left = rect.x;
			rc.right = rect.x + rect.width;
			rc.bottom = rect.y + rect.height;
		}
		inline static RECT CopyRectToRECT(const wxRect& rect)
		{
			RECT tWinRect;
			CopyRectToRECT(rect, tWinRect);
			return tWinRect;
		}

		template<class TOUT, class TIN1, class TIN2> static TOUT MakeInt(TIN1 vHigh, TIN2 vLow)
		{
			static_assert(std::_Is_integral<TIN1>::value && std::_Is_integral<TIN2>::value && std::_Is_integral<TOUT>::value, "only integral types allowed");
			static_assert(sizeof(TIN1) == sizeof(TIN2), "sizeof(TIN1) not equal to sizeof(TIN2)");
			static_assert(sizeof(TOUT) == sizeof(TIN1) + sizeof(TIN2), "sizeof(TIN1) + sizeof(TIN2) not equal to sizeof(TOUT)");
			using TIN = TIN1;
			using TUIN = typename std::make_unsigned<TIN>::type;

			const constexpr TUIN mask = static_cast<TUIN>(-1);
			const constexpr TUIN inputBits = 8 * sizeof(vHigh);
			return (TOUT(vHigh & mask) << inputBits) | TOUT(vLow & mask);
		}
		template<class TOUT, class TIN> static TOUT GetIntHighPart(TIN vInt)
		{
			return GetIntPart<TOUT>(vInt, true);
		}
		template<class TOUT, class TIN> static TOUT GetIntLowPart(TIN vInt)
		{
			return GetIntPart<TOUT>(vInt, false);
		}

		template<class OldType, class NewType, typename ConvFunc> static std::vector<NewType> RepackVector(const std::vector<OldType>& oldVector, ConvFunc ConvFunc)
		{
			std::vector<NewType> newVector;
			newVector.reserve(oldVector.size());
			for (size_t i = 0; i < oldVector.size(); i++)
			{
				newVector.emplace_back(ConvFunc(oldVector[i]));
			}
			return newVector;
		}

		static bool StringToBool(const wxString& value, bool* isUnknown = nullptr);

		template<class... Args> constexpr static size_t SizeOfParameterPack()
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
};
