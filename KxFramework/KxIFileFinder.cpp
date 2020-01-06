#include "KxStdAfx.h"
#include "KxFramework/KxIFileFinder.h"

wxString KxIFileFinder::Normalize(const wxString& source, bool start, bool end) const
{
	wxString out = source;

	// Remove long path prefix
	out.StartsWith(wxS("\\\\?\\"), &out);

	if (start)
	{
		out.StartsWith(wxS("\\"), &out);
	}
	if (end)
	{
		out.EndsWith(wxS("\\"), &out);
	}

	out.Replace(wxS('/'), wxS('\\'), true);
	return out;
}
wxString KxIFileFinder::ConstructSearchQuery(const wxString& source, const wxString& filter) const
{
	wxString out = !filter.IsEmpty() ? source + wxS('\\') + filter : source;

	out.Replace(wxS('/'), wxS('\\'), true);
	return out;
}
wxString KxIFileFinder::ExtractSourceFromSearchQuery(const wxString& searchQuery) const
{
	wxString source = searchQuery.BeforeLast(wxS('\\'));
	return !source.IsEmpty() ? source : searchQuery;
}
