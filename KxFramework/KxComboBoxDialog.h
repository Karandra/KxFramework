#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"

enum
{
	KxCBD_BITMAP = 1 << 0,
	KxCBD_READONLY = 1 << 1
};
class KX_API KxComboBoxDialog: public KxStdDialog
{
	private:
		wxComboBox* m_View = NULL;

	private:
		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = NULL) const;

	public:
		static const long DefaultStyle = KxStdDialog::DefaultStyle|KxCBD_READONLY;
		static const int DefaultComboBoxWidth = 300;

		KxComboBoxDialog() {}
		KxComboBoxDialog(wxWindow* parent,
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
		virtual ~KxComboBoxDialog();

	public:
		wxComboBox* GetComboBox() const
		{
			return m_View;
		}
		virtual wxWindow* GetDialogMainCtrl() const override
		{
			return GetComboBox();
		}
		virtual int GetViewSizerProportion() const
		{
			return 0;
		}
		
		int GetSelection()
		{
			if (GetComboBox())
			{
				return GetComboBox()->GetSelection();
			}
			return -1;
		}
		void SetSelection(int index)
		{
			if (GetComboBox())
			{
				GetComboBox()->SetSelection(index);
			}
		}
		
		wxString GetValue()
		{
			if (GetComboBox())
			{
				return GetComboBox()->GetValue();
			}
			return wxEmptyString;
		}
		void SetValue(const wxString& string)
		{
			if (GetComboBox())
			{
				GetComboBox()->SetValue(string);
			}
		}
		
		void SetItemLabel(int index, const wxString& label)
		{
			if (GetComboBox())
			{
				GetComboBox()->SetString(index, label);
			}
		}
		
		int InsertItem(int index, const wxString& label)
		{
			if (GetComboBox())
			{
				int nItemIndex = GetComboBox()->Insert(label, index);
				SetSelection(0);
				return nItemIndex;
			}
			return -1;
		}
		int AddItem(const wxString& label)
		{
			if (GetComboBox())
			{
				return InsertItem(GetComboBox()->GetCount(), label);
			}
			return -1;
		}
		void SetItems(const KxStringVector& list)
		{
			if (GetComboBox())
			{
				GetComboBox()->Clear();
				GetComboBox()->Append(list);
				SetSelection(0);
			}
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxComboBoxDialog);
};
