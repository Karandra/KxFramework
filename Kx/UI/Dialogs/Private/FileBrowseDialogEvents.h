#pragma once
#include "Kx/UI/Common.h"
#include "Kx/System/COM.h"
#include "Kx/EventSystem/Common.h"
#include <PropKey.h>
#include <ShObjIDL.h>
#include "Kx/System/UndefWindows.h"
#include <atomic>

namespace KxFramework::UI
{
	class KX_API FileBrowseDialog;
}

namespace KxFramework::UI::Private
{
	class KX_API FileBrowseDialogEvents: public IFileDialogEvents
	{
		private:
			std::atomic<ULONG> m_RefCount = 1;
			FileBrowseDialog* m_Dialog = nullptr;
			DWORD m_EventsCookie = 0;

		private:
			wxNotifyEvent CreateEvent(EventID nEventType);
			bool ProcessEvent(wxEvent& event) const;

		public:
			FileBrowseDialogEvents(FileBrowseDialog& fileBrowseDialog);
			~FileBrowseDialogEvents();

		public:
			// IUnknown
			HRESULT STDMETHODCALLTYPE QueryInterface(const ::IID& riid, void** ppvObject) override;
			ULONG STDMETHODCALLTYPE AddRef() override;
			ULONG STDMETHODCALLTYPE Release() override;

			// IFileDialogEvents
			HRESULT STDMETHODCALLTYPE OnShow(IFileDialog* instance);
			HRESULT STDMETHODCALLTYPE OnFileOk(IFileDialog* instance) override;
			HRESULT STDMETHODCALLTYPE OnFolderChange(IFileDialog* instance) override;
			HRESULT STDMETHODCALLTYPE OnFolderChanging(IFileDialog* instance, IShellItem* shellItem) override;
			HRESULT STDMETHODCALLTYPE OnOverwrite(IFileDialog* instance, IShellItem* shellItem, FDE_OVERWRITE_RESPONSE* pResponse) override;
			HRESULT STDMETHODCALLTYPE OnSelectionChange(IFileDialog* instance) override;
			HRESULT STDMETHODCALLTYPE OnTypeChange(IFileDialog* instance) override;
			HRESULT STDMETHODCALLTYPE OnShareViolation(IFileDialog* instance, IShellItem* shellItem, FDE_SHAREVIOLATION_RESPONSE* pResponse) override;
	};
}
