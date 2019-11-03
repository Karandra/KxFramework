#include "KxStdAfx.h"
#include "KxFramework/KxLabel.h"
#include "KxFramework/KxUxTheme.h"

namespace
{
	constexpr wxEllipsizeMode LabelEllipsizeMode = wxELLIPSIZE_END;
	constexpr int MinSingleLineHeight = 23;
}

wxIMPLEMENT_DYNAMIC_CLASS(KxLabel, wxStaticText)

const wxColour& KxLabel::GetStateColor() const
{
	if (IsThisEnabled())
	{
		switch (m_State)
		{
			case wxCONTROL_NONE:
			{
				return m_ColorNormal;
			}
			case wxCONTROL_FOCUSED:
			{
				return m_ColorHighlight;
			}
			case wxCONTROL_PRESSED:
			{
				return m_ColorClick;
			}
		};
		return m_ColorNormal;
	}
	return m_ColorDisabled;
}
wxSize KxLabel::CalcBestSize(wxDC* dc)
{
	const wxSize padding = ConvertDialogToPixels(wxSize(3, 1));

	if (m_IsMultilne)
	{
		wxSize textExtent;
		if (dc)
		{
			textExtent = dc->GetMultiLineTextExtent(m_Label);
		}
		else
		{
			wxClientDC clientDC(this);
			textExtent = clientDC.GetMultiLineTextExtent(m_Label);
		}
		return textExtent + padding;
	}
	else
	{
		return (dc ? dc->GetTextExtent(m_Label) : GetTextExtent(m_Label)) + padding;
	}
}

void KxLabel::OnEnter(wxMouseEvent& event)
{
	m_State = wxCONTROL_FOCUSED;
	Refresh();
	event.Skip();
}
void KxLabel::OnLeave(wxMouseEvent& event)
{
	m_State = wxCONTROL_NONE;
	Refresh();
	event.Skip();
}
void KxLabel::OnMouseDown(wxMouseEvent& event)
{
	m_State = wxCONTROL_PRESSED;
	Refresh();
	event.Skip();
}
void KxLabel::OnMouseUp(wxMouseEvent& event)
{
	m_State = wxCONTROL_FOCUSED;
	Refresh();
	event.Skip();

	wxTextUrlEvent evt(GetId(), event, 0, 0);
	evt.SetEventType(wxEVT_TEXT_URL);
	evt.SetEventObject(this);
	HandleWindowEvent(evt);
}
void KxLabel::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	if (IsFrozen())
	{
		return;
	}

	dc.SetBackgroundMode(wxPENSTYLE_TRANSPARENT);
	dc.SetBackground(GetParent()->GetBackgroundColour());

	wxRect rect(wxPoint(0, 0), GetSize());
	KxUxTheme::DrawParentBackground(*this, dc, rect);

	bool isSelected = m_State == wxCONTROL_FOCUSED || m_State == wxCONTROL_PRESSED;
	const wxColour& color = GetStateColor();
	wxString label = wxControl::Ellipsize(m_Label, dc, LabelEllipsizeMode, rect.GetWidth(), wxELLIPSIZE_FLAGS_NONE);

	// Draw main part
	if (m_Style & KxLABEL_SELECTION && isSelected)
	{
		wxRendererNative::Get().DrawItemSelectionRect(this, dc, rect, wxCONTROL_FOCUSED|wxCONTROL_SELECTED);
	}

	if (m_Style & KxLABEL_HYPERLINK && isSelected)
	{
		dc.SetFont(GetFont().MakeUnderlined());
	}

	wxBitmap image = m_Icon;
	if (!IsEnabled())
	{
		image = m_IconDisabled;
	}
	dc.SetTextForeground(color);
	wxRect labelRect = wxRect(rect);
	labelRect.SetLeft(2);

	wxSize contentSize;
	if (m_IsMultilne)
	{
		contentSize = CalcBestSize(&dc);

		int offset = 0;
		if (image.IsOk())
		{
			wxRect rect2 = labelRect;
			rect2.y += ConvertDialogToPixels(wxSize(0, 2)).GetHeight();

			dc.DrawLabel(wxEmptyString, image, rect2, m_MultiLineAlignStyle);
			offset = image.GetWidth() + ConvertDialogToPixels(wxSize(2, 0)).GetWidth();
		}

		wxPoint pos = labelRect.GetPosition();
		pos.x += offset;

		int spacingV = ConvertDialogToPixels(wxSize(0, 1)).GetHeight();
		pos.y += spacingV + spacingV / 2;
		dc.DrawText(label, pos);

		// Calculate best size
		if (image.IsOk())
		{
			contentSize.x += image.GetWidth();
		}
		m_BestSize = contentSize;
	}
	else
	{
		contentSize = dc.GetTextExtent(label);
		contentSize.SetHeight(MinSingleLineHeight);
		dc.DrawLabel(label, image, labelRect, m_AlignStyle);
	}


	if (m_Style & KxLABEL_LINE)
	{
		wxPoint pos1 = wxPoint(contentSize.GetWidth()+5, rect.GetHeight()/2 + 1);
		wxPoint pos2 = wxPoint(rect.GetWidth()-4, rect.GetHeight()/2 + 1);
		if (image.IsOk())
		{
			pos1.x += image.GetWidth()+4;
		}

		wxColour lineColor = wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR);
		if (!IsEnabled())
		{
			lineColor.MakeDisabled();
		}
		dc.SetPen(lineColor);
		dc.DrawLine(pos1, pos2);
	}
}

bool KxLabel::Create(wxWindow* parent,
					 wxWindowID id,
					 const wxString& label,
					 long style
)
{
	m_Style = style;
	m_IsMultilne = IsLabelMultiline(label);
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	if (wxStaticText::Create(parent, id, label, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE))
	{
		EnableSystemTheme();
		SetDoubleBuffered(true);
		SetLabel(label);

		wxColour color = GetForegroundColour();
		if (m_Style & KxLABEL_CAPTION)
		{
			wxFont font = GetFont();
			font.SetPointSize(font.GetPointSize() + 2);
			SetFont(font);
		}
		if (m_Style & KxLABEL_COLORED)
		{
			color = KxUxTheme::GetDialogMainInstructionColor(*this);
		}
		SetupColors(color);

		// Initial best size
		m_BestSize = CalcBestSize();

		// Events
		m_EvtHandler.Bind(wxEVT_PAINT, &KxLabel::OnPaint, this);
		m_EvtHandler.Bind(wxEVT_ENTER_WINDOW, &KxLabel::OnEnter, this);
		m_EvtHandler.Bind(wxEVT_LEAVE_WINDOW, &KxLabel::OnLeave, this);
		m_EvtHandler.Bind(wxEVT_LEFT_DOWN, &KxLabel::OnMouseDown, this);
		m_EvtHandler.Bind(wxEVT_LEFT_UP, &KxLabel::OnMouseUp, this);
		PushEventHandler(&m_EvtHandler);
		return true;
	}
	return false;
}
KxLabel::~KxLabel()
{
	PopEventHandler();
}

void KxLabel::SetBitmap(const wxBitmap& image)
{
	ScheduleRefresh();

	if (image.IsOk())
	{
		m_Icon = image;
		m_IconDisabled = image.ConvertToDisabled();

		wxSize size = GetSize();
		if (size.GetHeight() <= image.GetHeight())
		{
			SetSize(size.GetWidth(), image.GetHeight()+4);
		}
	}
	else
	{
		m_Icon = wxNullBitmap;
		m_IconDisabled = wxNullBitmap;
	}
}
