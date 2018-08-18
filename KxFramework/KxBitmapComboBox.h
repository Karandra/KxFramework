#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithImageList.h"

class KxBitmapComboBox: public wxSystemThemedControl<wxBitmapComboBox>, public KxWithImageList
{
	private:
		std::unordered_map<size_t, int> m_ImageIDs;

	private:
		int Insert() = delete;
		int Append() = delete;
		void Delete() = delete;
		void Set() = delete;
		void GetItemBitmap() = delete;
		void SetItemBitmap() = delete;

	public:
		static const long DefaultStyle = wxTE_PROCESS_ENTER|wxCB_DROPDOWN|wxCB_READONLY;

		KxBitmapComboBox() {}
		KxBitmapComboBox(wxWindow* parent,
						 wxWindowID id,
						 const wxString& value = wxEmptyString,
						 long style = DefaultStyle,
						 const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, value, style, validator);
		}
		KxBitmapComboBox(wxWindow* parent,
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
		void SetVisibleItemsCount(size_t count);
		int InsertItem(const wxString& s, size_t i, int imageID = NO_IMAGE);
		int AddItem(const wxString& s, int imageID = NO_IMAGE);
		void RemoveItem(size_t i);
		void Clear();

		int GetItemImage(size_t i) const;
		void SetItemImage(size_t i, int imageID = NO_IMAGE);

		wxDECLARE_DYNAMIC_CLASS(KxBitmapComboBox);
};
