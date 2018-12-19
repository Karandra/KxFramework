#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxCOM.h"
#include <PropKey.h>
#include <ShObjIDL.h>
#include "KxFramework/KxWinUndef.h"

enum KxFBD_Mode
{
	KxFBD_OPEN,
	KxFBD_SAVE,
	KxFBD_OPEN_FOLDER,
};
enum KxFBD_Options
{
	KxFBD_NONE = 0,

	KxFBD_OVERWRITE_PROMPT = FOS_OVERWRITEPROMPT,
	KxFBD_STRICT_FILE_TYPES = FOS_STRICTFILETYPES,
	KxFBD_NO_CHANGE_CWD = FOS_NOCHANGEDIR,
	KxFBD_PICK_FOLDERS = FOS_PICKFOLDERS,
	KxFBD_FORCE_FILE_SYSTEM = FOS_FORCEFILESYSTEM,
	KxFBD_ALLOW_NONSTORAGE_ITEMS = FOS_ALLNONSTORAGEITEMS,
	KxFBD_NOVALIDATE = FOS_NOVALIDATE,
	KxFBD_ALLOW_MULTISELECT = FOS_ALLOWMULTISELECT,
	KxFBD_PATH_MUST_EXIST = FOS_PATHMUSTEXIST,
	KxFBD_FILE_MUST_EXIST = FOS_FILEMUSTEXIST,
	KxFBD_CREATE_PROMPT = FOS_CREATEPROMPT,
	KxFBD_SHAREAWARE = FOS_SHAREAWARE,
	KxFBD_NO_READONLY_RETURN = FOS_NOREADONLYRETURN,
	KxFBD_CREATE_NO_TEST = FOS_NOTESTFILECREATE,
	KxFBD_HIDEMRUPLACES = FOS_HIDEMRUPLACES, // Not supported Win7+
	KxFBD_HIDE_PINNED_PLACES = FOS_HIDEPINNEDPLACES,
	KxFBD_NO_DEREFERENCE_LINKS = FOS_NODEREFERENCELINKS,
	KxFBD_OK_BUTTON_NEEDS_INTERACTION = FOS_OKBUTTONNEEDSINTERACTION, // WTF is that?
	KxFBD_DONT_ADD_TO_RECENT = FOS_DONTADDTORECENT,
	KxFBD_FORCE_SHOW_HIDDEN = FOS_FORCESHOWHIDDEN,
	KxFBD_DEFAULTNOMINIMODE = FOS_DEFAULTNOMINIMODE, // Not supported Win7+
	KxFBD_FORCE_PREVIEW_PANE = FOS_FORCEPREVIEWPANEON,
	KxFBD_SUPPORT_STREAMABLE_ITEMS = FOS_SUPPORTSTREAMABLEITEMS
};

class KX_API KxFileBrowseDialog;
class KX_API KxFileBrowseDialogEvents: public IFileDialogEvents
{
	private:
		KxFileBrowseDialog* self = NULL;
		DWORD m_EventsCookie = 0;

	private:
		wxNotifyEvent CreateEvent(wxEventType nEventType);
		bool ProcessEvent(wxEvent& event) const;

	public:
		bool Create(KxFileBrowseDialog* fileBrowseDialog);
		void Destroy();
		virtual ~KxFileBrowseDialogEvents();

	public:
		HRESULT KX_COMCALL QueryInterface(REFIID riid, void** ppvObject) override
		{
			return E_NOINTERFACE;
		}
		ULONG KX_COMCALL AddRef() override;
		ULONG KX_COMCALL Release() override;

