#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxIFileFinder.h"
#include "KxFramework/KxFileItem.h"

namespace KxArchive
{
	class KX_API IArchiveSearch;
}

namespace KxArchive
{
	class KX_API FileFinder: public KxIFileFinder
	{
		private:
			const IArchiveSearch* m_Archive = nullptr;
			wxEvtHandler* m_EvtHandler = nullptr;
			wxString m_SearchQuery;

			void* m_Handle = nullptr;
			bool m_IsCanceled = false;

		protected:
			bool OnFound(const KxFileItem& foundItem) override;

		public:
			FileFinder(const IArchiveSearch& archive, const wxString& searchQuery, wxEvtHandler* eventHandler = nullptr);
			FileFinder(const IArchiveSearch& archive, const wxString& source, const wxString& filter, wxEvtHandler* eventHandler = nullptr);
			~FileFinder();

		public:
			bool IsOK() const override;
			bool IsCanceled() const override
			{
				return m_IsCanceled;
			}

			bool Run() override;
			KxFileItem FindNext() override;
	};
}
