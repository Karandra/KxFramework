#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include <wx/nativewin.h>

enum class KxToolTipExOption
{
	None = 0,
	Ballon = 1 << 0,
	CloseButton = 1 << 1,
	LargeIcons = 1 << 2,
	AutoHide = 1 << 3,
};

class KX_API KxToolTipEx: public wxNativeWindow, public KxWithOptions<KxToolTipExOption, KxToolTipExOption::None>
{
	private:
		wxString m_Caption;
		wxString m_Message;
		std::variant<wxIcon, KxIconType> m_Icon = KxIconType::KxICON_NONE;
		wxIcon m_QuestionIcon;
		int m_MaxWidth = -1;

	private:
		bool IsBalloonStyleSupported() const;
		bool CreateWindow(wxWindow* parent);
		bool CreateToolTip();
		void UpdateCaption();
		int ConvertIconID(KxIconType icon) const;
		void DoRedraw();

	protected:
		bool MSWHandleMessage(WXLRESULT* result, WXUINT message, WXWPARAM wParam, WXLPARAM lParam) override;
		bool MSWHandleNotify(WXLRESULT* result, WXUINT message, WXWPARAM wParam, WXLPARAM lParam, const NMHDR& notificationInfo);

	public:
		KxToolTipEx() = default;
		KxToolTipEx(wxWindow* parent)
		{
			Create(parent);
		}
		bool Create(wxWindow* parent)
		{
			return CreateWindow(parent);
		}

	public:
		bool Popup(const wxPoint& position, wxWindow* window = nullptr);
		void Dismiss();
		void Disown() = delete;

		wxString GetCaption() const
		{
			return m_Caption;
		}
		void SetCaption(const wxString& value)
		{
			m_Caption = value;
		}

		wxString GetMessage() const
		{
			return m_Message;
		}
		void SetMessage(const wxString& value)
		{
			m_Message = value;
		}

		KxIconType GetIconID() const;
		wxIcon GetIcon() const;
		void SetIcon(const wxIcon& icon);
		void SetIcon(KxIconType iconID);

		int GetMaxWidth() const
		{
			return m_MaxWidth;
		}
		void SetMaxWidth(int maxWidth)
		{
			m_MaxWidth = maxWidth;
		}
};
