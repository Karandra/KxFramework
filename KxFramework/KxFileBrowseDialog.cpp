#include "KxStdAfx.h"
#include "KxFramework/KxFileBrowseDialog.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxFileBrowseDialog, KxStdDialog);

wxString KxFileBrowseDialog::GetDisplayName(IShellItem* shellItem, SIGDN type)
{
	LPWSTR name = NULL;
	if (SUCCEEDED(shellItem->GetDisplayName(type, &name)) && name)
	{
		wxString out(name);
		CoTaskMemFree(name);
		return out;
	}
	return wxEmptyString;
}

wxString KxFileBrowseDialog::NormalizeFileFilter(const wxString& rawFilter) const
{
	// Not implemented yet
	return rawFilter;
}
wxString KxFileBrowseDialog::GetFullPathName(const wxString& sRelativePath)
{
	if (!sRelativePath.IsEmpty())
	{
		DWORD length = GetFullPathNameW(sRelativePath, NULL, NULL, NULL);
		wxString out;
		GetFullPathNameW(sRelativePath, length, wxStringBuffer(out, length), NULL);
		return out;
	}
	else
	{
		return sRelativePath;
	}
}
void KxFileBrowseDialog::AdjustModeFromOptions(FILEOPENDIALOGOPTIONS options, bool enable)
{
	if (options & KxFBD_PICK_FOLDERS)
	{
		if (GetMode() == KxFBD_OPEN && enable)
		{
			m_Mode = KxFBD_OPEN_FOLDER;
		}
		else if (GetMode() == KxFBD_OPEN_FOLDER && !enable)
		{
			m_Mode = KxFBD_OPEN;
		}
	}
}
void KxFileBrowseDialog::ApplyFileFilters()
{
	if (m_Instance)
	{
		for (size_t i = 0; i < m_FilterList.size(); i++)
		{
			auto& spec = m_FilterListSpec[i];
			spec.pszSpec = m_FilterList[i].first;
			spec.pszName = m_FilterList[i].second;
		}
		m_Instance->SetFileTypes(m_FilterListSpec.size(), m_FilterListSpec.data());
	}
}
void KxFileBrowseDialog::InitHWND()
{
	HWND hWnd = NULL;
	if (m_Instance)
	{
		KxCOMPtr<IOleWindow> window;
		if (SUCCEEDED(m_Instance->QueryInterface(IID_PPV_ARGS(&window))) && window)
		{
			window->GetWindow(&hWnd);
		}
	}
	m_Handle = hWnd;
}
void KxFileBrowseDialog::InitIFileDialog2()
{
	IFileDialog2* dialog = NULL;
	if (SUCCEEDED(m_Instance->QueryInterface(IID_PPV_ARGS(&dialog))) && dialog)
	{
		m_InstanceExtra = dialog;
	}
}

bool KxFileBrowseDialog::Create(wxWindow* parent,
								wxWindowID id,
								KxFBD_Mode mode,
								const wxString& caption,
								const wxPoint& pos,
								const wxSize& size,
								int buttons,
								long style
)
{
	if (KxDialog::Create(parent, id, caption, pos, size, style))
	{
		m_Parent = parent;
		m_Mode = mode;

		// Create instance
		HRESULT res = E_FAIL;
		const CLSCTX context = CLSCTX_INPROC_SERVER;
		if (m_Mode == KxFBD_SAVE)
		{
			IFileSaveDialog* saveDialog = NULL;
			res = CoCreateInstance(CLSID_FileSaveDialog, NULL, context, IID_PPV_ARGS(&saveDialog));
			m_Instance = saveDialog;
		}
		else
		{
			IFileOpenDialog* openDialog = NULL;
			res = CoCreateInstance(CLSID_FileOpenDialog, NULL, context, IID_PPV_ARGS(&openDialog));
			m_Instance = openDialog;
		}

		if (m_Instance && SUCCEEDED(res))
		{
			// Options
			FILEOPENDIALOGOPTIONS options = DefaultFlags | (m_Mode == KxFBD_OPEN_FOLDER ? FOS_PICKFOLDERS : 0);
			FILEOPENDIALOGOPTIONS nOptionsDefault = 0;
			if (SUCCEEDED(m_Instance->GetOptions(&nOptionsDefault)))
			{
				options |= nOptionsDefault;
			}
			m_Instance->SetOptions(options);

			if (!caption.IsEmpty())
			{
				SetCaption(caption);
			}

			InitIFileDialog2();
			m_Events.Create(this);

			return true;
		}
	}
	return false;
}
KxFileBrowseDialog::~KxFileBrowseDialog()
{
	m_Events.Destroy();
}