		HRESULT KX_COMCALL OnShow(IFileDialog* instance);
		HRESULT KX_COMCALL OnFileOk(IFileDialog* instance) override
		{
			wxNotifyEvent event = CreateEvent(KxEVT_STDDIALOG_BUTTON);
			event.SetId(wxID_OK);
			ProcessEvent(event);
			return event.IsAllowed() ? S_OK : S_FALSE;
		}
		HRESULT KX_COMCALL OnFolderChange(IFileDialog* instance) override;
		HRESULT KX_COMCALL OnFolderChanging(IFileDialog* instance, IShellItem* shellItem) override;
		HRESULT KX_COMCALL OnOverwrite(IFileDialog* instance, IShellItem* shellItem, FDE_OVERWRITE_RESPONSE* pResponse) override;
		HRESULT KX_COMCALL OnSelectionChange(IFileDialog* instance) override
		{
			wxNotifyEvent event = CreateEvent(KxEVT_STDDIALOG_SELECT);
			ProcessEvent(event);
			return S_OK;
		}
		HRESULT KX_COMCALL OnTypeChange(IFileDialog* instance) override;
		HRESULT KX_COMCALL OnShareViolation(IFileDialog* instance, IShellItem* shellItem, FDE_SHAREVIOLATION_RESPONSE* pResponse) override
		{
			return E_NOTIMPL;
		}
};

class KX_API KxFileBrowseDialog: public KxDialog, public KxIStdDialog
{
	friend class KxFileBrowseDialogEvents;

	public:
		typedef std::vector<std::pair<wxString, wxString>> ItemList;

	private:
		KxCOMPtr<IFileDialog> m_Instance;
		KxCOMPtr<IFileDialog2> m_InstanceExtra;

		HWND m_Handle = NULL;
		wxWindow* m_Parent = NULL;
		HRESULT m_ShowStatus = E_FAIL;
		KxFBD_Mode m_Mode = KxFBD_OPEN;
		
		wxString m_Title;
		wxString m_Label;
		std::vector<std::pair<wxString, wxString>> m_FilterList;
		std::vector<COMDLG_FILTERSPEC> m_FilterListSpec;
		KxFileBrowseDialogEvents m_Events;

	private:
		static wxString GetDisplayName(IShellItem* shellItem, SIGDN type);

		wxString NormalizeFileFilter(const wxString& rawFilter) const;
		wxString GetFullPathName(const wxString& sRelativePath);
		void AdjustModeFromOptions(FILEOPENDIALOGOPTIONS options, bool enable);
		void ApplyFileFilters();
		void InitHWND();
		void InitIFileDialog2();

	public:
		static const int DefaultStyle = 0;
		static const int DefaultFlags = FOS_OVERWRITEPROMPT|FOS_DONTADDTORECENT|FOS_FORCEFILESYSTEM;
		static const SIGDN DefaultItemType = SIGDN_FILESYSPATH;

