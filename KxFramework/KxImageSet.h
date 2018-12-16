#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxImageSet: public wxObject
{
	public:
		enum ImageType
		{
			TYPE_INVALID = -1,

			TYPE_BITMAP,
			TYPE_IMAGE,
			TYPE_ICON,

			TYPE_MAX
		};

	private:
		std::unordered_map<wxString, std::variant<wxBitmap, wxImage, wxIcon>> m_Data;
		
	public:
		KxImageSet(size_t count = 0);
		~KxImageSet();

		void Set(const wxString& id, const wxImage& image);
		void Set(const wxString& id, const wxBitmap& image);
		void Set(const wxString& id, const wxIcon& image);
		const wxObject* Get(const wxString& id, ImageType* type = NULL) const;
		void Remove(const wxString& id);
		void Clear()
		{
			m_Data.clear();
		}

		wxImage GetImage(const wxString& id) const;
		wxBitmap GetBitmap(const wxString& id) const;
		wxIcon GetIcon(const wxString& id) const;
		size_t GetCount() const
		{
			return m_Data.size();
		}
		wxImageList* ToImageList(const wxSize& size) const;
		
		wxDECLARE_DYNAMIC_CLASS(KxImageSet);
};
