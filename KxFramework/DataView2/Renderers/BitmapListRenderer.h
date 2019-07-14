#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"
#include <KxFramework/KxWithImageList.h>
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace KxDataView2
{
	class KX_API BitmapListValue: public TextValue, public BitmapValueBase
	{
		private:
			std::vector<wxBitmap> m_Bitmaps;

		public:
			BitmapListValue() = default;
			BitmapListValue(const wxString& text)
				:TextValue(text)
			{
			}
			BitmapListValue(const wxBitmap& bitmap)
			{
				AddBitmap(bitmap);
			}
			BitmapListValue(const wxString& text, const wxBitmap& bitmap)
				:TextValue(text)
			{
				AddBitmap(bitmap);
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
				return !m_Bitmaps.empty();
			}
			size_t GetBitmapsCount() const
			{
				return m_Bitmaps.size();
			}
			const wxBitmap& GetBitmap(size_t index) const
			{
				return index < m_Bitmaps.size() ? m_Bitmaps[index] : wxNullBitmap;
			}
			void AddBitmap(const wxBitmap& bitmap)
			{
				m_Bitmaps.push_back(bitmap);
			}
			void ClearBitmaps()
			{
				m_Bitmaps.clear();
			}
	};
}

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
	class KX_API BitmapListRendererBase: public Renderer
	{
		private:
			TextValue& m_TextValue;
			BitmapValueBase& m_BitmapValueBase;

		protected:
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		protected:
			virtual size_t GetBitmapCount() const = 0;
			virtual const wxBitmap& GetBitmap(size_t index) const = 0;

		public:
			BitmapListRendererBase(TextValue& textValue, BitmapValueBase& bitmapValueBase, int alignment = wxALIGN_INVALID)
				:Renderer(alignment), m_TextValue(textValue), m_BitmapValueBase(bitmapValueBase)
			{
			}
	};
}

namespace KxDataView2
{
	class KX_API BitmapListRenderer: public BitmapListRendererBase
	{
		private:
			BitmapListValue m_Value;

		protected:
			bool SetValue(const wxAny& value) override;

			size_t GetBitmapCount() const override
			{
				return m_Value.GetBitmapsCount();
			}
			const wxBitmap& GetBitmap(size_t index) const override
			{
				return m_Value.GetBitmap(index);
			}

		public:
			BitmapListRenderer(int alignment = wxALIGN_INVALID)
				:BitmapListRendererBase(m_Value, m_Value, alignment)
			{
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
			const wxBitmap& GetBitmap(size_t index) const override
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
