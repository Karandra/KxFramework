#pragma once
#include "KxFramework/KxFramework.h"

class KxComboBox: public wxSystemThemedControl<wxComboBox>
{
	public:
		static const long DefaultStyle = wxTE_PROCESS_ENTER|wxCB_DROPDOWN|wxCB_READONLY;

		KxComboBox() {}
		KxComboBox(wxWindow* parent,
				   wxWindowID id,
				   const wxString& value = wxEmptyString,
				   long style = DefaultStyle,
				   const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, value, style, validator);
		}
		KxComboBox(wxWindow* parent,
				   wxWindowID id,
				   const wxString& value,
				   const wxPoint& pos,
				   const wxSize& size,
				   long style = DefaultStyle,
				   const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, value, pos, size, style, validator);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& value = wxEmptyString,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		)
		{
			return Create(parent, id, value, wxDefaultPosition, wxDefaultSize, style, validator);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& value,
					const wxPoint& pos,
					const wxSize& size,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);

	public:
		int InsertItem(const wxString& s, size_t i)
		{
			return Insert(s, i);
		}
		int AddItem(const wxString& s)
		{
			return Insert(s, GetCount());
		}
		void RemoveItem(size_t i)
		{
			Delete(i);
		}

		void SetVisibleItemsCount(size_t count);

		wxString GetItemLabel(size_t i) const
		{
			return GetString(i);
		}
		void SetItemLabel(size_t i, const wxString& s)
		{
			SetString(i, s);
		}
		
		wxClientData* GetItemData(size_t i) const
		{
			return GetClientObject(i);
		}
		void SetItemData(size_t i, wxClientData* p)
		{
			SetClientObject(i, p);
		}
};