int KxFileBrowseDialog::ShowModal()
{
	wxWindowID retCode = wxID_CANCEL;
	if (m_Instance)
	{
		// Add empty filter if there is no user filters
		if (m_FilterListSpec.empty())
		{
			AddFilter();
		}

		// Apply pending changes
		ApplyFileFilters();

		m_ShowStatus = m_Instance->Show(m_Parent ? m_Parent->GetHandle() : NULL);
		if (SUCCEEDED(m_ShowStatus))
		{
			retCode = wxID_OK;
		}
	}

	KxDialog::SetReturnCode(retCode);
	return retCode;
}
bool KxFileBrowseDialog::Show(bool show)
{
	return ShowNativeWindow(this, show);
}
bool KxFileBrowseDialog::Close(bool force)
{
	if (m_Instance)
	{
		m_Instance->Close(m_ShowStatus);
	}
	return KxDialog::Close(force);
}
bool KxFileBrowseDialog::IsVisible() const
{
	return GetHandle() != NULL && ::IsWindowVisible(GetHandle());
}

bool KxFileBrowseDialog::IsOptionEnabled(KxFBD_Options flag) const
{
	FILEOPENDIALOGOPTIONS options = 0;
	if (m_Instance && SUCCEEDED(m_Instance->GetOptions(&options)))
	{
		return options & flag;
	}
	return false;
}
void KxFileBrowseDialog::SetOptionEnabled(KxFBD_Options flag, bool isEnabled)
{
	FILEOPENDIALOGOPTIONS options = 0;
	if (m_Instance && SUCCEEDED(m_Instance->GetOptions(&options)))
	{
		AdjustModeFromOptions(flag, isEnabled);
		options = KxUtility::ModFlag(options, flag, isEnabled);
		m_Instance->SetOptions(options);
	}
}
KxFBD_Options KxFileBrowseDialog::GetOptions() const
{
	if (m_Instance)
	{
		FILEOPENDIALOGOPTIONS options = 0;
		if (SUCCEEDED(m_Instance->GetOptions(&options)))
		{
			return static_cast<KxFBD_Options>(options);
		}
	}
	return KxFBD_NONE;
}
void KxFileBrowseDialog::SetOptions(KxFBD_Options options)
{
	if (m_Instance)
	{
		AdjustModeFromOptions(options, options & KxFBD_PICK_FOLDERS);
		m_Instance->SetOptions(options);
	}
}

void KxFileBrowseDialog::SetFolder(const wxString& path)
{
	if (m_Instance)
	{
		KxCOMPtr<IShellItem> folderPath;
		SHCreateItemFromParsingName(path, NULL, IID_PPV_ARGS(&folderPath));
		if (folderPath)
		{
			m_Instance->SetFolder(folderPath);
		}
	}
}
void KxFileBrowseDialog::SetNavigationRoot(const wxString& path)
{
	if (m_InstanceExtra)
	{
		KxCOMPtr<IShellItem> folderPath;
		SHCreateItemFromParsingName(path, NULL, IID_PPV_ARGS(&folderPath));
		if (folderPath)
		{
			m_InstanceExtra->SetNavigationRoot(folderPath);
		}
	}
}
void KxFileBrowseDialog::AddPlace(const wxString& path, const wxString& label, bool top)
{
	if (m_Instance)
	{
		KxCOMPtr<IShellItem> pathItem;
		SHCreateItemFromParsingName(path, NULL, IID_PPV_ARGS(&pathItem));
		if (pathItem)
		{
			if (!label.IsEmpty())
			{
				size_t length = label.Length() * sizeof(WCHAR) + sizeof(WCHAR);
				LPWSTR labelCOM = (LPWSTR)CoTaskMemAlloc(length);
				wsprintfW(labelCOM, L"%s", label.wc_str());

				PROPVARIANT nameVariant = {0};
				nameVariant.vt = VT_LPWSTR;
				nameVariant.pwszVal = labelCOM;
				SHSetTemporaryPropertyForItem(pathItem, PKEY_ItemNameDisplay, nameVariant);
				CoTaskMemFree(labelCOM);
			}

			m_Instance->AddPlace(pathItem, top ? FDAP_TOP : FDAP_BOTTOM);
		}
	}
}
size_t KxFileBrowseDialog::AddFilter(const wxString& filter, const wxString& label)
{
	if (filter.IsEmpty() && label.IsEmpty())
	{
		return AddFilter();
	}
	else if (label.IsEmpty())
	{
		// Create label from filter
		wxString temp = wxString(filter);
		temp.Replace(";", ", ", true);
		temp = wxString::Format("%s (%s)", label, temp);

		m_FilterList.push_back(std::make_pair(NormalizeFileFilter(filter), temp));
	}
	else
	{
		m_FilterList.push_back(std::make_pair(NormalizeFileFilter(filter), label));
	}
	m_FilterListSpec.push_back(COMDLG_FILTERSPEC());
	return m_FilterListSpec.size() - 1;
}
size_t KxFileBrowseDialog::AddFilter()
{
	return AddFilter("*", "*");
}

