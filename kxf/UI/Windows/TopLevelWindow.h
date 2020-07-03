#pragma once
#include "kxf/UI/Common.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/Utility/System.h"
#include <wx/toplevel.h>

namespace kxf::UI
{
	enum class TopLevelWindowStyle: uint32_t
	{
		None = 0,

		Caption = wxCAPTION,
		MinimizeBox = wxMINIMIZE_BOX,
		MaximizeBox = wxMAXIMIZE_BOX,
		CloseBox = wxCLOSE_BOX,
		StayOnTop = wxSTAY_ON_TOP,
		SystemMenu = wxSYSTEM_MENU,
		Resizeable = wxRESIZE_BORDER,
	};
}
namespace kxf
{
	Kx_DeclareFlagSet(UI::TopLevelWindowStyle);
}

namespace kxf::UI::Private
{
	class KX_API TopLevelWindowBase final
	{
		public:
			KxEVENT_MEMBER(wxNotifyEvent, DWMGlassColorChanged);
			KxEVENT_MEMBER(wxNotifyEvent, DWMCompositionChanged);

		public:
			static bool DWMIsCompositionEnabled();
			static bool DWMIsGlassOpaque();
			static Color DWMGetGlassColor();

		private:
			wxTopLevelWindow* m_Window = nullptr;

		public:
			TopLevelWindowBase(wxTopLevelWindow& window)
				:m_Window(&window)
			{
			}

		public:
			Color DWMGetColorKey() const;
			bool DWMExtendFrame();
			bool DWMExtendFrame(const Rect& rect, const Color& color);
			bool DWMBlurBehind(bool enable, const wxRegion& region);

			void SetDefaultBackgroundColor();
			bool MSWWindowProc(WXLRESULT& result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);

			wxIcon GetTitleIcon() const;
			void SetTitleIcon(const wxIcon& icon);

			bool EnableCloseButton(bool enable = true);
			bool EnableMinimizeButton(bool enable = true);
			bool EnableMaximizeButton(bool enable = true);

			void* GetWindowUserData() const;
			void SetWindowUserData(const void* data);
	};
}

namespace kxf::UI
{
	template<class T = wxTopLevelWindow>
	class TopLevelWindow: public T
	{
		public:
			KxEVENT_MEMBER_AS(wxNotifyEvent, DWMGlassColorChanged, Private::TopLevelWindowBase::EvtDWMGlassColorChanged);
			KxEVENT_MEMBER_AS(wxNotifyEvent, DWMCompositionChanged, Private::TopLevelWindowBase::EvtDWMCompositionChanged);

		public:
			static bool DWMIsCompositionEnabled()
			{
				return Private::TopLevelWindowBase::DWMIsCompositionEnabled();
			}
			static bool DWMIsGlassOpaque()
			{
				return Private::TopLevelWindowBase::DWMIsGlassOpaque();
			}
			static Color DWMGetGlassColor()
			{
				return Private::TopLevelWindowBase::DWMGetGlassColor();
			}

		private:
			Private::TopLevelWindowBase m_TLW;

		private:
			T& Self() noexcept
			{
				return static_cast<T&>(*this);
			}
			const T& Self() const noexcept
			{
				return static_cast<const T&>(*this);
			}

		protected:
			void SetDefaultBackgroundColor()
			{
				m_TLW.SetDefaultBackgroundColor();
			}
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override
			{
				WXLRESULT result = 0;
				if (m_TLW.MSWWindowProc(result, msg, wParam, lParam))
				{
					return result;
				}
				return T::MSWWindowProc(msg, wParam, lParam);
			}

		public:
			TopLevelWindow()
				:m_TLW(*this)
			{
				static_assert(std::is_base_of_v<wxTopLevelWindow, T>, "wxTopLevelWindow descendant required");
			}

		public:
			Color DWMGetColorKey() const
			{
				return m_TLW.DWMGetColorKey();
			}
			bool DWMExtendFrame()
			{
				return m_TLW.DWMExtendFrame();
			}
			bool DWMExtendFrame(const Rect& rect, const Color& color = {})
			{
				return m_TLW.DWMExtendFrame(rect, color);
			}
			bool DWMBlurBehind(bool enable, const wxRegion& region = {})
			{
				return m_TLW.DWMBlurBehind(enable, region);
			}

			wxIcon GetTitleIcon() const
			{
				return m_TLW.GetTitleIcon();
			}
			void SetTitleIcon(const wxIcon& icon)
			{
				m_TLW.SetTitleIcon(icon);
			}

			bool EnableCloseButton(bool enable = true) override
			{
				return m_TLW.EnableCloseButton(enable);
			}
			bool EnableMinimizeButton(bool enable = true) override
			{
				return m_TLW.EnableMinimizeButton(enable);
			}
			bool EnableMaximizeButton(bool enable = true) override
			{
				return m_TLW.EnableMaximizeButton(enable);
			}

			void* GetWindowUserData() const
			{
				return m_TLW.GetWindowUserData();
			}
			void SetWindowUserData(const void* data)
			{
				m_TLW.SetWindowUserData(data);
			}
	};
}
