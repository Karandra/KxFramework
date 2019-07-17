#pragma once
#include "KxFramework/KxFramework.h"
#include "KxEvent.h"

class [[deprecated("Use KxAuiToolBar")]] KX_API KxToolBar;

KxEVENT_DECLARE_GLOBAL(TOOLBAR, wxCommandEvent);
KxEVENT_DECLARE_GLOBAL(TOOLBAR_RIGHTCLICK, wxCommandEvent);
KxEVENT_DECLARE_GLOBAL(TOOLBAR_DROPDOWN, wxCommandEvent);
KxEVENT_DECLARE_GLOBAL(TOOLBAR_ENTER, wxCommandEvent);
KxEVENT_DECLARE_GLOBAL(TOOLBAR_LEAVE, wxCommandEvent);

enum
{
	KxTB_LAST = wxTB_RIGHT,

	KxTB_DD_ARROWS = KxTB_LAST << 1,
	KxTB_FLAT = KxTB_LAST << 2,

	KxTB_MASK = KxTB_DD_ARROWS|KxTB_FLAT
};

class KX_API KxToolBar: public wxToolBar
{
	private:
		int m_Indent = 0;
		wxColour m_ColorBorder;

	private:
		virtual bool AcceptsFocus() const override
		{
			return true;
		}
		virtual bool AcceptsFocusRecursively() const override
		{
			return true;
		}
		void EventHandler(wxCommandEvent& event);
		void OnDrawLine(wxEraseEvent& event);

	public:
		static const long DefaultStyle = wxTB_DEFAULT_STYLE|wxTB_TEXT;
		static const int DefaultSeparatorSize = 2;

		KxToolBar() {}
		KxToolBar(wxWindow* parent,
				  wxWindowID id,
				  long style = DefaultStyle
		)
		{
			Create(parent, id, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle
		);
		virtual ~KxToolBar();

	public:
		wxColour GetBorderColor() const
		{
			return m_ColorBorder;
		}
		void SetBorderColor(const wxColour& color = wxNullColour)
		{
			if (color.IsOk())
			{
				m_ColorBorder = color;
			}
			else
			{
				m_ColorBorder = GetParent()->GetBackgroundColour().ChangeLightness(50);
			}
			Refresh();
		}
		wxSize GetPadding() const;
		void SetPadding(const wxSize& padding);
		int GetSpacing() const;
		void SetSpacing(int spacing);
		void SetButtonSize(int id, const wxSize& size);
		wxPoint GetDropdownMenuPosition(int id) const;

	public:
		wxDECLARE_DYNAMIC_CLASS(KxToolBar);
};
