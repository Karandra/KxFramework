#pragma once
#include "Kx/UI/Common.h"
#include "Frame.h"
#include "Kx/UI/WindowRefreshScheduler.h"

namespace KxFramework::UI
{
	enum class SplashWindowStyle
	{
		None = 0,

		CenterOnParent = 1 << 0,
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::SplashWindowStyle);
}

namespace KxFramework::UI
{
	class KX_API SplashWindow: public WindowRefreshScheduler<Frame>
	{
		public:
			static constexpr SplashWindowStyle DefaultStyle = SplashWindowStyle::None;

		private:
			wxBitmap m_Bitmap;
			uint8_t m_Alpha = 255;
		
			wxTimer m_Timer;
			TimeSpan m_Timeout;
			SplashWindowStyle m_Style = DefaultStyle;

		private:
			void OnTimer(wxTimerEvent& event);
			void OnSize(wxSizeEvent& event);

		protected:
			virtual void DoSetSplash(const wxBitmap& bitmap, const Size& size);
			virtual bool DoUpdateSplash();
			virtual void DoCenterWindow();

		public:
			SplashWindow() = default;
			SplashWindow(wxWindow* parent,
						 const wxBitmap& bitmap,
						 TimeSpan timeout = {},
						 SplashWindowStyle style = DefaultStyle
			)
			{
				Create(parent, bitmap, timeout, style);
			}
			SplashWindow(wxWindow* parent,
						 const wxBitmap& bitmap,
						 const Size& size,
						 TimeSpan timeout = {},
						 SplashWindowStyle style = DefaultStyle
			)
			{
				Create(parent, bitmap, size, timeout, style);
			}
			bool Create(wxWindow* parent,
						const wxBitmap& bitmap,
						TimeSpan timeout = {},
						SplashWindowStyle style = DefaultStyle
			)
			{
				return Create(parent, bitmap, bitmap.GetSize(), timeout, style);
			}
			bool Create(wxWindow* parent,
						const wxBitmap& bitmap,
						const Size& size,
						TimeSpan timeout = {},
						SplashWindowStyle style = DefaultStyle
			);
			virtual ~SplashWindow();

		public:
			long GetWindowStyleFlag() const override
			{
				return ToInt(m_Style);
			}
			void SetWindowStyleFlag(long style) override;
		
			bool CanSetTransparent() override
			{
				return true;
			}
			bool SetTransparent(wxByte value) override
			{
				SetSplashAlpha(value);
				return true;
			}

			bool Show(bool show = true) override;
			void Update() override;
			void Refresh(bool eraseBackground = true, const wxRect* rect = nullptr) override;

			const wxBitmap& GetSplashBitmap() const
			{
				return m_Bitmap;
			}
			void SetSplashBitmap(const wxBitmap& bitmap, const Size& size = Size::UnspecifiedSize());
		
			uint8_t GetSplashAlpha() const
			{
				return m_Alpha;
			}
			void SetSplashAlpha(uint8_t value);
		
			TimeSpan GetTimeout() const
			{
				return m_Timeout;
			}
			void SetTimeout(TimeSpan timeout)
			{
				m_Timeout = timeout;
			}

			bool UpdateSplash()
			{
				return DoUpdateSplash();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(SplashWindow);
	};
}
