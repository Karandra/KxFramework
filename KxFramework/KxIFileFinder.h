#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileItem.h"

class KX_API KxIFileFinder
{
	protected:
		static wxString Normalize(const wxString& source, bool start, bool end);
		static wxString ConstructSearchQuery(const wxString& source, const wxString& filter);
		static wxString ExtractSourceFromSearchQuery(const wxString& searchQuery);

	protected:
		virtual bool OnFound(const KxFileItem& foundItem) = 0;

	public:
		KxIFileFinder() = default;
		virtual ~KxIFileFinder() = default;

	public:
		virtual bool IsOK() const = 0;
		virtual bool IsCanceled() const = 0;

		virtual bool Run() = 0;
		virtual KxFileItem FindNext() = 0;
};
