#pragma once
#include "Common.h"
#include "Color.h"
#include "Font.h"
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/region.h>

namespace kxf::Drawing
{
	Color GetAreaAverageColor(const wxDC& dc, const Rect& rect);
}

namespace kxf
{
	class KX_API DCClip final
	{
		private:
			wxDC& m_DC;
			wxRegion m_Region;

		public:
			DCClip(wxDC& dc, const Rect& rect)
				:m_DC(dc), m_Region(rect)
			{
			}
			DCClip(wxDC& dc, const wxRegion& region)
				:m_DC(dc), m_Region(region)
			{
			}
			~DCClip()
			{
				if (m_Region.IsOk())
				{
					m_DC.DestroyClippingRegion();
				}
			}

		public:
			bool Add(const wxRegion& region)
			{
				return m_Region.Union(region);
			}
			bool Add(const Rect& rect)
			{
				return m_Region.Union(rect);
			}

			bool Remove(const wxRegion& region)
			{
				return m_Region.Subtract(region);
			}
			bool Remove(const Rect& rect)
			{
				return m_Region.Subtract(rect);
			}

			void Apply()
			{
				m_DC.SetDeviceClippingRegion(m_Region);
			}
	};

	class DCChangeLogicalFunction final
	{
		private:
			wxDC& m_DC;
			const wxRasterOperationMode m_OriginalMode = wxRasterOperationMode::wxCLEAR;

		public:
			DCChangeLogicalFunction(wxDC& dc, wxRasterOperationMode newMode)
				:m_DC(dc), m_OriginalMode(dc.GetLogicalFunction())
			{
				m_DC.SetLogicalFunction(newMode);
			}
			~DCChangeLogicalFunction()
			{
				m_DC.SetLogicalFunction(m_OriginalMode);
			}
	};

	class DCFontChanger final
	{
		private:
			wxDC& m_DC;
			Font m_OriginalFont;

		public:
			DCFontChanger(wxDC& dc)
				:m_DC(dc)
			{
			}
			DCFontChanger(wxDC& dc, const Font& font)
				:m_DC(dc), m_OriginalFont(dc.GetFont())
			{
				m_DC.SetFont(font.ToWxFont());
			}
			~DCFontChanger()
			{
				if (m_OriginalFont)
				{
					m_DC.SetFont(m_OriginalFont.ToWxFont());
				}
			}

		public:
			void Set(const Font& font)
			{
				if (!m_OriginalFont)
				{
					m_OriginalFont = m_DC.GetFont();
				}
				m_DC.SetFont(font.ToWxFont());
			}
	};

	class DCTextColorChanger final
	{
		private:
			wxDC& m_DC;
			Color m_OriginalColor;

		public:
			DCTextColorChanger(wxDC& dc)
				:m_DC(dc)
			{
			}
			DCTextColorChanger(wxDC& dc, const Color& color)
				:m_DC(dc), m_OriginalColor(dc.GetTextForeground())
			{
				m_DC.SetTextForeground(color.ToWxColor());
			}
			~DCTextColorChanger()
			{
				if (m_OriginalColor)
				{
					m_DC.SetTextForeground(m_OriginalColor.ToWxColor());
				}
			}

		public:
			void Set(const Color& color)
			{
				if (!m_OriginalColor)
				{
					m_OriginalColor = m_DC.GetTextForeground();
				}
				m_DC.SetTextForeground(color.ToWxColor());
			}
	};
}
