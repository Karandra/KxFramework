#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxImageList.h"

enum class KxImageSetImageType
{
	Invalid = -1,

	Bitmap,
	Image,
	Icon,

	MAX
};

class KX_API KxImageSet: public wxObject
{
	public:
		using ImageType = KxImageSetImageType;

	private:
		std::unordered_map<wxString, std::variant<wxBitmap, wxImage, wxIcon>> m_Store;
		
	public:
		KxImageSet(size_t initialCount = 0);
		KxImageSet(const KxImageSet&) = delete;
		KxImageSet(KxImageSet&& other)
			:m_Store(std::move(other.m_Store))
		{
		}
		~KxImageSet() = default;

	public:
		size_t GetCount() const
		{
			return m_Store.size();
		}
		bool Remove(const wxString& id);
		void Clear();

		void Set(const wxString& id, const wxImage& image);
		void Set(const wxString& id, const wxBitmap& image);
		void Set(const wxString& id, const wxIcon& image);

		const wxObject* Get(const wxString& id, ImageType* type = nullptr) const;
		wxSize GetSize(const wxString& id) const;

		wxImage GetImage(const wxString& id) const;
		wxBitmap GetBitmap(const wxString& id) const;
		wxIcon GetIcon(const wxString& id) const;

		const wxImage* GetImagePtr(const wxString& id) const;
		const wxBitmap* GetBitmapPtr(const wxString& id) const;
		const wxIcon* GetIconPtr(const wxString& id) const;

		std::unique_ptr<KxImageList> CreateImageList(const wxSize& size) const;

	public:
		KxImageSet& operator=(const KxImageSet&) = delete;
		KxImageSet& operator=(KxImageSet&& other)
		{
			m_Store = std::move(other.m_Store);
			return *this;
		}
		
	public:
		wxDECLARE_DYNAMIC_CLASS(KxImageSet);
};
