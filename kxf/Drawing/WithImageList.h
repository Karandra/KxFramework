#pragma once
#include "Common.h"
#include "ImageList.h"

namespace kxf
{
	class KX_API WithImageList
	{
		private:
			ImageList* m_ImageList = nullptr;
			bool m_IsOwned = false;

		private:
			void DeleteIfNeeded() noexcept
			{
				if (m_IsOwned)
				{
					delete m_ImageList;
					m_IsOwned = false;
				}
				m_ImageList = nullptr;
			}

		public:
			WithImageList() noexcept = default;
			virtual ~WithImageList() noexcept
			{
				DeleteIfNeeded();
			}

		public:
			bool HasImageList() const noexcept
			{
				return m_ImageList != nullptr;
			}
			bool OwnsImageList() const noexcept
			{
				return m_IsOwned;
			}

			ImageList* GetImageList() noexcept
			{
				return m_ImageList;
			}
			const ImageList* GetImageList() const noexcept
			{
				return m_ImageList;
			}
			
			virtual void SetImageList(const ImageList* imageList)
			{
				DeleteIfNeeded();
				m_IsOwned = false;
				m_ImageList = const_cast<ImageList*>(imageList);
			}
			virtual void AssignImageList(ImageList* imageList)
			{
				DeleteIfNeeded();
				m_IsOwned = true;
				m_ImageList = imageList;
			}
	};
}

// Derive from this class to use 'WithImageList' functions on class that derived from 'wxWithImages'.
// Like this: 'class wxNotebook: public WithImageListWrapper<wxNotebook> { ... };'
namespace kxf
{
	template<class TBase>
	class WithImageListWrapper: public TBase
	{
		private:
			bool m_IsOwned = false;

		private:
			const TBase& GetThis() const noexcept
			{
				return static_cast<const TBase&>(*this);
			}
			TBase& GetThis() noexcept
			{
				return static_cast<TBase&>(*this);
			}

			static const ImageList* ToKxList(const wxImageList& imageList) noexcept
			{
				if (imageList.IsKindOf(wxCLASSINFO(ImageList)))
				{
					return static_cast<const ImageList*>(&imageList);
				}
				return nullptr;
			}
			static ImageList* ToKxList(wxImageList& imageList)
			{
				if (imageList.IsKindOf(wxCLASSINFO(ImageList)))
				{
					return static_cast<ImageList*>(&imageList);
				}
				return nullptr;
			}

		public:
			bool HasImageList() const noexcept
			{
				return GetImageList() != nullptr;
			}
			bool OwnsImageList() const noexcept
			{
				return m_IsOwned;
			}
			wxIcon GetImage(int index) const = delete;

			wxImageList* GetWxImageList() noexcept
			{
				return GetThis().wxWithImages::GetImageList();
			}
			const wxImageList* GetWxImageList() const noexcept
			{
				return GetThis().wxWithImages::GetImageList();
			}

			ImageList* GetImageList() noexcept
			{
				if (wxImageList* imageList = GetWxImageList())
				{
					if (ImageList* kxImageList = ToKxList(*imageList))
					{
						return kxImageList;
					}
				}
				return nullptr;
			}
			const ImageList* GetImageList() const noexcept
			{
				if (const wxImageList* imageList = GetWxImageList())
				{
					if (const ImageList* kxImageList = ToKxList(*imageList))
					{
						return kxImageList;
					}
				}
				return nullptr;
			}

			void SetImageList(const ImageList* imageList)
			{
				GetThis().wxWithImages::SetImageList(const_cast<ImageList*>(imageList));
				m_IsOwned = false;
			}
			void SetImageList(wxImageList* imageList)
			{
				GetThis().wxWithImages::SetImageList(imageList);
				m_IsOwned = false;
			}

			void AssignImageList(ImageList* imageList)
			{
				GetThis().wxWithImages::AssignImageList(imageList);
				m_IsOwned = true;
			}
			void AssignImageList(wxImageList* imageList)
			{
				GetThis().wxWithImages::AssignImageList(imageList);
				m_IsOwned = false;
			}
	};
}
