#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/KxWithImageList.h"

template<size_t ImagesCount>
class KxDataViewImageListRendererValue
{
	public:
		using ImageIndexT = int;
		using ArrayT = std::array<ImageIndexT, ImagesCount>;
		using IListT = std::initializer_list<ImageIndexT>;

		static void ClearArray(ArrayT& array)
		{
			array.fill(KxWithImageList::NO_IMAGE);
		}

	private:
		ArrayT m_Value;

	public:
		KxDataViewImageListRendererValue()
		{
			Clear();
		}
		KxDataViewImageListRendererValue(const ArrayT& values)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				m_Value[i] = values[i];
			}
		}
		KxDataViewImageListRendererValue(const KxIntVector& values)
		{
			Clear();
			for (size_t i = 0; i < std::min(values.size(), m_Value.size()); i++)
			{
				m_Value[i] = values[i];
			}
		}
		KxDataViewImageListRendererValue(IListT list)
		{
			Clear();

			size_t index = 0;
			for (const ImageIndexT& id: list)
			{
				if (index < list.size())
				{
					m_Value[index] = id;
					index++;
				}
				else
				{
					break;
				}
			}
		}

	public:
		void Clear()
		{
			ClearArray(m_Value);
		}
		
		const ArrayT& GetList() const
		{
			return m_Value;
		}
		ArrayT& GetList()
		{
			return m_Value;
		}
};

class KX_API KxDataViewImageListRendererBase: public KxDataViewRenderer, public KxWithImageList
{
	private:
		int m_Spacing = 0;

	protected:
		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	protected:
		virtual size_t GetImageCount() const = 0;
		virtual int GetImageID(size_t index) const = 0;

		size_t GetActualImageCount() const;
		size_t GetEffectiveImageCount() const;

	public:
		KxDataViewImageListRendererBase(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}

	public:
		int GetSpacing() const
		{
			return m_Spacing;
		}
		void SetSpacing(int value);
};

//////////////////////////////////////////////////////////////////////////
template<size_t ImagesCount>
class KxDataViewImageListRenderer: public KxDataViewImageListRendererBase
{
	public:
		using ValueT = KxDataViewImageListRendererValue<ImagesCount>;
		using ArrayT = typename ValueT::ArrayT;
		using ImageIndexT = typename ValueT::ImageIndexT;

	private:
		ValueT m_Value;

	protected:
		virtual size_t GetImageCount() const override
		{
			return ImagesCount;
		}
		virtual int GetImageID(size_t index) const override
		{
			return m_Value.GetList()[index];
		}

		virtual bool SetValue(const wxAny& value)
		{
			m_Value.Clear();

			if (value.GetAs<ValueT>(&m_Value))
			{
				return true;
			}

			ImageIndexT index = KxWithImageList::NO_IMAGE;
			if (value.GetAs<ImageIndexT>(&index))
			{
				m_Value.GetList()[0] = index;
				return true;
			}

			return false;
		}

	public:
		KxDataViewImageListRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewImageListRendererBase(cellMode, alignment)
		{
		}
};
