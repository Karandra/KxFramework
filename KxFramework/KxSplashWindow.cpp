#include "KxStdAfx.h"
#include "KxFramework/KxSplashWindow.h"
#include "KxFramework/KxUtility.h"
#include <wx/rawbmp.h>

wxIMPLEMENT_DYNAMIC_CLASS(KxSplashWindow, wxFrame)

void KxSplashWindow::OnTimer(wxTimerEvent& event)
{
	wxTheApp->ScheduleForDestruction(this);
}

void KxSplashWindow::DoSetSplash(const wxBitmap& bitmap)
{
	m_Bitmap = bitmap;
	SetSize(m_Bitmap.GetSize());
}
bool KxSplashWindow::DoUpdateSplash()
{
	// UpdateLayeredWindow expects premultiplied alpha
	wxImage image = m_Bitmap.ConvertToImage();
	if (image.IsOk())
	{
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
	}

	wxBitmap bitmap = wxBitmap(image, 32);
	wxMemoryDC dc(bitmap);

	BLENDFUNCTION blendFunction = {0};
	blendFunction.AlphaFormat = AC_SRC_OVER;
	blendFunction.BlendFlags = 0;
	blendFunction.SourceConstantAlpha = m_Alpha;
	blendFunction.AlphaFormat = AC_SRC_ALPHA;

	POINT pos = {0, 0};
	SIZE size = {bitmap.GetWidth(), bitmap.GetHeight()};
	return ::UpdateLayeredWindow(GetHandle(), NULL, NULL, &size, dc.GetHDC(), &pos, 0, &blendFunction, ULW_ALPHA);
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
							int timeout,
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

	if (wxFrame::Create(parent, wxID_NONE, wxEmptyString, wxDefaultPosition, bitmap.GetSize(), frameStyle, GetClassInfo()->GetClassName()))
	{
		KxUtility::ToggleWindowStyle(GetHandle(), GWL_EXSTYLE, WS_EX_LAYERED|WS_EX_TOOLWINDOW, true);
		m_Timer.Bind(wxEVT_TIMER, &KxSplashWindow::OnTimer, this);

		DoSetSplash(bitmap);
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
	Update();
}

void KxSplashWindow::Update()
{
	wxFrame::Update();
	DoUpdateSplash();
}
bool KxSplashWindow::Show(bool show)
{
	bool ret = wxFrame::Show(show);
	if (ret && show && m_Timeout > 0)
	{
		m_Timer.StartOnce(m_Timeout);
	}
	return ret;
}

void KxSplashWindow::SetSplashBitmap(const wxBitmap& bitmap)
{
	DoSetSplash(bitmap);
	DoCenterWindow();
	Update();
}
void KxSplashWindow::SetSplashAlpha(uint8_t value)
{
	m_Alpha = value;
	Update();
}
