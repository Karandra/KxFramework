#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxLabel.h"
#include "KxFramework/KxPanel.h"
#include "Kx/UI/StdIcon.h"
#include "Kx/UI/StdButton.h"
#include "Kx/UI/StdDialogButtonSizer.h"
class KX_API KxButton;

KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, STDDIALOG_BUTTON);
KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, STDDIALOG_NAVIGATE);
KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, STDDIALOG_NAVIGATING);
KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, STDDIALOG_OVERWRITE);
KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, STDDIALOG_SELECT);
KxEVENT_DECLARE_GLOBAL(wxNotifyEvent, STDDIALOG_TYPE_CHANGED);

class KX_API KxStdDialogControl
{
	public:
		enum class Type
		{
			ID,
			Window,
			Button,
			RadioButton,
		};

	private:
		union
		{
			wxWindowID ID = wxID_NONE;
			wxWindow* Control;
		} m_Data;
		Type m_Type = Type::ID;

	public:
		KxStdDialogControl(wxWindowID id = wxID_NONE)
			:m_Type(Type::ID)
		{
			m_Data.ID = id;
		}
		KxStdDialogControl(wxWindow* control)
			:m_Type(Type::Window)
		{
			m_Data.Control = control;
		}
		KxStdDialogControl(KxButton* control);

	public:
		bool IsID() const
		{
			return m_Type == Type::ID;
		}
		bool IsControl() const
		{
			return !IsID();
		}
		Type GetType() const
		{
			return m_Type;
		}
		wxWindow* GetControl() const
		{
			if (IsControl())
			{
				return m_Data.Control;
			}
			return nullptr;
		}
		template<class T> T* As() const
		{
			if (IsControl())
			{
				return static_cast<T*>(m_Data.Control);
			}
			return nullptr;
		}

