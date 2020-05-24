#pragma once
#include "../Renderer.h"
#include "kxf/Drawing/WithImageList.h"
#include "BitmapListRenderer.h"

namespace kxf::UI::DataView
{
	class KX_API ImageListValue: public TextValue, public BitmapValueBase, public kxf::WithImageList
	{
		public:
			ImageListValue() = default;
			ImageListValue(const String& text)
				:TextValue(text)
			{
			}
			ImageListValue(const kxf::ImageList& imageList)
			{
				WithImageList::SetImageList(&imageList);
			}
			ImageListValue(kxf::ImageList* imageList)
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
				if (const kxf::ImageList* imageList = WithImageList::GetImageList())
				{
					return imageList->GetImageCount();
				}
				return 0;
			}
			wxBitmap GetBitmap(size_t index) const
			{
				const kxf::ImageList* imageList = WithImageList::GetImageList();
				if (imageList && index < (size_t)imageList->GetImageCount())
				{
					return imageList->GetBitmap(index);
				}
				return wxNullBitmap;
			}
			void AddBitmap(const wxBitmap& bitmap)
			{
				if (kxf::ImageList* imageList = WithImageList::GetImageList())
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

namespace kxf::UI::DataView
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
			ImageListRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:BitmapListRendererBase(m_Value, m_Value, alignment)
			{
			}
			
		public:
			String GetTextValue(const wxAny& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
