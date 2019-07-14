#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"
#include <KxFramework/KxWithImageList.h>
#include "BitmapListRenderer.h"

namespace KxDataView2
{
	class KX_API ImageListValue: public TextValue, public BitmapValueBase, public KxWithImageList
	{
		public:
			ImageListValue() = default;
			ImageListValue(const wxString& text)
				:TextValue(text)
			{
			}
			ImageListValue(const KxImageList& imageList)
			{
				KxWithImageList::SetImageList(&imageList);
			}
			ImageListValue(KxImageList* imageList)
			{
				KxWithImageList::AssignImageList(imageList);
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				TextValue::Clear();
				BitmapValueBase::Clear();
				ClearBitmaps();
			}

			bool HasBitmaps() const
			{
				return GetBitmapsCount() != 0;
			}
			size_t GetBitmapsCount() const
			{
				if (const KxImageList* imageList = KxWithImageList::GetImageList())
				{
					return imageList->GetImageCount();
				}
				return 0;
			}
			wxBitmap GetBitmap(size_t index) const
			{
				const KxImageList* imageList = KxWithImageList::GetImageList();
				if (imageList && index < (size_t)imageList->GetImageCount())
				{
					return imageList->GetBitmap(index);
				}
				return wxNullBitmap;
			}
			void AddBitmap(const wxBitmap& bitmap)
			{
				if (KxImageList* imageList = KxWithImageList::GetImageList())
				{
					imageList->Add(bitmap);
				}
			}
			void ClearBitmaps()
			{
				KxWithImageList::SetImageList(nullptr);
			}
	};
}

namespace KxDataView2
{
	class KX_API ImageListRenderer: public BitmapListRendererBase
	{
		private:
			ImageListValue m_Value;

		protected:
			bool SetValue(const wxAny& value) override;

			size_t GetBitmapCount() const override
			{
				return m_Value.GetBitmapsCount();
			}
			wxBitmap GetBitmap(size_t index) const override
			{
				return m_Value.GetBitmap(index);
			}

		public:
			ImageListRenderer(int alignment = wxALIGN_INVALID)
				:BitmapListRendererBase(m_Value, m_Value, alignment)
			{
			}
	};
}
