#pragma once
#include "Common.h"
#include "Color.h"
#include "Font.h"
#include "GDICanvas.h"

namespace kxf::Drawing
{
	Color GetAreaAverageColor(const GDICanvas& canvas, const Rect& rect);
}

namespace kxf::GDICanvasAction
{
	class KX_API Clip final
	{
		private:
			GDICanvas& m_Canvas;
			Region m_Region;

		public:
			Clip(GDICanvas& canvas)
				:m_Canvas(canvas)
			{
			}
			Clip(GDICanvas& canvas, const Rect& rect)
				:m_Canvas(canvas), m_Region(canvas.LogicalToDevice(rect))
			{
			}
			Clip(GDICanvas& canvas, const Region& region)
				:m_Canvas(canvas), m_Region(region)
			{
			}
			~Clip()
			{
				if (m_Region)
				{
					m_Canvas.ResetClippingRegion();
				}
			}

		public:
			bool Add(const Region& region)
			{
				return m_Region.Union(region);
			}
			bool Add(const Rect& rect)
			{
				return m_Region.Union(m_Canvas.LogicalToDevice(rect));
			}

			bool Remove(const Region& region)
			{
				return m_Region.Subtract(region);
			}
			bool Remove(const Rect& rect)
			{
				return m_Region.Subtract(m_Canvas.LogicalToDevice(rect));
			}

			void Apply()
			{
				if (m_Region)
				{
					m_Canvas.SetDeviceClippingRegion(m_Region);
				}
			}
	};

	class ChangeLogicalFunction final
	{
		private:
			GDICanvas& m_Canvas;
			GDILogicalFunction m_OriginalMode = GDILogicalFunction::Clear;

		public:
			ChangeLogicalFunction(GDICanvas& canvas, GDILogicalFunction newMode)
				:m_Canvas(canvas), m_OriginalMode(canvas.GetLogicalFunction())
			{
				m_Canvas.SetLogicalFunction(newMode);
			}
			~ChangeLogicalFunction()
			{
				m_Canvas.SetLogicalFunction(m_OriginalMode);
			}
	};

	class ChangeFont final
	{
		private:
			GDICanvas& m_Canvas;
			Font m_OriginalFont;

		public:
			ChangeFont(GDICanvas& canvas)
				:m_Canvas(canvas)
			{
			}
			ChangeFont(GDICanvas& canvas, const Font& font)
				:m_Canvas(canvas), m_OriginalFont(canvas.GetFont())
			{
				m_Canvas.SetFont(font.ToWxFont());
			}
			~ChangeFont()
			{
				if (m_OriginalFont)
				{
					m_Canvas.SetFont(std::move(m_OriginalFont));
				}
			}

		public:
			void Set(const Font& font)
			{
				if (!m_OriginalFont)
				{
					m_OriginalFont = m_Canvas.GetFont();
				}
				m_Canvas.SetFont(font);
			}
	};

	class ChangePen final
	{
		private:
			GDICanvas& m_Canvas;
			Pen m_Pen;

		public:
			ChangePen(GDICanvas& canvas)
				:m_Canvas(canvas)
			{
			}
			ChangePen(GDICanvas& canvas, const Pen& pen)
				:m_Canvas(canvas), m_Pen(canvas.GetPen())
			{
				m_Canvas.SetPen(pen);
			}
			~ChangePen()
			{
				if (m_Pen)
				{
					m_Canvas.SetPen(std::move(m_Pen));
				}
			}

		public:
			void Set(const Pen& pen)
			{
				if (!m_Pen)
				{
					m_Pen = m_Canvas.GetPen();
				}
				m_Canvas.SetPen(pen);
			}
	};

	class ChangeBrush final
	{
		private:
			GDICanvas& m_Canvas;
			Brush m_Brush;

		public:
			ChangeBrush(GDICanvas& canvas)
				:m_Canvas(canvas)
			{
			}
			ChangeBrush(GDICanvas& canvas, const Brush& brush)
				:m_Canvas(canvas), m_Brush(canvas.GetBrush())
			{
				m_Canvas.SetBrush(brush);
			}
			~ChangeBrush()
			{
				if (m_Brush)
				{
					m_Canvas.SetBrush(std::move(m_Brush));
				}
			}

		public:
			void Set(const Brush& brush)
			{
				if (!m_Brush)
				{
					m_Brush = m_Canvas.GetBrush();
				}
				m_Canvas.SetBrush(brush);
			}
	};

	class ChangeTextForeground final
	{
		private:
			GDICanvas& m_Canvas;
			Color m_OriginalColor;

		public:
			ChangeTextForeground(GDICanvas& canvas)
				:m_Canvas(canvas)
			{
			}
			ChangeTextForeground(GDICanvas& canvas, const Color& color)
				:m_Canvas(canvas), m_OriginalColor(canvas.GetTextForeground())
			{
				m_Canvas.SetTextForeground(color);
			}
			~ChangeTextForeground()
			{
				if (m_OriginalColor)
				{
					m_Canvas.SetTextForeground(m_OriginalColor);
				}
			}

		public:
			void Set(const Color& color)
			{
				if (!m_OriginalColor)
				{
					m_OriginalColor = m_Canvas.GetTextForeground();
				}
				m_Canvas.SetTextForeground(color);
			}
	};

	class ChangeTextBackground final
	{
		private:
			GDICanvas& m_Canvas;
			Color m_OriginalColor;

		public:
			ChangeTextBackground(GDICanvas& canvas)
				:m_Canvas(canvas)
			{
			}
			ChangeTextBackground(GDICanvas& canvas, const Color& color)
				:m_Canvas(canvas), m_OriginalColor(canvas.GetTextForeground())
			{
				m_Canvas.SetTextBackground(color);
			}
			~ChangeTextBackground()
			{
				if (m_OriginalColor)
				{
					m_Canvas.SetTextBackground(m_OriginalColor);
				}
			}

		public:
			void Set(const Color& color)
			{
				if (!m_OriginalColor)
				{
					m_OriginalColor = m_Canvas.GetTextBackground();
				}
				m_Canvas.SetTextBackground(color);
			}
	};
}