		operator wxWindowID() const;
		operator wxWindow*() const;
		operator KxButton*() const;
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxIStdDialog
{
	public:
		using StdButton = KxFramework::StdButton;
		using StdIcon = KxFramework::StdIcon;

	public:
		static const StdIcon DefaultIconID = StdIcon::Information;
		static const StdButton DefaultButtons = StdButton::OK|StdButton::Cancel;

	protected:
		bool ShowNativeWindow(wxDialog* window, bool show); // Will show/hide window if GetHandle() != nullptr and call ShowModal() otherwise
		int TranslateIconIDToWx(StdIcon id) const;
		StdIcon TranslateIconIDFromWx(int id) const;

	public:
		virtual wxString GetCaption() const = 0;
		virtual void SetCaption(const wxString& label) = 0;

		virtual wxString GetLabel() const = 0;
		virtual void SetLabel(const wxString& label) = 0;

		virtual wxBitmap GetMainIcon() const = 0;
		virtual StdIcon GetMainIconID() const = 0;
		virtual void SetMainIcon(const wxBitmap& icon) = 0;
		virtual void SetMainIcon(StdIcon iconID = DefaultIconID) = 0;

		virtual void SetDefaultButton(wxWindowID id) = 0;
		virtual KxStdDialogControl GetButton(wxWindowID id) const = 0;
		virtual KxStdDialogControl AddButton(wxWindowID id, const wxString& label = wxEmptyString, bool prepend = false) = 0;
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxStdDialog: public KxDialog, public KxIStdDialog
{
	public:
		using StdButtonsIDs = std::vector<wxWindowID>;

		static const StdButtonsIDs ms_DefaultCloseIDs;
		static const StdButtonsIDs ms_DefaultEnterIDs;
		static const wxColour ms_WindowBackgroundColor;
		static const wxColour ms_LineBackgroundColor;

	private:
		static bool ms_IsBellAllowed;

	public:
		static bool IsBellAllowed()
		{
			return ms_IsBellAllowed;
		}
		static void SetAllowBell(bool allow)
		{
			ms_IsBellAllowed = allow;
		}

	protected:
		wxOrientation m_DialogResizeSide = wxBOTH;
		KxLabel* m_CaptionLabel = nullptr;
		KxLabel* m_ViewLabel = nullptr;
		KxPanel* m_ContentPanel = nullptr;
		wxStaticBitmap* m_IconView = nullptr;
		wxBitmap m_MainIcon = wxNullBitmap;
		StdIcon m_MainIconID = DefaultIconID;
		std::vector<wxWindow*> m_UserControls;
		bool m_IsAutoSizeEnabled = true;

	private:
		KxPanel* m_ContentPanelLine = nullptr;
		wxSizer* m_DialogSizer = nullptr;
		wxSizer* m_ContentSizerBase = nullptr;
		wxSizer* m_ContentSizer = nullptr;
		wxSizer* m_IconSizer = nullptr;
		KxFramework::UI::StdDialogButtonSizer* m_ButtonsSizer = nullptr;
		bool m_IsGlassFrameEnabled = false;
		wxNativeWindow m_GripperWindow;
		wxSize m_GripperWindowSize;

		wxSizerItem* m_ViewLabelSpacerSI = nullptr;
		wxSizerItem* m_ViewLabelSI = nullptr;

		StdButtonsIDs m_CloseIDs = ms_DefaultCloseIDs;
		StdButtonsIDs m_EnterIDs = ms_DefaultEnterIDs;
		StdButton m_SelectedButtons = StdButton::None;

	protected:
		void PostCreate(const wxPoint& pos = wxDefaultPosition);
		virtual int GetViewSizerProportion() const
		{
			return 1;
		}
		virtual wxOrientation GetViewSizerOrientation() const;
		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual bool IsCloseBoxEnabled() const
		{
			return IsEscapeAllowed();
		}
		virtual bool IsEscapeAllowed(wxWindowID* idOut = nullptr) const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const;
		void SetResizingBehavior();
		static wxWindowID TranslateButtonConstantsToIDs(int btnValue);
		virtual wxRect GetGlassRect() const
		{
			return wxRect(0, 0, 0, m_ButtonsSizer->GetSize().GetHeight() + 1);
		}

	private:
		void OnStdButtonClick(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnEscape(wxKeyEvent& event);
		void InitIcon()
		{
			if (m_MainIcon.IsOk() != true && m_MainIconID != StdIcon::None)
			{
				SetMainIcon();
				LoadIcon();
			}
			else
			{
				SetIconVisibility();
			}
		}
		void LoadIcon()
		{
			m_IconView->SetBitmap(m_MainIcon);
		}
		void SetIconVisibility()
		{
			if (m_MainIconID == StdIcon::None || !m_MainIcon.IsOk())
			{
				m_ContentSizerBase->Hide(m_IconSizer);
			}
			else
			{
				m_ContentSizerBase->Show(m_IconSizer);
			}
		}
		void SetAffirmativeID(int id)
		{
			KxDialog::SetAffirmativeId(id);
		}
		void SetEscapeID(int id)
		{
			KxDialog::SetEscapeId(id);
		}
		wxString GetDefaultTitle()
		{
			return wxTheApp->GetAppDisplayName();
		}
		bool IsUserWindowHasFocus() const
		{
			for (const auto& window: m_UserControls)
			{
				if (window->HasFocus())
				{
					return true;
				}
			}
			return false;
		}
		
		bool MSWTranslateMessage(WXMSG* msg) override;
		void EnableGlassFrame();
		HWND GetGripperWindow()
		{
			return FindWindowExW(this->GetHandle(), nullptr, L"ScrollBar", L"");
		}
		void OnDrawFrameBorder(wxPaintEvent& event);

	public:
		static const int DefaultStyle = KxDialog::DefaultStyle;

		KxStdDialog()
		{
		}
		KxStdDialog(wxWindow* parent,
					 wxWindowID id,
					 const wxString& caption,
					 const wxPoint& pos = wxDefaultPosition,
					 const wxSize& size = wxDefaultSize,
					StdButton buttons = DefaultButtons,
					 long style = DefaultStyle
		)
		{
			Create(parent, id, caption, pos, size, buttons, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& caption,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					StdButton buttons = DefaultButtons,
					long style = DefaultStyle
		);
		~KxStdDialog();

	public:
		virtual void SetAffirmativeId(int id)
		{

		}
		virtual void SetEscapeId(int id)
		{

		}

		wxWindow* GetContentWindow() const override
		{
			return m_ContentPanel;
		}
		virtual wxSizer* GetContentWindowSizer()
		{
			return GetDialogMainCtrl()->GetContainingSizer();
		}
		wxSizer* GetContentWindowMainSizer()
		{
			return m_ContentSizer;
		}
		virtual wxWindow* GetDialogMainCtrl() const = 0;
		virtual wxWindow* GetDialogFocusCtrl() const
		{
			return GetDialogMainCtrl();
		}

		virtual int ShowModal() override;
		void AdjustWindow(const wxPoint &pos = wxPoint(-2, -2), const wxSize& minSize = wxDefaultSize);
		void AddUserWindow(wxWindow* window)
		{
			m_UserControls.push_back(window);
		}

		// Layout
		virtual wxOrientation GetWindowResizeSide() const
		{
			return m_DialogResizeSide;
		}
		void SetWindowResizeSide(wxOrientation value)
		{
			m_DialogResizeSide = value;
			SetResizingBehavior();
		}
		bool IsAutoSizeEnabled() const
		{
			return m_IsAutoSizeEnabled;
		}
		void SetAutoSize(bool value)
		{
			m_IsAutoSizeEnabled = value;
		}

		// Misc UI functions
		void SetGlassFrameEnabled(bool value)
		{
			m_IsGlassFrameEnabled = value;
			EnableGlassFrame();
		}
		void SetGripperVisible(bool value)
		{
			if (value)
			{
				m_GripperWindow.SetSize(m_GripperWindowSize);
				m_GripperWindow.Show();

				wxSize size = GetSize();
				SetSize(size.x + 1, size.y + 1);
				SetSize(size);
				Refresh();
			}
			else
			{
				m_GripperWindow.SetSize(wxSize(0, 0));
				m_GripperWindow.Hide();
			}
		}

		// Icons
		wxBitmap GetMainIcon() const override
		{
			return m_MainIcon;
		}
		StdIcon GetMainIconID() const override
		{
			return m_MainIconID;
		}
		void SetMainIcon(const wxBitmap& icon) override
		{
			if (icon.IsOk())
			{
				m_MainIcon = icon;
				m_MainIconID = StdIcon::None;
				LoadIcon();
			}
			else
			{
				m_MainIconID = StdIcon::None;
			}
			SetIconVisibility();
		}
		void SetMainIcon(StdIcon iconID = DefaultIconID) override
		{
			m_MainIconID = iconID;
			if (iconID != StdIcon::None)
			{
				m_MainIcon = wxArtProvider::GetMessageBoxIcon(KxFramework::UI::ToWxStdIcon(iconID));
				LoadIcon();
			}
			SetIconVisibility();
		}
		
		// Caption
		wxString GetCaption() const override
		{
			return m_CaptionLabel->GetLabel();
		}
		void SetCaption(const wxString& label) override
		{
			m_CaptionLabel->SetLabel(label);
		}
		
		// Small label above main control
		wxString GetLabel() const override
		{
			return m_ViewLabel->GetLabel();
		}
		void SetLabel(const wxString& label) override;
		
		// Buttons customization
		void SetDefaultButton(wxWindowID id) override;
		KxStdDialogControl GetButton(wxWindowID id) const override;
		KxStdDialogControl AddButton(wxWindowID id, const wxString& label = wxEmptyString, bool prepend = false) override;

		// Keyboard control
		StdButtonsIDs GetCloseIDs() const
		{
			return m_CloseIDs;
		}
		void SetCloseIDs(const StdButtonsIDs& list)
		{
			m_CloseIDs = list;

			int style = GetWindowStyle();
			if (IsEscapeAllowed())
			{
				style |= wxCLOSE_BOX;
			}
			else
			{
				style &= ~wxCLOSE_BOX;
			}
			SetWindowStyle(style);
		}
		void SetDefaultCloseIDs()
		{
			m_CloseIDs = ms_DefaultCloseIDs;
			SetWindowStyle(GetWindowStyle()|wxCLOSE_BOX);
		}
		StdButtonsIDs GetEnterIDs() const
		{
			return m_EnterIDs;
		}
		void SetEnterIDs(const StdButtonsIDs& list)
		{
			m_EnterIDs = list;
		}
		void SetDefaultEnterDs()
		{
			m_EnterIDs = ms_DefaultEnterIDs;
		}

	public:
		wxDECLARE_ABSTRACT_CLASS(KxStdDialog);
};
