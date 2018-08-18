#include "KxStdAfx.h"
#include "KxFramework/KxWebView.h"

wxIMPLEMENT_ABSTRACT_CLASS(KxWebView, wxWebView);

const wxString KxWebView::DefaultPage = "about:blank";

wxWebView* KxWebView::New(wxWindow* parent,
						  wxWindowID id,
						  const wxString& defaultURL,
						  const wxString& backend,
						  long style
)
{
	return wxWebView::New(parent, id, defaultURL, wxDefaultPosition, wxDefaultSize, backend, style);
}
