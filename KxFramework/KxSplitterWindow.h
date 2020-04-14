#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/splitter.h>

class KX_API KxSplitterWindow: public wxSplitterWindow
{
	private:
		wxColour m_SashColor;
		int m_InitialPosition = 0;

	private:
		virtual void OnDoubleClickSash(int x, int y);
		void OnDrawSash(wxPaintEvent& event);

	public:
		static const long DefaultStyle = wxSP_LIVE_UPDATE|wxSP_NOBORDER|wxSP_3DSASH|wxSP_THIN_SASH;

		KxSplitterWindow() = default;
		KxSplitterWindow(wxWindow* parent,
						 wxWindowID id = wxID_ANY,
						 long style = DefaultStyle
		)
		{
			Create(parent, id, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id = wxID_ANY,
					long style = DefaultStyle
		);

	public:
		virtual bool ShouldInheritColours() const override
		{
			return true;
		}
		virtual bool SplitHorizontally(wxWindow* window1, wxWindow* window2, int sashPosition = 0) override;
		virtual bool SplitVertically(wxWindow* window1, wxWindow* window2, int sashPosition = 0) override;

		int GetInitialPosition() const
		{
			return m_InitialPosition;
		}
		void SetInitialPosition(int pos);

		wxColour GetSashColor() const
		{
			return m_SashColor;
		}
		void SetSashColor(const wxColour& color);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxSplitterWindow);
};
