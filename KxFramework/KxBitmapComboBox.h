#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/Drawing/WithImageList.h"

class KX_API KxBitmapComboBox: public wxSystemThemedControl<wxBitmapComboBox>, public KxFramework::WithImageList
{
	public:
		static const long DefaultStyle = wxTE_PROCESS_ENTER|wxCB_DROPDOWN|wxCB_READONLY;

	private:
		std::unordered_map<size_t, int> m_ImageIDs;
		std::optional<wxSize> m_DefaultBitmapSize;

	public:
		KxBitmapComboBox() {}
		KxBitmapComboBox(wxWindow* parent,
						 wxWindowID id,
						 const wxString& value = {},
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
		
	public:
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& value = {},
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
		int InsertItem(const wxString& s, size_t i, int imageID = KxFramework::Drawing::InvalidImageIndex);
		int AddItem(const wxString& s, int imageID = KxFramework::Drawing::InvalidImageIndex);
		void RemoveItem(size_t i);
		void Clear() override;

		int GetItemImage(size_t i) const;
		void SetItemImage(size_t i, int imageID = KxFramework::Drawing::InvalidImageIndex);

		wxSize GetBitmapSize() const override
		{
			if (m_DefaultBitmapSize)
			{
				return *m_DefaultBitmapSize;
			}
			return wxBitmapComboBox::GetBitmapSize();
		}
		void SetDefaultBitmapSize(const wxSize& size)
		{
			m_DefaultBitmapSize = size;
		}
		void SetDefaultBitmapSize()
		{
			m_DefaultBitmapSize = std::nullopt;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxBitmapComboBox);
};
