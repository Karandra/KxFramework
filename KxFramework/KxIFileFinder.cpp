#include "KxStdAfx.h"
#include "KxFramework/KxIFileFinder.h"

wxString KxIFileFinder::Normalize(const wxString& source, bool start, bool end) const
{
	wxString out = source;
	if (start)
	{
		out.StartsWith(wxS("\\"), &out);
	}
	if (end)
	{
		out.EndsWith(wxS("\\"), &out);
	}
	return out;
}
wxString KxIFileFinder::ConstructSearchQuery(const wxString& source, const wxString& filter) const
{
	return !filter.IsEmpty() ? source + wxS('\\') + filter : source;
}
wxString KxIFileFinder::ExtrctSourceFromSearchQuery(const wxString& searchQuery) const
{
	wxString source = searchQuery.BeforeLast(wxS('\\'));
	return source.IsEmpty() ? searchQuery : source;
}
