#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxWebView: public wxWebView
{
	public:
		static wxWebView* New(wxWindow* parent,
							  wxWindowID id,
							  const wxString& defaultURL = DefaultPage,
							  const wxString& backend = wxWebViewBackendDefault,
							  long style = DefaultStyle
		);

	public:
		static const wxString DefaultPage;
		static const int DefaultStyle = 0;

	public:
		wxDECLARE_ABSTRACT_CLASS(KxWebView);
};
