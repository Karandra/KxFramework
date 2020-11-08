#include "stdafx.h"
#include "Brush.h"
#include "Private/GDI.h"

namespace
{
	class wxBrushRefDataHack final: public wxObjectRefData
	{
		public:
			wxBrushStyle m_style = wxBRUSHSTYLE_INVALID;
			wxBitmap m_stipple;
			wxColour m_colour;
			HBRUSH m_hBrush = nullptr;
	};

	constexpr std::optional<int> TranslateHatchStyle(wxBrushStyle style) noexcept
	{
		switch (style)
		{
			case wxBRUSHSTYLE_BDIAGONAL_HATCH:
			{
				return HS_BDIAGONAL;
			}
			case wxBRUSHSTYLE_CROSSDIAG_HATCH:
			{
				return HS_DIAGCROSS;
			}
			case wxBRUSHSTYLE_FDIAGONAL_HATCH:
			{
				return HS_FDIAGONAL;
			}
			case wxBRUSHSTYLE_CROSS_HATCH:
			{
				return HS_CROSS;
			}
			case wxBRUSHSTYLE_HORIZONTAL_HATCH:
			{
				return HS_HORIZONTAL;
			}
			case wxBRUSHSTYLE_VERTICAL_HATCH:
			{
				return HS_VERTICAL;
			}
		};
		return {};
	}
	constexpr wxBrushStyle TranslateHatchStyle(int style) noexcept
	{
		switch (style)
		{
			case HS_BDIAGONAL:
			{
				return wxBRUSHSTYLE_BDIAGONAL_HATCH;
			}
			case HS_DIAGCROSS:
			{
				return wxBRUSHSTYLE_CROSSDIAG_HATCH;
			}
			case HS_FDIAGONAL:
			{
				return wxBRUSHSTYLE_FDIAGONAL_HATCH;
			}
			case HS_CROSS:
			{
				return wxBRUSHSTYLE_CROSS_HATCH;
			}
			case HS_HORIZONTAL:
			{
				return wxBRUSHSTYLE_HORIZONTAL_HATCH;
			}
			case HS_VERTICAL:
			{
				return wxBRUSHSTYLE_VERTICAL_HATCH;
			}
		};
		return {};
	}
}

