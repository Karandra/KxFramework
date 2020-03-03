#include "KxStdAfx.h"
#include "KxFramework/KxIFileFinder.h"

wxString KxIFileFinder::Normalize(const wxString& source, bool start, bool end)
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
wxString KxIFileFinder::ConstructSearchQuery(const wxString& source, const wxString& filter)
{
	wxString out = !filter.IsEmpty() ? source + wxS('\\') + filter : source;

	out.Replace(wxS('/'), wxS('\\'), true);
	return out;
}
wxString KxIFileFinder::ExtractSourceFromSearchQuery(const wxString& searchQuery)
{
	wxString source = searchQuery.BeforeLast(wxS('\\'));
	return !source.IsEmpty() ? source : searchQuery;
}
