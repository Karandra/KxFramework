#include "stdafx.h"
#include "SplashWindow.h"
#include "kxf/Utility/System.h"
#include <wx/rawbmp.h>

namespace kxf::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(SplashWindow, wxFrame)

	void SplashWindow::OnTimer(wxTimerEvent& event)
	{
		wxTheApp->ScheduleForDestruction(this);
	}
	void SplashWindow::OnSize(wxSizeEvent& event)
	{
		ScheduleRefresh();
		event.Skip();
	}

	void SplashWindow::DoSetSplash(const wxBitmap& bitmap, const Size& size)
	{
		m_Bitmap = bitmap;
		SetSize(size.IsFullySpecified() ? size : Size(GetSize()));
	}
	bool SplashWindow::DoUpdateSplash()
	{
		wxImage image = m_Bitmap.ConvertToImage();
		if (image.IsOk())
		{
			// UpdateLayeredWindow expects premultiplied alpha
			if (!image.HasAlpha())
			{
				image.InitAlpha();
			}
			const size_t imageSize = image.GetWidth() * image.GetHeight();
			for (size_t i = 0; i < imageSize; i++)
			{
				uint8_t* value = image.GetAlpha() + i;
				*value = *value * m_Alpha / 255.0;
			}

			// Scale the image for window size
			if (Size size = GetSize(); size != image.GetSize())
			{
				image.Rescale(size.GetWidth(), size.GetHeight(), wxImageResizeQuality::wxIMAGE_QUALITY_HIGH);
			}
		}

		wxBitmap bitmap = wxBitmap(image, 32);
		wxMemoryDC dc(bitmap);

		BLENDFUNCTION blendFunction = {0};
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.BlendFlags = 0;
		blendFunction.SourceConstantAlpha = m_Alpha;
		blendFunction.AlphaFormat = AC_SRC_ALPHA;

		POINT pos = {0, 0};
		SIZE size = {bitmap.GetWidth(), bitmap.GetHeight()};
		return ::UpdateLayeredWindow(GetHandle(), nullptr, nullptr, &size, dc.GetHDC(), &pos, 0, &blendFunction, ULW_ALPHA);
	}
	void SplashWindow::DoCenterWindow()
	{
		if (m_Style & SplashWindowStyle::CenterOnParent)
		{
			CenterOnParent();
		}
		else
		{
			CenterOnScreen();
		}
	}

	bool SplashWindow::Create(wxWindow* parent,
							  const wxBitmap& bitmap,
							  const Size& size,
							  TimeSpan timeout,
							  FlagSet<SplashWindowStyle> style
	)
	{
		m_Style = style;
		m_Timeout = timeout;

		int frameStyle = wxBORDER_NONE|wxWS_EX_TRANSIENT|wxFRAME_TOOL_WINDOW|wxFRAME_NO_TASKBAR|wxFRAME_SHAPED|wxTRANSPARENT_WINDOW;
		if (parent)
		{
			frameStyle |= wxFRAME_FLOAT_ON_PARENT;
		}

		if (wxFrame::Create(parent, wxID_NONE, {}, Point::UnspecifiedPosition(), size, frameStyle, GetClassInfo()->GetClassName()))
		{
			Utility::ModWindowStyle(GetHandle(), GWL_EXSTYLE, WS_EX_LAYERED|WS_EX_TOOLWINDOW, true);
			m_Timer.Bind(wxEVT_TIMER, &SplashWindow::OnTimer, this);
			m_Timer.Bind(wxEVT_SIZE, &SplashWindow::OnSize, this);

			DoSetSplash(bitmap, size);
			DoUpdateSplash();
			DoCenterWindow();
			return true;
		}
		return false;
	}
	SplashWindow::~SplashWindow()
	{
		m_Timer.Stop();
	}

	bool SplashWindow::Show(bool show)
	{
		const bool result = wxFrame::Show(show);
		if (result && show && m_Timeout.IsPositive())
		{
			m_Timer.StartOnce(m_Timeout.GetMilliseconds());
		}
		return result;
	}
	void SplashWindow::Update()
	{
		DoUpdateSplash();
	}
	void SplashWindow::Refresh(bool eraseBackground, const wxRect* rect)
	{
		DoUpdateSplash();
	}

	void SplashWindow::SetSplashBitmap(const wxBitmap& bitmap, const Size& size)
	{
		DoSetSplash(bitmap, size);
		DoCenterWindow();
		ScheduleRefresh();
	}
	void SplashWindow::SetSplashAlpha(uint8_t value)
	{
		m_Alpha = value;
		ScheduleRefresh();
	}
}
