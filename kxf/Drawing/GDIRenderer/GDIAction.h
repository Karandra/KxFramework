#pragma once
#include "Common.h"
#include "GDIContext.h"

namespace kxf::Drawing
{
	Color GetAreaAverageColor(const GDIContext& dc, const Rect& rect);
}

namespace kxf::GDIAction
{
	class KX_API Clip final
	{
		private:
			GDIContext& m_DC;
			Region m_Region;

		public:
			Clip(GDIContext& dc)
				:m_DC(dc)
			{
			}
			Clip(GDIContext& dc, const Rect& rect)
				:m_DC(dc), m_Region(dc.LogicalToDevice(rect))
			{
			}
			Clip(GDIContext& dc, const Region& region)
				:m_DC(dc), m_Region(region)
			{
			}
			~Clip()
			{
				if (m_Region)
				{
					m_DC.ResetClippingRegion();
				}
			}

		public:
			bool Add(const Region& region)
			{
				return m_Region.Union(region);
			}
			bool Add(const Rect& rect)
			{
				return m_Region.Union(m_DC.LogicalToDevice(rect));
			}

			bool Remove(const Region& region)
			{
				return m_Region.Subtract(region);
			}
			bool Remove(const Rect& rect)
			{
				return m_Region.Subtract(m_DC.LogicalToDevice(rect));
			}

			void Apply()
			{
				if (m_Region)
				{
					m_DC.SetDeviceClippingRegion(m_Region);
				}
			}
	};

	class ChangeLogicalFunction final
	{
		private:
			GDIContext& m_DC;
			GDILogicalFunction m_OriginalMode = GDILogicalFunction::Clear;

		public:
			ChangeLogicalFunction(GDIContext& dc, GDILogicalFunction newMode)
				:m_DC(dc), m_OriginalMode(dc.GetLogicalFunction())
			{
				m_DC.SetLogicalFunction(newMode);
			}
			~ChangeLogicalFunction()
			{
				m_DC.SetLogicalFunction(m_OriginalMode);
			}
	};

	class ChangeFont final
	{
		private:
			GDIContext& m_DC;
			Font m_OriginalFont;

		public:
			ChangeFont(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeFont(GDIContext& dc, const Font& font)
				:m_DC(dc), m_OriginalFont(dc.GetFont())
			{
				m_DC.SetFont(font.ToWxFont());
			}
			~ChangeFont()
			{
				if (m_OriginalFont)
				{
					m_DC.SetFont(std::move(m_OriginalFont));
				}
			}

		public:
			void Set(const Font& font)
			{
				if (!m_OriginalFont)
				{
					m_OriginalFont = m_DC.GetFont();
				}
				m_DC.SetFont(font);
			}
	};

	class ChangePen final
	{
		private:
			GDIContext& m_DC;
			Pen m_Pen;

		public:
			ChangePen(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangePen(GDIContext& dc, const Pen& pen)
				:m_DC(dc), m_Pen(dc.GetPen())
			{
				m_DC.SetPen(pen);
			}
			~ChangePen()
			{
				if (m_Pen)
				{
					m_DC.SetPen(std::move(m_Pen));
				}
			}

		public:
			void Set(const Pen& pen)
			{
				if (!m_Pen)
				{
					m_Pen = m_DC.GetPen();
				}
				m_DC.SetPen(pen);
			}
	};

	class ChangeBrush final
	{
		private:
			GDIContext& m_DC;
			Brush m_Brush;

		public:
			ChangeBrush(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeBrush(GDIContext& dc, const Brush& brush)
				:m_DC(dc), m_Brush(dc.GetBrush())
			{
				m_DC.SetBrush(brush);
			}
			~ChangeBrush()
			{
				if (m_Brush)
				{
					m_DC.SetBrush(std::move(m_Brush));
				}
			}

		public:
			void Set(const Brush& brush)
			{
				if (!m_Brush)
				{
					m_Brush = m_DC.GetBrush();
				}
				m_DC.SetBrush(brush);
			}
	};

	class ChangeTextForeground final
	{
		private:
			GDIContext& m_DC;
			Color m_OriginalColor;

		public:
			ChangeTextForeground(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeTextForeground(GDIContext& dc, const Color& color)
				:m_DC(dc), m_OriginalColor(dc.GetTextForeground())
			{
				m_DC.SetTextForeground(color);
			}
			~ChangeTextForeground()
			{
				if (m_OriginalColor)
				{
					m_DC.SetTextForeground(m_OriginalColor);
				}
			}

		public:
			void Set(const Color& color)
			{
				if (!m_OriginalColor)
				{
					m_OriginalColor = m_DC.GetTextForeground();
				}
				m_DC.SetTextForeground(color);
			}
	};

	class ChangeTextBackground final
	{
		private:
			GDIContext& m_DC;
			Color m_OriginalColor;

		public:
			ChangeTextBackground(GDIContext& dc)
				:m_DC(dc)
			{
			}
			ChangeTextBackground(GDIContext& dc, const Color& color)
				:m_DC(dc), m_OriginalColor(dc.GetTextForeground())
			{
				m_DC.SetTextBackground(color);
			}
			~ChangeTextBackground()
			{
				if (m_OriginalColor)
				{
					m_DC.SetTextBackground(m_OriginalColor);
				}
			}

		public:
			void Set(const Color& color)
			{
				if (!m_OriginalColor)
				{
					m_OriginalColor = m_DC.GetTextBackground();
				}
				m_DC.SetTextBackground(color);
			}
	};
}
