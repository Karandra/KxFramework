#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxTextBox.h"

class KX_API KxDualInputDialog: public KxStdDialog
{
	private:
		KxPanel* m_View = nullptr;

		KxTextBox* m_TextBox1 = nullptr;
		KxTextBox* m_TextBox2 = nullptr;

	private:
		wxOrientation GetWindowResizeSide() const override
		{
			return wxHORIZONTAL;
		}

	public:
		static const long DefaultStyle = KxStdDialog::DefaultStyle;
		static const int DefaultWidth = 300;

		KxDualInputDialog() {}
		KxDualInputDialog(wxWindow* parent,
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
		virtual ~KxDualInputDialog();

	public:
		wxWindow* GetDialogMainCtrl() const override
		{
			return m_View;
		}
		int GetViewSizerProportion() const override
		{
			return 1;
		}

		KxTextBox* GetTextBox1()
		{
			return m_TextBox1;
		}
		KxTextBox* GetTextBox2()
		{
			return m_TextBox2;
		}

		wxString GetValue1() const
		{
			return m_TextBox1->GetValue();
		}
		wxString GetValue2() const
		{
			return m_TextBox2->GetValue();
		}
		void SetValue1(const wxString& value)
		{
			m_TextBox1->SetValue(value);
		}
		void SetValue2(const wxString& value)
		{
			m_TextBox2->SetValue(value);
		}
		
		bool IsEditable1() const
		{
			return m_TextBox1->IsEditable();
		}
		bool IsEditable2() const
		{
			return m_TextBox2->IsEditable();
		}
		void SetEditable1(bool value)
		{
			m_TextBox1->SetEditable(value);
		}
		void SetEditable2(bool value)
		{
			m_TextBox2->SetEditable(value);
		}

		bool SetHint1(const wxString& value)
		{
			return m_TextBox1->SetHint(value);
		}
		bool SetHint2(const wxString& value)
		{
			return m_TextBox2->SetHint(value);
		}

		void SetFocus1()
		{
			m_TextBox1->SetFocus();
		}
		void SetFocus2()
		{
			m_TextBox1->SetFocus();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDualInputDialog);
};