wxString KxFileBrowseDialog::GetResult() const
{
	if (m_Instance)
	{
		if (IsMultiSelect() && GetMode() != KxFBD_SAVE)
		{
			IFileOpenDialog* dialog = static_cast<IFileOpenDialog*>(m_Instance.Get());
			IShellItemArray* shellList = NULL;
			if (SUCCEEDED(dialog->GetResults(&shellList)) && shellList)
			{
				DWORD count = 0;
				shellList->GetCount(&count);
				if (count != 0)
				{
					IShellItem* shellItem = NULL;
					if (SUCCEEDED(shellList->GetItemAt(0, &shellItem)) && shellItem)
					{
						return GetDisplayName(shellItem, DefaultItemType);
					}
				}
			}
		}
		else
		{
			IShellItem* shellItem = NULL;
			if (SUCCEEDED(m_Instance->GetResult(&shellItem)) && shellItem)
			{
				return GetDisplayName(shellItem, DefaultItemType);
			}
		}
	}
	return wxEmptyString;
}
KxStringVector KxFileBrowseDialog::GetResults() const
{
	KxStringVector list;
	if (m_Instance)
	{
		if (GetMode() == KxFBD_SAVE)
		{
			wxString out = GetResult();
			if (!out.IsEmpty())
			{
				list.push_back(out);
			}
		}
		else
		{
			IFileOpenDialog* dialog = static_cast<IFileOpenDialog*>(m_Instance.Get());
			IShellItemArray* shellList = NULL;
			if (SUCCEEDED(dialog->GetResults(&shellList)) && shellList)
			{
				DWORD count = 0;
				shellList->GetCount(&count);
				for (DWORD i = 0; i < count; i++)
				{
					IShellItem* shellItem = NULL;
					if (SUCCEEDED(shellList->GetItemAt(i, &shellItem)) && shellItem)
					{
						wxString out = GetDisplayName(shellItem, DefaultItemType);
						if (!out.IsEmpty())
						{
							list.push_back(out);
						}
					}
				}
			}
		}
	}
	return list;
}

//////////////////////////////////////////////////////////////////////////
wxNotifyEvent KxFileBrowseDialogEvents::CreateEvent(wxEventType nEventType)
{
	wxNotifyEvent event;
	event.SetEventType(nEventType);
	event.SetEventObject(self);
	event.SetId(wxID_NONE);
	event.Allow();

	return event;
}
bool KxFileBrowseDialogEvents::ProcessEvent(wxEvent& event) const
{
	if (self)
	{
		return self->HandleWindowEvent(event) && !event.GetSkipped();
	}
	return false;
}

bool KxFileBrowseDialogEvents::Create(KxFileBrowseDialog* fileBrowseDialog)
{
	self = fileBrowseDialog;
	if (fileBrowseDialog)
	{
		return SUCCEEDED(fileBrowseDialog->m_Instance->Advise(this, &m_EventsCookie));
	}
	return false;
}
void KxFileBrowseDialogEvents::Destroy()
{
	if (self)
	{
		self->m_Instance->Unadvise(m_EventsCookie);
		
		self = NULL;
		m_EventsCookie = 0;
	}
}
KxFileBrowseDialogEvents::~KxFileBrowseDialogEvents()
{
	Destroy();
}

ULONG KX_COMCALL KxFileBrowseDialogEvents::AddRef()
{
	// On Create
	self->m_Handle = NULL;

	return 0;
}
ULONG KX_COMCALL KxFileBrowseDialogEvents::Release()
{
	// On Destroy
	self->m_Handle = NULL;

	wxNotifyEvent event = CreateEvent(wxEVT_CLOSE_WINDOW);
	ProcessEvent(event);
	return 0;
}

HRESULT KX_COMCALL KxFileBrowseDialogEvents::OnShow(IFileDialog* instance)
{
	self->InitHWND();

	wxNotifyEvent event = CreateEvent(wxEVT_SHOW);
	ProcessEvent(event);
	return S_OK;
}
HRESULT KX_COMCALL KxFileBrowseDialogEvents::OnFolderChange(IFileDialog* instance)
{
	if (self->GetHandle() == NULL)
	{
		OnShow(instance);
	}

	wxNotifyEvent event = CreateEvent(KxEVT_STDDIALOG_NAVIGATE);
	ProcessEvent(event);
	return S_OK;
}
HRESULT KX_COMCALL KxFileBrowseDialogEvents::OnFolderChanging(IFileDialog* instance, IShellItem* shellItem)
{
	wxNotifyEvent event = CreateEvent(KxEVT_STDDIALOG_NAVIGATING);
	event.SetString(KxFileBrowseDialog::GetDisplayName(shellItem, SIGDN_NORMALDISPLAY));
	ProcessEvent(event);
	return event.IsAllowed() ? S_OK : S_FALSE;
}
HRESULT KX_COMCALL KxFileBrowseDialogEvents::OnOverwrite(IFileDialog* instance, IShellItem* shellItem, FDE_OVERWRITE_RESPONSE* pResponse)
{
	wxNotifyEvent event = CreateEvent(KxEVT_STDDIALOG_OVERWRITE);
	event.SetString(KxFileBrowseDialog::GetDisplayName(shellItem, SIGDN_NORMALDISPLAY));
	event.SetInt(*pResponse);
	ProcessEvent(event);
	return S_OK;
}
HRESULT KX_COMCALL KxFileBrowseDialogEvents::OnTypeChange(IFileDialog* instance)
{
	wxNotifyEvent event = CreateEvent(KxEVT_STDDIALOG_TYPE_CHANGED);
	event.SetInt(self->GetSelectedFilter());
	ProcessEvent(event);
	return S_OK;
}
