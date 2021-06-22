#pragma once
#include "../../ColorDepth.h"
#include "../../Geometry.h"
#include "wx/msw/gdiimage.h"
#include <wx/gdicmn.h>

namespace kxf
{
	class GDIBitmap;
}

namespace kxf::Private
{
	inline std::vector<wxPoint> ConvertWxPoints(const Point* points, size_t count)
	{
		return {points, points + count};
	}

	template<size_t N>
	auto ConvertWxPoints(const Point(&points)[N]) noexcept
	{
		std::array<wxPoint, N> pointsBuffer;
		std::copy_n(std::begin(points), N, pointsBuffer.begin());

		return pointsBuffer;
	}

	template<size_t N>
	auto ConvertWxPoints(const std::array<Point, N>& points) noexcept
	{
		std::array<wxPoint, N> pointsBuffer;
		std::copy_n(points.begin(), N, pointsBuffer.begin());

		return pointsBuffer;
	}
}

namespace kxf::Drawing::Private
{
	template<class T, class = std::enable_if_t<std::is_base_of_v<wxGDIImage, T>>>
	void* DetachGDIImageHandle(T& gdiImage)
	{
		if (gdiImage.IsOk())
		{
			void* handle = nullptr;
			std::swap(handle, gdiImage.GetGDIImageData()->m_handle);
			gdiImage = T{};

			return handle;
		}
		return nullptr;
	}

	template<class TFunc>
	bool AttachIconHandle(wxGDIImage& gdiImage, void* handle, TFunc&& unshareFunc)
	{
		if (handle)
		{
			ICONINFOEXW iconInfo = {};
			if (::GetIconInfoExW(reinterpret_cast<HICON>(handle), &iconInfo))
			{
				auto GetBitmapInfo = [](void* handle, Size& size, ColorDepth& depth)
				{
					BITMAP bitmap = {};
					if (::GetObject(handle, sizeof(bitmap), &bitmap) != 0)
					{
						size = {bitmap.bmWidth, bitmap.bmHeight};
						depth = bitmap.bmBitsPixel;

						return true;
					}
					return false;
				};

				Size size;
				ColorDepth depth;
				bool hasInfo = false;

				if (iconInfo.hbmColor)
				{
					hasInfo = GetBitmapInfo(iconInfo.hbmColor, size, depth);
				}
				else if (iconInfo.hbmMask)
				{
					hasInfo = GetBitmapInfo(iconInfo.hbmMask, size, depth);
				}

				if (hasInfo && std::invoke(unshareFunc))
				{
					wxGDIImageRefData* refData = gdiImage.GetGDIImageData();
					refData->m_handle = handle;
					refData->m_width = size.GetWidth();
					refData->m_height = size.GetHeight();
					refData->m_depth = depth.GetValue();

					return true;
				}
			}

			// Delete the handle if we can't attach it
			::DeleteObject(handle);
		}
		return false;
	}

	GDIBitmap BitmapFromMemoryLocation(const void* data);
}
