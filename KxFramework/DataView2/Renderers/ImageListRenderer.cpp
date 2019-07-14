#include "KxStdAfx.h"
#include "ImageListRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"

namespace KxDataView2
{
	bool ImageListValue::FromAny(const wxAny& value)
	{
		if (TextValue::FromAny(value) || value.GetAs(this))
		{
			return true;
		}
		else if (const KxImageList* imageList = nullptr; value.GetAs(&imageList))
		{
			KxWithImageList::SetImageList(imageList);
			return true;
		}
		return false;
	}
}

namespace KxDataView2
{
	bool ImageListRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		return m_Value.FromAny(value);
	}
}
