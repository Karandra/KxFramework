#pragma once
#include "kxf/UI/Common.h"
#include "Panel.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "kxf/UI/Controls/StaticBitmap.h"
#include <wx/graphics.h>

namespace kxf::UI
{
	enum class DrawablePanelMode: uint32_t
	{
		None = 0,

		Soild = 1 << 0,
		Gradient = 1 << 1,
		TransparencyPattern = 1 << 2,
		BGImage = 1 << 3,
		FGImage = 1 << 4,
	};
}
namespace kxf
{
	KxFlagSet_Declare(UI::DrawablePanelMode);
}

namespace kxf::UI
{
	class KX_API DrawablePanel: public WindowRefreshScheduler<Panel>
	{
		public:
			static Size DrawScaledBitmap(wxGraphicsContext* gc, const wxGraphicsBitmap& bitmap, const Size& bitmapSize, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static Size DrawScaledBitmap(wxGraphicsContext* gc, const wxBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static Size DrawScaledBitmap(wxWindowDC& dc, const wxBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static Size DrawScaledBitmap(wxMemoryDC& dc, const wxBitmap& bitmap, const Rect& rect, BitmapScaleMode scaleMode, double globalScale = 1.0);
			static void DrawTransparencyPattern(wxDC& dc);

		private:
			wxBitmap m_Bitmap;
			BitmapScaleMode m_ImageScaleMode = BitmapScaleMode::None;
			FlagSet<DrawablePanelMode> m_BackgroundMode = DrawablePanelMode::Soild;
			wxDirection m_GradientDirection = wxDOWN;
			Size m_ScaledImageSize;
			double m_ScaleFactor = 1.0;

		private:
			void OnDrawBackground(wxEraseEvent& event);
			void OnDrawForeground(wxPaintEvent& event);

		public:
			DrawablePanel() = default;
			DrawablePanel(wxWindow* parent,
						  wxWindowID id,
						  FlagSet<WindowStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<WindowStyle> style = DefaultStyle
			);
			bool Create(wxWindow* parent,
						wxWindowID id,
						const Point& pos,
						const Size& size,
						FlagSet<WindowStyle> style = DefaultStyle,
						const String& name = {}
			)
			{
				return Create(parent, id, style);
			}

		public:
			bool HasBitmap() const
			{
				return m_Bitmap.IsOk();
			}
			const wxBitmap& GetBitmap() const
			{
				return m_Bitmap;
			}
			void SetBitmap(const wxBitmap& image)
			{
				m_Bitmap = image;
				ScheduleRefresh();
			}
		
			BitmapScaleMode GetScaleMode() const
			{
				return m_ImageScaleMode;
			}
			void SetScaleMode(BitmapScaleMode mode)
			{
				m_ImageScaleMode = mode;
				ScheduleRefresh();
			}
			
			FlagSet<DrawablePanelMode> GetBGMode() const
			{
				return m_BackgroundMode;
			}
			void SetBGMode(FlagSet<DrawablePanelMode> mode)
			{
				m_BackgroundMode = mode;
				ScheduleRefresh();
			}
		
			wxDirection GetGradientDirection() const
			{
				return m_GradientDirection;
			}
			void SetGradientDirection(wxDirection mode)
			{
				m_GradientDirection = mode;
				ScheduleRefresh();
			}

			Size GetScaledImageSize() const
			{
				return m_ScaledImageSize;
			}
			double GetScaleFactor() const
			{
				return m_ScaleFactor;
			}
			void SetScaleFactor(double factor)
			{
				m_ScaleFactor = factor;
				ScheduleRefresh();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(DrawablePanel);
	};

	using ScrolledDrawablePanel = wxScrolled<DrawablePanel>;
}
