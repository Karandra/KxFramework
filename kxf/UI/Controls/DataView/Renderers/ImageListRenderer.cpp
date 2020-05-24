#include "stdafx.h"
#include "ImageListRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"

namespace kxf::UI::DataView
{
	bool ImageListValue::FromAny(const wxAny& value)
	{
		if (TextValue::FromAny(value) || value.GetAs(this))
		{
			return true;
		}
		else if (const kxf::ImageList* imageList = nullptr; value.GetAs(&imageList))
		{
			WithImageList::SetImageList(imageList);
			return true;
		}
		return false;
	}
}

namespace kxf::UI::DataView
{
	bool ImageListRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}
}