namespace kxf
{
	// IGDIObject
	void* Brush::GetHandle() const
	{
		return m_Brush.GetResourceHandle();
	}
	void* Brush::DetachHandle()
	{
		if (wxBrushRefDataHack* refData = static_cast<wxBrushRefDataHack*>(m_Brush.GetRefData()))
		{
			// 'GetResourceHandle' creates the actual brush object if it doesn't already exist
			void* handle = m_Brush.GetResourceHandle();

			// Clear the internal structures
			refData->m_hBrush = nullptr;
			refData->m_stipple = wxNullBitmap;
			refData->m_colour = wxNullColour;
			refData->m_style = wxBRUSHSTYLE_INVALID;

			return handle;
		}
	}
	void Brush::AttachHandle(void* handle)
	{
		if (handle)
		{
			m_Brush = *wxTRANSPARENT_BRUSH;

			LOGBRUSH brushInfo = {};
			if (::GetObjectW(handle, sizeof(brushInfo), &brushInfo) != 0)
			{
				if (wxBrushRefDataHack* refData = static_cast<wxBrushRefDataHack*>(m_Brush.GetRefData()))
				{
					refData->m_hBrush = static_cast<HBRUSH>(handle);
					refData->m_stipple = wxNullBitmap;
					refData->m_colour = wxNullColour;
					refData->m_style = wxBRUSHSTYLE_INVALID;

					if (::GetStockObject(NULL_BRUSH) == handle)
					{
						refData->m_style = wxBRUSHSTYLE_TRANSPARENT;
						return;
					}

					switch (brushInfo.lbStyle)
					{
						case BS_DIBPATTERN:
						case BS_DIBPATTERN8X8:
						case BS_DIBPATTERNPT:
						{
							auto ToBitmap = [](const void* data) -> Bitmap
							{
								const BITMAPINFO& bitmapInfo = *static_cast<const BITMAPINFO*>(data);
								const BITMAPINFOHEADER& bitmapHeader = bitmapInfo.bmiHeader;

								// We don't support other color depths here
								const ColorDepth colorDepth = bitmapHeader.biBitCount;
								if (colorDepth == ColorDepthDB::BPP24 || colorDepth == ColorDepthDB::BPP32)
								{
									// TODO: Check the correctness of this pointer
									const auto bitmapData = reinterpret_cast<const uint8_t*>(data) + sizeof(BITMAPINFO);

									Bitmap bitmap;
									bitmap.AttachHandle(::CreateBitmap(bitmapHeader.biWidth, bitmapHeader.biHeight, bitmapHeader.biPlanes, bitmapHeader.biBitCount, bitmapData));
									return bitmap;
								}
								return {};
							};

							// DIB_PAL_COLORS
							// DIB_RGB_COLORS
							const int colorType = LOWORD(brushInfo.lbColor);
							if (brushInfo.lbStyle == BS_DIBPATTERN || brushInfo.lbStyle == BS_DIBPATTERN8X8)
							{
								const void* data = ::GlobalLock(reinterpret_cast<HGLOBAL>(brushInfo.lbHatch));
								refData->m_stipple = ToBitmap(data).ToWxBitmap();
							}
							else if (brushInfo.lbStyle == BS_DIBPATTERNPT)
							{
								const void* data = reinterpret_cast<const void*>(brushInfo.lbHatch);
								refData->m_stipple = ToBitmap(data).ToWxBitmap();
							}
							break;
						}
						case BS_PATTERN:
						case BS_PATTERN8X8:
						{
							Bitmap bitmap;
							bitmap.AttachHandle(reinterpret_cast<HBITMAP>(brushInfo.lbHatch));

							refData->m_stipple = bitmap.ToWxBitmap();
							refData->m_style = wxBRUSHSTYLE_STIPPLE;

							break;
						}
						case BS_HATCHED:
						case BS_SOLID:
						{
							refData->m_colour = Color::FromCOLORREF(brushInfo.lbColor).ToWxColor();
							if (brushInfo.lbStyle == BS_HATCHED)
							{
								refData->m_style = TranslateHatchStyle(static_cast<int>(brushInfo.lbHatch));
							}
							else
							{
								refData->m_style = wxBRUSHSTYLE_SOLID;
							}
							break;
						}
						case BS_NULL:
						{
							refData->m_style = wxBRUSHSTYLE_TRANSPARENT;
							break;
						}
					};
					return;
				}

				// Delete the handle if we can't attach it
				::DeleteObject(handle);
			}
		}
		else
		{
			m_Brush = wxBrush();
		}
	}
}

namespace kxf::Drawing
{
	Brush GetStockBrush(StockBrush brush)
	{
		switch (brush)
		{
			case StockBrush::Black:
			{
				return *wxBLACK_BRUSH;
			}
			case StockBrush::Cyan:
			{
				return *wxCYAN_BRUSH;
			}
			case StockBrush::Blue:
			{
				return *wxBLUE_BRUSH;
			}
			case StockBrush::Red:
			{
				return *wxRED_BRUSH;
			}
			case StockBrush::Green:
			{
				return *wxGREY_BRUSH;
			}
			case StockBrush::Yellow:
			{
				return *wxYELLOW_BRUSH;
			}
			case StockBrush::Gray:
			{
				return *wxGREY_BRUSH;
			}
			case StockBrush::LightGray:
			{
				return *wxLIGHT_GREY_BRUSH;
			}
			case StockBrush::MediumGray:
			{
				return *wxMEDIUM_GREY_BRUSH;
			}
			case StockBrush::White:
			{
				return *wxWHITE_BRUSH;
			}
			case StockBrush::Transparent:
			{
				return *wxTRANSPARENT_BRUSH;
			}
		};
		return {};
	}
}
