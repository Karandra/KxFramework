#pragma once
#include "Kx/UI/Common.h"
#include "Dialog.h"
#include "Kx/RTTI/QueryInterface.h"
#include "Kx/UI/Controls/Label.h"
#include "Kx/UI/Controls/Button.h"
#include "Kx/UI/Windows/Panel.h"
#include "Kx/UI/StdIcon.h"
#include "Kx/UI/StdButton.h"
#include <wx/nativewin.h>
#include <wx/artprov.h>
#include <wx/statbmp.h>
#include <wx/app.h>

namespace KxFramework::UI
{
	class KX_API StdDialogControl final
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
			StdDialogControl(wxWindowID id = wxID_NONE)
				:m_Type(Type::ID)
			{
				m_Data.ID = id;
			}
			StdDialogControl(wxWindow* control)
				:m_Type(Type::Window)
			{
				m_Data.Control = control;
			}
			StdDialogControl(Button* control);

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
			operator Button*() const;
	};
}

namespace KxFramework::UI
{
	class KX_API IStdDialog: public RTTI::Interface<IStdDialog>
	{
		KxDecalreIID(IStdDialog, {0x432d41e4, 0x2a7d, 0x44a0, {0x8f, 0x26, 0x49, 0x64, 0xdd, 0x31, 0x7a, 0x9f}});

		public:
			static constexpr StdIcon DefaultIconID = StdIcon::Information;
			static constexpr StdButton DefaultButtons = StdButton::OK|StdButton::Cancel;

			KxEVENT_MEMBER(wxNotifyEvent, Button);
			KxEVENT_MEMBER(wxNotifyEvent, Navigate);
			KxEVENT_MEMBER(wxNotifyEvent, Navigating);
			KxEVENT_MEMBER(wxNotifyEvent, Overwrite);
			KxEVENT_MEMBER(wxNotifyEvent, Select);
			KxEVENT_MEMBER(wxNotifyEvent, TypeChanged);

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
			virtual StdDialogControl GetButton(wxWindowID id) const = 0;
			virtual StdDialogControl AddButton(wxWindowID id, const String& label = {}, bool prepend = false) = 0;
	};
}

namespace KxFramework::UI
{
	// This is probably the most broken class of them all...
	class KX_API StdDialog: public Dialog, public IStdDialog
	{
		public:
			using StdButtonsIDs = std::vector<wxWindowID>;

			static constexpr DialogStyle DefaultStyle = Dialog::DefaultStyle;
			static const StdButtonsIDs ms_DefaultCloseIDs;
			static const StdButtonsIDs ms_DefaultEnterIDs;
			static const Color ms_WindowBackgroundColor;
			static const Color ms_LineBackgroundColor;

		private:
			static inline bool ms_IsBellAllowed = false;

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
			Label* m_CaptionLabel = nullptr;
			Label* m_ViewLabel = nullptr;
			Panel* m_ContentPanel = nullptr;
			wxStaticBitmap* m_IconView = nullptr;
			wxBitmap m_MainIcon = wxNullBitmap;
			StdIcon m_MainIconID = DefaultIconID;
			std::vector<wxWindow*> m_UserControls;
			bool m_IsAutoSizeEnabled = true;

		private:
			Panel* m_ContentPanelLine = nullptr;
			wxSizer* m_DialogSizer = nullptr;
			wxSizer* m_ContentSizerBase = nullptr;
			wxSizer* m_ContentSizer = nullptr;
			wxSizer* m_IconSizer = nullptr;
			StdDialogButtonSizer* m_ButtonsSizer = nullptr;
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
				Dialog::SetAffirmativeId(id);
			}
			void SetEscapeID(int id)
			{
				Dialog::SetEscapeId(id);
			}
			String GetDefaultTitle()
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
			void* GetGripperWindow();
			void OnDrawFrameBorder(wxPaintEvent& event);

		public:
			StdDialog() = default;
			StdDialog(wxWindow* parent,
					  wxWindowID id,
					  const String& caption,
					  const wxPoint& pos = wxDefaultPosition,
					  const wxSize& size = wxDefaultSize,
					  StdButton buttons = DefaultButtons,
					  DialogStyle style = DefaultStyle
			)
			{
				Create(parent, id, caption, pos, size, buttons, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						StdButton buttons = DefaultButtons,
						DialogStyle style = DefaultStyle
			);

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
					m_MainIcon = wxArtProvider::GetMessageBoxIcon(ToWxStdIcon(iconID));
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
			StdDialogControl GetButton(wxWindowID id) const override;
			StdDialogControl AddButton(wxWindowID id, const String& label = {}, bool prepend = false) override;

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
			wxDECLARE_ABSTRACT_CLASS(StdDialog);
	};
}
