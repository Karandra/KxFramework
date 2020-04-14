#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxTextBox.h"
#include "KxFramework/KxStyledTextBox.h"

enum KxTBD_Options
{
	KxTBD_NONE = 0,

	KxTBD_MULTILINE = 1 << 0,
	KxTBD_PASSWORD = 1 << 1,
	KxTBD_READONLY = 1 << 2,
	KxTBD_STYLED = 1 << 3,

	KxTBD_MASK = KxTBD_MULTILINE|KxTBD_PASSWORD|KxTBD_READONLY|KxTBD_STYLED
};

class KX_API KxTextBoxDialog: public KxStdDialog
{
	private:
		wxControl* m_View = nullptr;
		KxTBD_Options m_Options = KxTBD_NONE;

	private:
		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual wxOrientation GetWindowResizeSide() const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const;

		bool IsStyledTextBox() const
		{
			return m_Options & KxTBD_STYLED;
		}

	public:
		static const long DefaultStyle = KxStdDialog::DefaultStyle;
		static const int DefaultWidth = 300;
		static const int DefaultMLWidth = 450;
		static const int DefaultMLHeight = 200;

		KxTextBoxDialog() = default;
		KxTextBoxDialog(wxWindow* parent,
						wxWindowID id,
						const wxString& caption,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						int buttons = DefaultButtons,
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
					int buttons = DefaultButtons,
					long style = DefaultStyle
		);

	public:
		wxWindow* GetDialogMainCtrl() const override
		{
			return m_View;
		}
		int GetViewSizerProportion() const override
		{
			return IsMultiLine() ? 1 : 0;
		}
		
		KxTextBox* GetTextBox() const
		{
			if (!IsStyledTextBox())
			{
				return static_cast<KxTextBox*>(m_View);
			}
			return nullptr;
		}
		KxStyledTextBox* GetStyledTextBox() const
		{
			if (IsStyledTextBox())
			{
				return static_cast<KxStyledTextBox*>(m_View);
			}
			return nullptr;
		}

		wxString GetValue() const
		{
			if (GetTextBox())
			{
				return GetTextBox()->GetValue();
			}
			else if (GetStyledTextBox())
			{
				return GetStyledTextBox()->GetValue();
			}
			return wxEmptyString;
		}
		void SetValue(const wxString& value)
		{
			if (GetTextBox())
			{
				GetTextBox()->SetValue(value);
			}
			else if (GetStyledTextBox())
			{
				return GetStyledTextBox()->SetValue(value);
			}
		}
		
		bool IsMultiLine() const
		{
			if (GetTextBox())
			{
				return GetTextBox()->IsMultiLine();
			}
			else if (GetStyledTextBox())
			{
				return GetStyledTextBox()->IsMultiLine();
			}
			return false;
		}
		void SetMultiLine(bool value)
		{
			if (GetStyledTextBox())
			{
				return GetStyledTextBox()->SetMultiLine(value);
			}
		}
		
		bool IsEditable() const
		{
			if (GetTextBox())
			{
				return GetTextBox()->IsEditable();
			}
			else if (GetStyledTextBox())
			{
				return GetStyledTextBox()->IsEditable();
			}
			return false;
		}
		void SetEditable(bool value)
		{
			if (GetTextBox())
			{
				GetTextBox()->SetEditable(value);
			}
			else if (GetStyledTextBox())
			{
				return GetStyledTextBox()->SetEditable(value);
			}
		}
		
		bool SetHint(const wxString& value)
		{
			if (GetTextBox())
			{
				return GetTextBox()->SetHint(value);
			}
			else if (GetStyledTextBox())
			{
				return GetStyledTextBox()->SetHint(value);
			}
			return false;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxTextBoxDialog);
};