		KxFileBrowseDialog() {}
		KxFileBrowseDialog(wxWindow* parent,
						   wxWindowID id,
						   KxFBD_Mode mode,
						   const wxString& caption = wxEmptyString,
						   const wxPoint& pos = wxDefaultPosition,
						   const wxSize& size = wxDefaultSize,
						   int buttons = DefaultButtons,
						   long style = DefaultStyle
		)
		{
			Create(parent, id, mode, caption, pos, size, buttons, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					KxFBD_Mode mode,
					const wxString& caption = wxEmptyString,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					int buttons = DefaultButtons,
					long style = DefaultStyle
		);
		virtual ~KxFileBrowseDialog();

	public:
		virtual WXWidget GetHandle() const override
		{
			return m_Handle;
		}
		bool IsOK() const
		{
			return m_Instance && SUCCEEDED(m_ShowStatus);
		}
		KxFBD_Mode GetMode() const
		{
			return m_Mode;
		}

		bool IsOptionEnabled(KxFBD_Options flag) const;
		void SetOptionEnabled(KxFBD_Options flag, bool isEnabled = true);
		KxFBD_Options GetOptions() const;
		void SetOptions(KxFBD_Options options);
		
		// Show and close the dialog
		virtual int ShowModal();
		virtual bool Show(bool show = true) override;
		virtual bool Close(bool force = false);
		virtual bool IsVisible() const override;

		// Window title
		virtual wxString GetTitle() const override
		{
			return m_Title;
		}
		virtual void SetTitle(const wxString& title) override
		{
			m_Title = title;
			if (m_Instance)
			{
				m_Instance->SetTitle(m_Title.IsEmpty() ? NULL : m_Title.wc_str());
			}
		}
		virtual wxString GetLabel() const
		{
			return m_Label;
		}
		virtual void SetLabel(const wxString& label) override
		{
			m_Label = label;
			if (m_Instance)
			{
				m_Instance->SetFileNameLabel(m_Label.wc_str());
			}
		}
		virtual wxString GetCaption() const
		{
			return GetTitle();
		}
		virtual void SetCaption(const wxString& label) override
		{
			SetTitle(label);
		}

		// Icons
		virtual wxBitmap GetMainIcon() const
		{
			return GetIcon();
		}
		virtual KxIconType GetMainIconID() const
		{
			return KxICON_NONE;
		}
		virtual void SetMainIcon(const wxBitmap& icon)
		{
			wxIcon temp;
			temp.CopyFromBitmap(icon);
			SetIcon(temp);
		}
		virtual void SetMainIcon(KxIconType iconID = DefaultIconID)
		{
		}

		// Other options
		void SetFolder(const wxString& path);
		void SetNavigationRoot(const wxString& path);
		void SetOKButtonLabel(const wxString& text)
		{
			if (m_Instance)
			{
				m_Instance->SetOkButtonLabel(text.wc_str());
			}
		}
		void SetCancelButtonLabel(const wxString& text)
		{
			if (m_InstanceExtra)
			{
				m_InstanceExtra->SetCancelButtonLabel(text.wc_str());
			}
		}
		wxString GetFileName() const
		{
			if (m_Instance)
			{
				LPWSTR name = NULL;
				if (SUCCEEDED(m_Instance->GetFileName(&name)) && name)
				{
					wxString out(name);
					CoTaskMemFree(name);
					return out;
				}
			}
			return wxEmptyString;
		}
		void SetFileName(const wxString& text)
		{
			if (m_Instance)
			{
				m_Instance->SetFileName(text.wc_str());
			}
		}
		void SetDefaultExtension(const wxString& text)
		{
			if (m_Instance)
			{
				if (text.Length() > 0 && text[0] == '.')
				{
					m_Instance->SetDefaultExtension(text.Mid(1).wc_str());
				}
				else
				{
					m_Instance->SetDefaultExtension(text.wc_str());
				}
			}
		}
		size_t GetSelectedFilter() const
		{
			if (m_Instance)
			{
				UINT index = 0;
				if (SUCCEEDED(m_Instance->GetFileTypeIndex(&index)) && index != 0 && index - 1 < m_FilterListSpec.size())
				{
					// This is one-based index: https://msdn.microsoft.com/ru-ru/library/windows/desktop/bb775958(v=vs.85).aspx
					return index - 1;
				}
			}
			return (size_t)-1;
		}
		void SetSelectedFilter(size_t filterIndex)
		{
			if (m_Instance && filterIndex < m_FilterListSpec.size())
			{
				// One-based index
				m_Instance->SetFileTypeIndex(filterIndex + 1);
			}
		}
		bool IsMultiSelect() const
		{
			return IsOptionEnabled(KxFBD_ALLOW_MULTISELECT);
		}
		void SetMultiSelect(bool flag)
		{
			SetOptionEnabled(KxFBD_ALLOW_MULTISELECT);
		}

		// Pinned places and filters
		void AddPlace(const wxString& path, const wxString& label = wxEmptyString, bool top = true);
		size_t AddFilter(const wxString& filter, const wxString& label = wxEmptyString);
		size_t AddFilter();

		// Results
		wxString GetResult() const;
		KxStringVector GetResults() const;

		// Buttons
		virtual void SetDefaultButton(wxWindowID id) override
		{
		}
		virtual KxStdDialogControl GetButton(wxWindowID id) const override
		{
			HWND hWnd = GetHandle();
			if (hWnd)
			{
				return id;
				//return reinterpret_cast<wxWindow*>(GetDlgItem(hWnd, id));
			}
			return wxID_NONE;
		}
		virtual KxStdDialogControl AddButton(wxWindowID id, const wxString& label, bool prepend)
		{
			return wxID_NONE;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxFileBrowseDialog);
};
