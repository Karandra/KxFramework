#pragma once
#include "KxFramework/KxFramework.h"

enum
{
	KxSPLASH_NONE = 0,
	KxSPLASH_CENTER_ON_PARENT = 1 << 0,
};

class KxSplashWindow: public wxFrame
{
	private:
		wxBitmap m_Bitmap;
		uint8_t m_Alpha = 255;
		
		wxTimer m_Timer;
		int m_Timeout = 0;
		int m_Style = DefaultStyle;

	private:
		void OnTimer(wxTimerEvent& event);

	protected:
		virtual void DoSetSplash(const wxBitmap& bitmap);
		virtual bool DoUpdateSplash();
		virtual void DoCenterWindow();

	public:
		static const int DefaultStyle = KxSPLASH_NONE;

		KxSplashWindow() {}
		KxSplashWindow(wxWindow* parent,
					   const wxBitmap& bitmap,
					   int timeout = 0,
					   int style = DefaultStyle
		)
		{
			Create(parent, bitmap, timeout, style);
		}
		bool Create(wxWindow* parent,
					const wxBitmap& bitmap,
					int timeout = 0,
					int style = DefaultStyle
		);
		virtual ~KxSplashWindow();

	public:
		virtual long GetWindowStyleFlag() const override
		{
			return m_Style;
		}
		virtual void SetWindowStyleFlag(long style) override;
		
		virtual bool CanSetTransparent() override
		{
			return true;
		}
		virtual bool SetTransparent(wxByte value) override
		{
			SetSplashAlpha(value);
			return true;
		}

		virtual void Update() override;
		virtual bool Show(bool show = true) override;

		const wxBitmap& GetSplashBitmap() const
		{
			return m_Bitmap;
		}
		void SetSplashBitmap(const wxBitmap& bitmap);
		
		uint8_t GetSplashAlpha() const
		{
			return m_Alpha;
		}
		void SetSplashAlpha(uint8_t value);
		
		int GetTimeout() const
		{
			return m_Timeout;
		}
		void SetTimeout(int timeout)
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
