#pragma once
#include "Common.h"
#include "GDIRenderer/GDIImageList.h"

namespace kxf
{
	class KX_API WithImageList
	{
		private:
			GDIImageList* m_ImageList = nullptr;
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

			GDIImageList* GetImageList() noexcept
			{
				return m_ImageList;
			}
			const GDIImageList* GetImageList() const noexcept
			{
				return m_ImageList;
			}

			virtual void SetImageList(const GDIImageList* imageList)
			{
				DeleteIfNeeded();
				m_IsOwned = false;
				m_ImageList = const_cast<GDIImageList*>(imageList);
			}
			virtual void AssignImageList(GDIImageList* imageList)
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

			static const GDIImageList* ToKxList(const wxImageList& imageList) noexcept
			{
				if (imageList.IsKindOf(wxCLASSINFO(GDIImageList)))
				{
					return static_cast<const GDIImageList*>(&imageList);
				}
				return nullptr;
			}
			static GDIImageList* ToKxList(wxImageList& imageList)
			{
				if (imageList.IsKindOf(wxCLASSINFO(GDIImageList)))
				{
					return static_cast<GDIImageList*>(&imageList);
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

			GDIImageList* GetImageList() noexcept
			{
				if (wxImageList* imageList = GetWxImageList())
				{
					if (GDIImageList* kxImageList = ToKxList(*imageList))
					{
						return kxImageList;
					}
				}
				return nullptr;
			}
			const GDIImageList* GetImageList() const noexcept
			{
				if (const wxImageList* imageList = GetWxImageList())
				{
					if (const GDIImageList* kxImageList = ToKxList(*imageList))
					{
						return kxImageList;
					}
				}
				return nullptr;
			}

			void SetImageList(const GDIImageList* imageList)
			{
				GetThis().wxWithImages::SetImageList(const_cast<GDIImageList*>(imageList));
				m_IsOwned = false;
			}
			void SetImageList(wxImageList* imageList)
			{
				GetThis().wxWithImages::SetImageList(imageList);
				m_IsOwned = false;
			}

			void AssignImageList(GDIImageList* imageList)
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
