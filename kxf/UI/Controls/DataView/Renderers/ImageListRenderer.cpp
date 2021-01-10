#include "stdafx.h"
#include "ImageListRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"

namespace kxf::UI::DataView
{
	bool ImageListValue::FromAny(const Any& value)
	{
		if (TextValue::FromAny(value) || value.GetAs(*this))
		{
			return true;
		}
		else if (const kxf::GDIImageList* imageList = nullptr; value.GetAs(imageList))
		{
			WithImageList::SetImageList(imageList);
			return true;
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	bool ImageListRenderer::SetValue(const Any& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}
}
