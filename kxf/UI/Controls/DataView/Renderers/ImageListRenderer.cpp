#include "stdafx.h"
#include "ImageListRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"

namespace kxf::UI::DataView
{
	bool ImageListValue::FromAny(Any value)
	{
		if (TextValue::FromAny(std::move(value)) || std::move(value).GetAs(*this))
		{
			return true;
		}
		else if (const GDIImageList* imageList = nullptr; value.GetAs(imageList))
		{
			WithImageList::SetImageList(imageList);
			return true;
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	bool ImageListRenderer::SetDisplayValue(Any value)
	{
		m_Value.Clear();
		return m_Value.FromAny(std::move(value));
	}
}
