#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileItem.h"

class KX_API KxIFileFinder
{
	protected:
		virtual bool OnFound(const KxFileItem& foundItem) = 0;

		wxString Normalize(const wxString& source, bool start, bool end) const;
		wxString ConstructSearchQuery(const wxString& source, const wxString& filter) const;
		wxString ExtrctSourceFromSearchQuery(const wxString& searchQuery) const;

	public:
		KxIFileFinder() = default;
		virtual ~KxIFileFinder() = default;

	public:
		virtual bool IsOK() const = 0;
		virtual bool IsCanceled() const = 0;

		virtual bool Run() = 0;
		virtual KxFileItem FindNext() = 0;
};
