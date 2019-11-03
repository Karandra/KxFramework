#include "KxStdAfx.h"
#include "KxFramework/KxSplashWindow.h"
#include "KxFramework/KxUtility.h"
#include <wx/rawbmp.h>

wxIMPLEMENT_DYNAMIC_CLASS(KxSplashWindow, wxFrame)

void KxSplashWindow::OnTimer(wxTimerEvent& event)
{
	wxTheApp->ScheduleForDestruction(this);
}
void KxSplashWindow::OnSize(wxSizeEvent& event)
{
	ScheduleRefresh();
	event.Skip();
}

void KxSplashWindow::DoSetSplash(const wxBitmap& bitmap, const wxSize& size)
{
	m_Bitmap = bitmap;
	SetSize(size.IsFullySpecified() ? size : GetSize());
}
bool KxSplashWindow::DoUpdateSplash()
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
		if (wxSize size = GetSize(); size != image.GetSize())
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
void KxSplashWindow::DoCenterWindow()
{
	if (m_Style & KxSPLASH_CENTER_ON_PARENT)
	{
		CenterOnParent();
	}
	else
	{
		CenterOnScreen();
	}
}

bool KxSplashWindow::Create(wxWindow* parent,
							const wxBitmap& bitmap,
							const wxSize& size,
							wxTimeSpan timeout,
							int style
)
{
	m_Style = style;
	m_Timeout = timeout;

	int frameStyle = wxBORDER_NONE|wxWS_EX_TRANSIENT|wxFRAME_TOOL_WINDOW|wxFRAME_NO_TASKBAR|wxFRAME_SHAPED|wxTRANSPARENT_WINDOW;
	if (parent)
	{
		frameStyle |= wxFRAME_FLOAT_ON_PARENT;
	}

	if (wxFrame::Create(parent, wxID_NONE, wxEmptyString, wxDefaultPosition, size, frameStyle, GetClassInfo()->GetClassName()))
	{
		KxUtility::ToggleWindowStyle(GetHandle(), GWL_EXSTYLE, WS_EX_LAYERED|WS_EX_TOOLWINDOW, true);
		m_Timer.Bind(wxEVT_TIMER, &KxSplashWindow::OnTimer, this);
		m_Timer.Bind(wxEVT_SIZE, &KxSplashWindow::OnSize, this);

		DoSetSplash(bitmap, size);
		DoUpdateSplash();
		DoCenterWindow();
		return true;
	}
	return false;
}

KxSplashWindow::~KxSplashWindow()
{
	m_Timer.Stop();
}

void KxSplashWindow::SetWindowStyleFlag(long style)
{
	m_Style = style;
	ScheduleRefresh();
}

bool KxSplashWindow::Show(bool show)
{
	const bool result = wxFrame::Show(show);
	if (result && show && m_Timeout > 0)
	{
		m_Timer.StartOnce(m_Timeout.GetMilliseconds().GetValue());
	}
	return result;
}
void KxSplashWindow::Update()
{
	DoUpdateSplash();
}
void KxSplashWindow::Refresh(bool eraseBackground, const wxRect* rect)
{
	DoUpdateSplash();
}

void KxSplashWindow::SetSplashBitmap(const wxBitmap& bitmap, const wxSize& size)
{
	DoSetSplash(bitmap, size);
	DoCenterWindow();
	ScheduleRefresh();
}
void KxSplashWindow::SetSplashAlpha(uint8_t value)
{
	m_Alpha = value;
	ScheduleRefresh();
}
