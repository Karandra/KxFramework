#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/collpane.h>

class KxCollapsiblePane: public wxCollapsiblePane
{
	private:
		wxEvtHandler m_EvtHandler;

	private:
		void OnCollapsedExpanded(wxCollapsiblePaneEvent& event);

	public:
		static const long DefaultStyle = wxCP_DEFAULT_STYLE|wxCP_NO_TLW_RESIZE;
		
		KxCollapsiblePane() {}
		KxCollapsiblePane(wxWindow* parent,
						  wxWindowID id,
						  const wxString& label,
						  long style = DefaultStyle
		)
		{
			Create(parent, id, label, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& label,
					long style = DefaultStyle
		);
		virtual ~KxCollapsiblePane();

	public:
		wxDECLARE_DYNAMIC_CLASS(KxCollapsiblePane);
};
