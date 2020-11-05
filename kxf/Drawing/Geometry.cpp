#include "stdafx.h"
#include "Geometry.h"

namespace kxf
{
	Size Size::FromPoint(const Point& point) noexcept
	{
		return {point.GetX(), point.GetY()};
	}

	Point Point::FromSize(const Size& size) noexcept
	{
		return {size.GetWidth(), size.GetHeight()};
	}
}
