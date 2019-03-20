#include "KxStdAfx.h"
#include "KxFramework/KxGraphicsContext.h"
#include "wx/msw/wrapgdip.h"

namespace
{
	inline Graphics* GetNativeContext(wxGraphicsContext* context)
	{
		return static_cast<Gdiplus::Graphics*>(context->GetNativeContext());
	}

	inline wxRect2DDouble ToRect2DDouble(const Gdiplus::RectF& rect)
	{
		return wxRect2DDouble(rect.X, rect.Y, rect.Width, rect.Height);
	}
	inline wxRect ToRect(const Gdiplus::Rect& rect)
	{
		return wxRect(rect.X, rect.Y, rect.Width, rect.Height);
	}
	inline wxRegion ToRegion(const Gdiplus::Region& region, Graphics* graphics)
	{
		return wxRegion(region.GetHRGN(graphics));
	}
}

wxRect2DDouble KxGraphicsContext::GetClip() const
{
	Gdiplus::RectF rect;
	GetNativeContext(m_Context)->GetClipBounds(&rect);

	return ToRect2DDouble(rect);
}
wxRect KxGraphicsContext::GetClipInt() const
{
	Gdiplus::Rect rect;
	GetNativeContext(m_Context)->GetClipBounds(&rect);

	return ToRect(rect);
}
wxRegion KxGraphicsContext::GetClipRegion() const
{
	Gdiplus::Region region;
	if (GetNativeContext(m_Context)->GetClip(&region) == Gdiplus::Status::Ok)
	{
		return ToRegion(region, GetNativeContext(m_Context));
	}
	return KxNullWxRegion;
}
