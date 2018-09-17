#pragma once
#include "KxFramework/KxFramework.h"

class KxUtility
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

		static bool DrawLabel(const wxWindow* window, wxDC& dc, const wxString& label, const wxBitmap& icon, wxRect rect, int alignment = wxALIGN_LEFT|wxALIGN_TOP, int accelIndex = -1, wxRect* boundingRect = NULL);
		static bool DrawGripper(const wxWindow* window, wxDC& dc, const wxRect& rect);
		
		static KxColor GetThemeColor(const wxWindow* window, const wxString& className, int iPartId, int iStateId, int iPropId, const wxColour& defaultColor = wxNullColour);
		static KxColor GetThemeColor_Caption(const wxWindow* window);

		static HMODULE GetAppHandle();
		static const wxString LoadResource(const wxString& name, const wxString& typeName = L"STRING");
		static const wxString LoadResource(int id, const wxString& typeName = L"STRING");
		static void ToggleWindowStyle(HWND hWnd, int index, LONG style, bool enable);
		static wxString GetStandardLocalizedString(int id, bool* isSuccess = NULL);
		template<class PointerType, class ValueType> static void SetIfNotNull(PointerType* p, ValueType v)
		{
			if (p)
			{
				*p = static_cast<PointerType>(v);
			}
		}
		template<class F, class V> static F ModFlag(F f, V v, bool set)
		{
			if (set)
			{
				f = static_cast<F>(f|static_cast<F>(v));
			}
			else
			{
				f = static_cast<F>(f & ~static_cast<F>(v));
			}
			return f;
		}
		template<class F, class V> static void ModFlagRef(F& f, V v, bool set)
		{
			f = ModFlag(f, v, set);
		}
		template<class F, class V> static bool HasFlag(F f, V v)
		{
			return f & v;
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

		static bool StringToBool(const wxString& value, bool* isUnknown = NULL);
};

class KxAlignedAllocator
{
	private:
		void* m_Source = NULL;
		void* m_Aligned = NULL;
		size_t m_Size = 0;

	public:
		KxAlignedAllocator(size_t size)
			:m_Size(size)
		{
			m_Source = std::malloc(size);
		}
		~KxAlignedAllocator()
		{
			std::free(m_Source);
		}

		void ReAlloc(size_t size)
		{
			m_Aligned = NULL;
			m_Size = size;

			void* temp = std::realloc(m_Source, size);
			if (temp)
			{
				m_Source = temp;
			}
		}
		template <typename T> T* Align(size_t a = alignof(T))
		{
			m_Aligned = NULL;
			if (std::align(a, sizeof(T), m_Aligned, m_Size))
			{
				m_Aligned = reinterpret_cast<T*>(m_Source);
				return m_Aligned;
			}
			return NULL;
		}

		size_t GetSize() const
		{
			return m_Size;
		}
		const void* GetSoure() const
		{
			return m_Source;
		}
		void* GetAligned()
		{
			return m_Aligned;
		}
		const void* GetAligned() const
		{
			return m_Aligned;
		}
};
