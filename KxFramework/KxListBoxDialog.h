#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxListBox.h"

class KX_API KxListBoxDialog: public KxStdDialog
{
	private:
		KxListBox* m_View = nullptr;

		virtual wxOrientation GetViewLabelSizerOrientation() const;
		virtual bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const;

	public:
		static const int DefaultStyle = KxStdDialog::DefaultStyle;

		KxListBoxDialog() = default;
		KxListBoxDialog(wxWindow* parent,
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
		KxListBox* GetListBox() const
		{
			return m_View;
		}
		KxListBox* GetDialogMainCtrl() const override
		{
			return GetListBox();
		}
		int GetViewSizerProportion() const override
		{
			return 1;
		}

		bool IsMultiSelect() const
		{
			return GetListBox()->IsMultiSelect();
		}
		void SetMultiSelect(bool value)
		{
			GetListBox()->SetMultiSelect(value);
		}
		
		bool IsCheckList() const
		{
			return GetListBox()->IsCheckList();
		}
		void SetCheckList(bool value)
		{
			GetListBox()->SetCheckList(value);
		}
		
		int GetSelection() const
		{
			return GetListBox()->GetSelection();
		}
		KxIntVector GetSelections() const
		{
			return GetListBox()->GetSelections();
		}
		void SelectItem(int index, bool select)
		{
			GetListBox()->Select(index, select);
		}
		
		bool GetItemChecked(int index) const
		{
			return GetListBox()->IsItemChecked(index);
		}
		void SetItemChecked(int index, bool check)
		{
			GetListBox()->CheckItem(index, check);
		}
		KxIntVector GetCheckedItems() const
		{
			return GetListBox()->GetCheckedItems();
		}
		
		wxString GetValue() const
		{
			if (!IsMultiSelect())
			{
				return GetListBox()->GetItemText(GetSelection(), 0);
			}
			return {};
		}
		void SetItemLabel(int index, const wxString& label)
		{
			GetListBox()->SetItemLabel(index, label);
		}
		
		void ClearItems()
		{
			GetListBox()->ClearItems();
		}
		int InsertItem(int index, const wxString& label)
		{
			return GetListBox()->InsertItem(label, index, -1);
		}
		int AddItem(const wxString& label)
		{
			return InsertItem(GetListBox()->GetItemCount(), label);
		}
		void AddItems(const KxStringVector& list)
		{
			for (const wxString& s: list)
			{
				GetListBox()->AddItem(s, -1);
			}
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxListBoxDialog);
};
