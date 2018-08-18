#include "KxStdAfx.h"
#include "KxFramework/KxImageView.h"
#include "KxFramework/KxPanel.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxImageView, wxControl);

void KxImageView::OnDrawBackground(wxEraseEvent& event)
{
	wxDC* dc = event.GetDC();
	dc->SetBackgroundMode(wxBG_STYLE_TRANSPARENT);
	dc->SetBackground(*wxTRANSPARENT_BRUSH);

	switch (m_BackgroundMode)
	{
		case KxIV_BG_SOLID:
		{
			dc->SetBackground(GetBackgroundColour());
			dc->Clear();
			break;
		}
		case KxIV_BG_GRADIENT:
		{
			dc->GradientFillLinear(GetSize(), GetForegroundColour(), GetBackgroundColour(), m_GradientDirection);
			break;
		}
		case KxIV_BG_TRANSPARENCY_PATTERN:
		{
			KxDrawablePanel::DrawTransparencyPattern(*dc);
			break;
		}
	};
}
void KxImageView::OnDrawForeground(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	m_ScaledImageSize = KxDrawablePanel::DrawScaledBitmap(dc, m_Bitmap, wxRect(wxPoint(0, 0), GetClientSize()), (KxDrawablePanel::ScaleMode)m_ScaleMode, m_ScaleFactor);
}

bool KxImageView::Create(wxWindow* parent,
						  wxWindowID id,
						  long style
)
{
	if (wxControl::Create(parent, id, wxDefaultPosition, wxDefaultSize, style|wxFULL_REPAINT_ON_RESIZE))
	{
		SetDoubleBuffered(true);
		if (ShouldInheritColours())
		{
			SetBackgroundColour(parent->GetBackgroundColour());
		}

		SetBackgroundStyle(wxBG_STYLE_ERASE);
		Bind(wxEVT_ERASE_BACKGROUND, &KxImageView::OnDrawBackground, this);
		Bind(wxEVT_PAINT, &KxImageView::OnDrawForeground, this);
		return true;
	}
	return false;
}
KxImageView::~KxImageView()
{
}

#if 0
wxAnimation KxImageView::GetAnimation() const
{
	if (m_IsAnimation)
	{
		return wxAnimationCtrl::GetAnimation();
	}
	return wxNullAnimation;
}
void KxImageView::SetAnimation(const wxAnimation& anim)
{
	m_IsAnimation = true;
	m_Bitmap = wxNullBitmap;
	Unbind(wxEVT_PAINT, &KxImageView::OnDrawForeground, this);
	wxAnimationCtrl::SetAnimation(anim);
	Refresh();
}
bool KxImageView::IsPlaying() const
{
	return m_IsAnimation && wxAnimationCtrl::IsPlaying();
}
bool KxImageView::Play()
{
	if (m_IsAnimation)
	{
		return wxAnimationCtrl::Play();
	}
	return false;
}
bool KxImageView::Play(bool isLoop)
{
	if (m_IsAnimation)
	{
		return wxAnimationCtrl::Play(isLoop);
	}
	return false;
}
void KxImageView::Stop()
{
	if (m_IsAnimation)
	{
		wxAnimationCtrl::Stop();
	}
}
void KxImageView::SetInactiveBitmap(const wxBitmap& bitmap)
{
	SetBitmap(bitmap);
	wxAnimationCtrl::SetInactiveBitmap(bitmap);
}
#endif

void KxImageView::SetScaleFactor(double factor)
{
	if (std::abs(m_ScaleFactor - factor) > 0.01)
	{
		m_ScaleFactor = factor;
		Refresh();
	}
}
void KxImageView::SetBitmap(const wxBitmap& image)
{
	m_Bitmap = image;
	m_IsAnimation = false;
	Refresh();
}
void KxImageView::LoadFile(const wxString& filePath, wxBitmapType type, int index)
{
	wxImage image;
	image.LoadFile(filePath, type, index);
	SetBitmap(wxBitmap(image, 32));
}
void KxImageView::LoadFile(wxInputStream& stream, wxBitmapType type, int index)
{
	wxImage image;
	image.LoadFile(stream, type, index);
	SetBitmap(wxBitmap(image, 32));
}
