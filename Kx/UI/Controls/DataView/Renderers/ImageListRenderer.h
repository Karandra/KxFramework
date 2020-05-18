#pragma once
#include "../Renderer.h"
#include "Kx/Drawing/WithImageList.h"
#include "BitmapListRenderer.h"

namespace KxFramework::UI::DataView
{
	class KX_API ImageListValue: public TextValue, public BitmapValueBase, public KxFramework::WithImageList
	{
		public:
			ImageListValue() = default;
			ImageListValue(const wxString& text)
				:TextValue(text)
			{
			}
			ImageListValue(const KxFramework::ImageList& imageList)
			{
				WithImageList::SetImageList(&imageList);
			}
			ImageListValue(KxFramework::ImageList* imageList)
			{
				WithImageList::AssignImageList(imageList);
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
				if (const KxFramework::ImageList* imageList = WithImageList::GetImageList())
				{
					return imageList->GetImageCount();
				}
				return 0;
			}
			wxBitmap GetBitmap(size_t index) const
			{
				const KxFramework::ImageList* imageList = WithImageList::GetImageList();
				if (imageList && index < (size_t)imageList->GetImageCount())
				{
					return imageList->GetBitmap(index);
				}
				return wxNullBitmap;
			}
			void AddBitmap(const wxBitmap& bitmap)
			{
				if (KxFramework::ImageList* imageList = WithImageList::GetImageList())
				{
					imageList->Add(bitmap);
				}
			}
			void ClearBitmaps()
			{
				WithImageList::SetImageList(nullptr);
			}
	};
}

namespace KxFramework::UI::DataView
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
			
		public:
			wxString GetTextValue(const wxAny& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
