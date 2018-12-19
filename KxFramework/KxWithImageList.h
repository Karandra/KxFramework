/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxImageList.h"

class KX_API KxWithImageList
{
	public:
		enum
		{
			NO_IMAGE = -1
		};

	private:
		KxImageList* m_ImageList = nullptr;
		bool m_IsOwned = false;

	private:
		void DeleteIfNeeded();

	public:
		KxWithImageList();
		virtual ~KxWithImageList();

	public:
		bool HasImageList() const
		{
			return m_ImageList != nullptr;
		}
		bool OwnsImageList() const
		{
			return m_IsOwned;
		}

		KxImageList* GetImageList()
		{
			return m_ImageList;
		}
		const KxImageList* GetImageList() const
		{
			return m_ImageList;
		}
		
		virtual void SetImageList(const KxImageList* imageList);
		void AssignImageList(KxImageList* imageList);
};

//////////////////////////////////////////////////////////////////////////
// Derive from this class to use 'KxWithImageList' functions on class that derived from 'wxWithImages'
// Like this: class KxNotebook: public KxWithImageListWrapper<wxNotebook> { ... }
template<class BaseT> class KxWithImageListWrapper: public BaseT
{
	private:
		bool m_IsOwned = false;
		bool m_IsKxList = false;

	private:
		const BaseT* GetThis() const
		{
			return static_cast<const BaseT*>(this);
		}
		BaseT* GetThis()
		{
			return static_cast<BaseT*>(this);
		}

		bool IsKxList(const wxImageList* imageList) const
		{
			return dynamic_cast<const KxImageList*>(imageList) != nullptr;
		}

		wxIcon GetImage(int index) const = delete;

	public:
		bool HasImageList() const
		{
			return GetImageList() != nullptr;
		}
		bool OwnsImageList() const
		{
			return m_IsOwned;
		}

		const wxImageList* GetWxImageList() const
		{
			return GetThis()->wxWithImages::GetImageList();
		}
		wxImageList* GetWxImageList()
		{
			return GetThis()->wxWithImages::GetImageList();
		}

		KxImageList* GetImageList()
		{
			return m_IsKxList ? static_cast<KxImageList*>(GetWxImageList()) : nullptr;
		}
		const KxImageList* GetImageList() const
		{
			return m_IsKxList ? static_cast<const KxImageList*>(GetWxImageList()) : nullptr;
		}

		void SetImageList(const KxImageList* imageList)
		{
			GetThis()->wxWithImages::SetImageList(const_cast<KxImageList*>(imageList));
			m_IsKxList = true;
			m_IsOwned = false;
		}
		void SetImageList(wxImageList* imageList)
		{
			GetThis()->wxWithImages::SetImageList(imageList);
			m_IsKxList = IsKxList(imageList);
			m_IsOwned = false;
		}

		void AssignImageList(KxImageList* imageList)
		{
			GetThis()->wxWithImages::AssignImageList(imageList);
			m_IsKxList = true;
			m_IsOwned = true;
		}
		void AssignImageList(wxImageList* imageList)
		{
			GetThis()->wxWithImages::AssignImageList(imageList);
			m_IsKxList = IsKxList(imageList);
			m_IsOwned = false;
		}
};
