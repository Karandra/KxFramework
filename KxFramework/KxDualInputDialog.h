#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxTextBox.h"

class KxDualInputDialog: public KxStdDialog
{
	private:
		KxPanel* m_View = NULL;
		KxTextBox* m_Input1 = NULL;
		KxTextBox* m_Input2 = NULL;

	private:
		virtual wxOrientation GetWindowResizeSide() const
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
		virtual wxWindow* GetDialogMainCtrl() const override
		{
			return m_View;
		}
		virtual int GetViewSizerProportion() const
		{
			return 1;
		}
		virtual void ResetState();

		wxString GetValue1() const
		{
			return m_Input1->GetValue();
		}
		wxString GetValue2() const
		{
			return m_Input2->GetValue();
		}
		void SetValue(const wxString& value1, const wxString& value2)
		{
			m_Input1->SetValue(value1);
			m_Input2->SetValue(value2);
		}
		bool IsEditable1() const
		{
			return m_Input1->IsEditable();
		}
		bool IsEditable2() const
		{
			return m_Input2->IsEditable();
		}
		void SetEditable(bool value1, bool value2)
		{
			m_Input1->SetEditable(value1);
			m_Input2->SetEditable(value2);
		}
		bool SetHint(const wxString& value1, const wxString& value2)
		{
			bool b1 = m_Input1->SetHint(value1);
			bool b2 = m_Input2->SetHint(value2);
			return b1 && b2;
		}
		void SetInputFocus(int index);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDualInputDialog);
};
