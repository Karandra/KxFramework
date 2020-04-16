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
		wxComboBox* m_View = nullptr;

	private:
		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const;

	public:
		static const long DefaultStyle = KxStdDialog::DefaultStyle|KxCBD_READONLY;
		static const int DefaultComboBoxWidth = 300;

		KxComboBoxDialog() = default;
		KxComboBoxDialog(wxWindow* parent,
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

	public:
		wxComboBox* GetComboBox() const
		{
			return m_View;
		}
		wxWindow* GetDialogMainCtrl() const override
		{
			return GetComboBox();
		}
		int GetViewSizerProportion() const override
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
			return {};
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
		void SetItems(const std::vector<wxString>& list)
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
