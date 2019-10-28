#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWindowRefreshScheduler.h"

enum
{
	KxSPLASH_NONE = 0,
	KxSPLASH_CENTER_ON_PARENT = 1 << 0,
};

class KX_API KxSplashWindow: public KxWindowRefreshScheduler<wxFrame>
{
	private:
		wxBitmap m_Bitmap;
		uint8_t m_Alpha = 255;
		
		wxTimer m_Timer;
		wxTimeSpan m_Timeout;
		int m_Style = DefaultStyle;

	private:
		void OnTimer(wxTimerEvent& event);
		void OnSize(wxSizeEvent& event);

	protected:
		virtual void DoSetSplash(const wxBitmap& bitmap, const wxSize& size);
		virtual bool DoUpdateSplash();
		virtual void DoCenterWindow();

	public:
		static const int DefaultStyle = KxSPLASH_NONE;

		KxSplashWindow() = default;
		KxSplashWindow(wxWindow* parent,
					   const wxBitmap& bitmap,
					   wxTimeSpan timeout = {},
					   int style = DefaultStyle
		)
		{
			Create(parent, bitmap, timeout, style);
		}
		KxSplashWindow(wxWindow* parent,
					   const wxBitmap& bitmap,
					   const wxSize& size,
					   wxTimeSpan timeout = {},
					   int style = DefaultStyle
		)
		{
			Create(parent, bitmap, size, timeout, style);
		}
		bool Create(wxWindow* parent,
					const wxBitmap& bitmap,
					wxTimeSpan timeout = {},
					int style = DefaultStyle
		)
		{
			return Create(parent, bitmap, bitmap.GetSize(), timeout, style);
		}
		bool Create(wxWindow* parent,
					const wxBitmap& bitmap,
					const wxSize& size,
					wxTimeSpan timeout = {},
					int style = DefaultStyle
		);
		virtual ~KxSplashWindow();

	public:
		long GetWindowStyleFlag() const override
		{
			return m_Style;
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
		void SetSplashBitmap(const wxBitmap& bitmap, const wxSize& size = wxDefaultSize);
		
		uint8_t GetSplashAlpha() const
		{
			return m_Alpha;
		}
		void SetSplashAlpha(uint8_t value);
		
		wxTimeSpan GetTimeout() const
		{
			return m_Timeout;
		}
		void SetTimeout(wxTimeSpan timeout)
		{
			m_Timeout = timeout;
		}

		bool UpdateSplash()
		{
			return DoUpdateSplash();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxSplashWindow);
};
