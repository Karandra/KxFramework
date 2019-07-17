#include "KxStdAfx.h"
#include "KxFramework/KxToolBar.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxIncludeWindows.h"

KxEVENT_DEFINE_GLOBAL(TOOLBAR, wxCommandEvent);
KxEVENT_DEFINE_GLOBAL(TOOLBAR_RIGHTCLICK, wxCommandEvent);
KxEVENT_DEFINE_GLOBAL(TOOLBAR_DROPDOWN, wxCommandEvent);
KxEVENT_DEFINE_GLOBAL(TOOLBAR_ENTER, wxCommandEvent);
KxEVENT_DEFINE_GLOBAL(TOOLBAR_LEAVE, wxCommandEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxToolBar, wxToolBar);

void KxToolBar::EventHandler(wxCommandEvent& event)
{
	#if 0
	SetFocus();

	wxToolBarToolBase* tool = FindById(event.GetId());
	if (tool)
	{
		KxMenu* menu = nullptr;
		if (event.GetEventType() == wxEVT_TOOL)
		{
			event.SetEventType(KxEVT_TOOLBAR);
		}
		else if (event.GetEventType() == wxEVT_TOOL_RCLICKED)
		{
			event.SetEventType(KxEVT_TOOLBAR_RIGHTCLICK);
		}
		else if (event.GetEventType() == wxEVT_TOOL_ENTER)
		{
			if (event.GetSelection() == -1)
			{
				event.SetEventType(KxEVT_TOOLBAR_LEAVE);
			}
			else
			{
				event.SetEventType(KxEVT_TOOLBAR_ENTER);
			}
		}
		else if (event.GetEventType() == wxEVT_TOOL_DROPDOWN)
		{
			menu = static_cast<KxMenu*>(tool->GetDropdownMenu());
			event.SetEventType(KxEVT_TOOLBAR_DROPDOWN);
		}

		event.SetEventObject(tool);
		event.SetId(tool->GetId());
		event.SetExtraLong(GetToolState(tool->GetId()));
		event.SetInt(tool->IsToggled());

		// Handle dropdown menu
		bool wasProcessed = tool->ProcessEvent(event);
		if (menu && (!wasProcessed || event.GetSkipped()))
		{
			menu->Show(this, GetDropdownMenuPosition(tool->GetId()));
		}
	}
	#endif
	event.Skip(false);
}
void KxToolBar::OnDrawLine(wxEraseEvent& event)
{
	wxDC* dc = event.GetDC();
	dc->SetPen(m_ColorBorder);
	wxSize size = GetSize();
	dc->DrawLine(wxPoint(0, size.GetHeight()), wxPoint(size.GetWidth(), size.GetHeight()));

	event.Skip();
}

bool KxToolBar::Create(wxWindow* parent,
					   wxWindowID id,
					   long style
)
{
	if (wxToolBar::Create(parent, id, wxDefaultPosition, wxDefaultSize, KxUtility::ModFlag(style, KxTB_MASK, false)))
	{
		int style = TBSTYLE_LIST|TBSTYLE_TRANSPARENT|CCS_NODIVIDER|CCS_NOPARENTALIGN;
		if (style & KxTB_FLAT)
		{
			style |= TBSTYLE_FLAT;
		}
		::SetWindowLongPtrW(GetHandle(), GWL_STYLE, GetWindowLongPtrW(GetHandle(), GWL_STYLE)|style);

		if (!(style & KxTB_DD_ARROWS))
		{
			::SendMessageW(GetHandle(), TB_SETEXTENDEDSTYLE, 0, 0);
		}

		SetToolBitmapSize(wxSize(22, 22));
		SetPadding(wxSize(2, 2));
		SetSpacing(1);
		SetToolSeparation(DefaultSeparatorSize);

		Bind(wxEVT_TOOL, &KxToolBar::EventHandler, this);
		Bind(wxEVT_TOOL_RCLICKED, &KxToolBar::EventHandler, this);
		Bind(wxEVT_TOOL_DROPDOWN, &KxToolBar::EventHandler, this);

		return true;
	}
	return false;
}
KxToolBar::~KxToolBar()
{
	for (size_t i = 0; i < GetToolsCount(); i++)
	{
		wxToolBarToolBase* tool = const_cast<wxToolBarToolBase*>(GetToolByPos(i));
		if (tool->GetKind() == wxITEM_DROPDOWN)
		{
			tool->SetDropdownMenu(nullptr);
		}
	}
}

wxSize KxToolBar::GetPadding() const
{
	wxSize padding;

	TBMETRICS info = {0};
	info.cbSize = sizeof(TBMETRICS);
	info.dwMask = TBMF_BARPAD;
	SendMessageW(GetHandle(), TB_GETMETRICS, 0, (LPARAM)&info);
	padding.SetWidth(m_Indent);
	padding.SetHeight(info.cyBarPad);

	return padding;
}
void KxToolBar::SetPadding(const wxSize& padding)
{
	TBMETRICS info = {0};
	info.cbSize = sizeof(TBMETRICS);
	info.dwMask = TBMF_BARPAD;
	info.cxBarPad = padding.GetWidth();
	info.cyBarPad = padding.GetHeight();
	SendMessageW(GetHandle(), TB_SETMETRICS, 0, (LPARAM)&info);
	SendMessageW(GetHandle(), TB_SETINDENT, padding.GetWidth(), 0);

	m_Indent = padding.GetWidth();
}
void KxToolBar::SetSpacing(int spacing)
{
	TBMETRICS info = {0};
	info.cbSize = sizeof(TBMETRICS);
	info.dwMask = TBMF_BUTTONSPACING;
	info.cxButtonSpacing = spacing;
	info.cyButtonSpacing = 0;
	SendMessageW(GetHandle(), TB_SETMETRICS, 0, (LPARAM)&info);
}
int KxToolBar::GetSpacing() const
{
	TBMETRICS info = {0};
	info.cbSize = sizeof(TBMETRICS);
	info.dwMask = TBMF_BUTTONSPACING;
	SendMessageW(GetHandle(), TB_GETMETRICS, 0, (LPARAM)&info);

	return info.cxButtonSpacing;
}
void KxToolBar::SetButtonSize(int id, const wxSize& size)
{
	TBBUTTONINFOW info = {0};
	info.cbSize = sizeof(TBBUTTONINFOW);
	info.dwMask = TBIF_SIZE|TBIF_BYINDEX;
	info.cx = size.GetWidth();
	info.lParam = id;

	SendMessageW(GetHandle(), TB_SETBUTTONINFO, id, (LPARAM)&info);
}
wxPoint KxToolBar::GetDropdownMenuPosition(int id) const
{
	RECT rectWin = {0};
	SendMessageW(GetHandle(), TB_GETITEMRECT, GetToolPos(id), (LPARAM)&rectWin);
	wxPoint pos = KxUtility::CopyRECTToRect(rectWin).GetLeftBottom();
	pos.y += 1;
	return pos;
}
