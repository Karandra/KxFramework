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
			ImageListValue(const GDIImageList& imageList)
			{
				WithImageList::SetImageList(&imageList);
			}
			ImageListValue(GDIImageList* imageList)
			{
				WithImageList::AssignImageList(imageList);
			}

		public:
			bool FromAny(Any value);
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
				if (const kxf::GDIImageList* imageList = WithImageList::GetImageList())
				{
					return imageList->GetImageCount();
				}
				return 0;
			}
			GDIBitmap GetBitmap(size_t index) const
			{
				const kxf::GDIImageList* imageList = WithImageList::GetImageList();
				if (imageList && index < (size_t)imageList->GetImageCount())
				{
					return imageList->GetBitmap(index).ToWxBitmap();
				}
				return wxNullBitmap;
			}
			void AddBitmap(const GDIBitmap& bitmap)
			{
				if (kxf::GDIImageList* imageList = WithImageList::GetImageList())
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
			bool SetDisplayValue(Any value) override;

			size_t GetBitmapCount() const override
			{
				return m_Value.GetBitmapsCount();
			}
			GDIBitmap GetBitmap(size_t index) const override
			{
				return m_Value.GetBitmap(index);
			}

		public:
			ImageListRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:BitmapListRendererBase(m_Value, m_Value, alignment)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(std::move(value)).GetText();
			}
	};
}
