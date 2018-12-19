#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxIFileFinder.h"
#include "KxFramework/KxFileItem.h"
class KX_API KxIArchiveSearch;

class KX_API KxArchiveFileFinder: public KxIFileFinder
{
	private:
		const KxIArchiveSearch* m_Archive = nullptr;
		wxEvtHandler* m_EvtHandler = nullptr;
		wxString m_SearchQuery;

		void* m_Handle = nullptr;
		bool m_IsCanceled = false;

	protected:
		virtual bool OnFound(const KxFileItem& foundItem) override;

	public:
		KxArchiveFileFinder(const KxIArchiveSearch& archive, const wxString& searchQuery, wxEvtHandler* eventHandler = nullptr);
		KxArchiveFileFinder(const KxIArchiveSearch& archive, const wxString& source, const wxString& filter, wxEvtHandler* eventHandler = nullptr);
		virtual ~KxArchiveFileFinder();

	public:
		virtual bool IsOK() const override;
		virtual bool IsCanceled() const override
		{
			return m_IsCanceled;
		}

		virtual bool Run() override;
		virtual KxFileItem FindNext() override;
};
