#pragma once
#include "KxFramework/KxFramework.h"
#include "KxEvent.h"
#include "Kx/Utility/System.h"

#include "Kx/General/Color.h"
using KxColor = KxFramework::Color;


KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, DWM_GLASS_COLOR_CHANGED);
KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, DWM_COMPOSITION_CHANGED);
KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, WINDOW_DPI_CHANGED);

namespace KxTLWInternal
{
	KX_API bool DWMIsCompositionEnabled();
	KX_API bool DWMIsGlassOpaque();
	KX_API KxColor DWMGetGlassColor();

	KX_API void SetDefaultBackgroundColor(wxWindow* window);
	KX_API bool MSWWindowProc(wxWindow* window, WXLRESULT& result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);

	KX_API KxColor DWMGetColorKey(const wxWindow* window);
	KX_API bool DWMExtendFrame(wxWindow* window);
	KX_API bool DWMExtendFrame(wxWindow* window, const wxRect& rect, const wxColour& color);
	KX_API bool DWMBlurBehind(wxWindow* window, bool enable, const wxRegion& region);

	KX_API wxIcon GetTitleIcon(const wxWindow* window);
	KX_API void SetTitleIcon(wxWindow* window, const wxIcon& icon);

	KX_API void* GetWindowUserData(HWND hWnd);
	KX_API void SetWindowUserData(HWND hWnd, const void* data);
}

template<class T = wxTopLevelWindow>
class KxTopLevelWindow: public T
{
	public:
		static bool DWMIsCompositionEnabled()
		{
			return KxTLWInternal::DWMIsCompositionEnabled();
		}
		static bool DWMIsGlassOpaque()
		{
			return KxTLWInternal::DWMIsGlassOpaque();
		}
		static KxColor DWMGetGlassColor()
		{
			return KxTLWInternal::DWMGetGlassColor();
		}

	private:
		T* GetThis()
		{
			return static_cast<T*>(this);
		}
		const T* GetThis() const
		{
			return static_cast<const T*>(this);
		}

	protected:
		void SetDefaultBackgroundColor()
		{
			KxTLWInternal::SetDefaultBackgroundColor(GetThis());
		}

	public:
		WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override
		{
			WXLRESULT result = 0;
			if (KxTLWInternal::MSWWindowProc(GetThis(), result, msg, wParam, lParam))
			{
				return result;
			}
			return T::MSWWindowProc(msg, wParam, lParam);
		}

		KxColor DWMGetColorKey() const
		{
			return KxTLWInternal::DWMGetColorKey(GetThis());
		}
		bool DWMExtendFrame()
		{
			return KxTLWInternal::DWMExtendFrame(GetThis());
		}
		bool DWMExtendFrame(const wxRect& rect, const wxColour& color = wxNullColour)
		{
			return KxTLWInternal::DWMExtendFrame(GetThis(), rect, color);
		}
		bool DWMBlurBehind(bool enable, const wxRegion& region = KxNullWxRegion)
		{
			return KxTLWInternal::DWMBlurBehind(GetThis(), enable, region);
		}

		wxIcon GetTitleIcon() const
		{
			return KxTLWInternal::GetTitleIcon(GetThis());
		}
		void SetTitleIcon(const wxIcon& icon = wxNullIcon)
		{
			KxTLWInternal::SetTitleIcon(GetThis(), icon);
		}

		bool EnableCloseButton(bool enable = true) override
		{
			int options = 0;
			if (enable)
			{
				options = MF_ENABLED;
			}
			else
			{
				options = MF_DISABLED|MF_GRAYED;
			}
			return ::EnableMenuItem(::GetSystemMenu(GetThis()->GetHandle(), FALSE), SC_CLOSE, MF_BYCOMMAND|options);
		}
		bool EnableMinimizeButton(bool enable = true) override
		{
			using namespace KxFramework;

			Utility::ModWindowStyle(GetThis()->GetHandle(), GWL_STYLE, WS_MINIMIZEBOX, enable);
			return true;
		}
		bool EnableMaximizeButton(bool enable = true) override
		{
			using namespace KxFramework;

			Utility::ModWindowStyle(GetThis()->GetHandle(), GWL_STYLE, WS_MAXIMIZEBOX, enable);
			return true;
		}

		void* GetWindowUserData() const
		{
			return KxTLWInternal::GetWindowUserData(GetThis()->GetHandle());
		}
		void SetWindowUserData(const void* data)
		{
			KxTLWInternal::SetWindowUserData(GetThis()->GetHandle(), data);
		}
};
