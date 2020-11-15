#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "kxf/UI/Controls/StaticBitmap.h"
#include "kxf/Drawing/Image.h"
#include "kxf/IO/IStream.h"
#include <wx/control.h>
#include <wx/statbmp.h>
#include <wx/graphics.h>
#include <wx/systhemectrl.h>

namespace kxf::UI
{
	enum class ImageViewBackground
	{
		Solid,
		Gradient,
		TransparenryPattern
	};
}

namespace kxf::UI
{
	class KX_API ImageView: public WindowRefreshScheduler<wxSystemThemedControl<wxControl>>
	{
		public:
			static constexpr FlagSet<WindowStyle> DefaultStyle = CombineFlags<WindowStyle>(WindowBorder::Theme);

		private:
			wxGraphicsRenderer* m_Renderer = nullptr;
			ImageViewBackground m_BackgroundMode = ImageViewBackground::Solid;
			BitmapScaleMode m_ScaleMode = BitmapScaleMode::None;
			Direction m_GradientDirection = Direction::Down;
			double m_ScaleFactor = 1.0;
			bool m_IsAnimation = false;

			wxGraphicsBitmap m_Bitmap;
			Size m_ScaledImageSize;
			Size m_ImageSize;

		private:
			void DoSetBitmap(const wxGraphicsBitmap& bitmap, const Size& size);
			void OnDrawBackground(wxEraseEvent& event);
			void OnDrawForeground(wxPaintEvent& event);
			void OnSize(wxSizeEvent& event);

		public:

			ImageView() = default;
			ImageView(wxWindow* parent,
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

		public:
			ImageViewBackground GetBackgroundMode()
			{
				return m_BackgroundMode;
			}
			void SetBackgroundMode(ImageViewBackground mode)
			{
				m_BackgroundMode = mode;
				ScheduleRefresh();
			}

			Direction GetGradientDirection() const
			{
				return m_GradientDirection;
			}
			void SetGradientDirection(Direction mode)
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
			void SetScaleFactor(double factor);

			BitmapScaleMode GetScaleMode() const
			{
				return m_ScaleMode;
			}
			void SetScaleMode(BitmapScaleMode mode)
			{
				m_ScaleMode = mode;
				ScheduleRefresh();
			}

			bool HasBitmap() const
			{
				return !m_Bitmap.IsNull();
			}
			Image GetImage() const
			{
				return m_Bitmap.ConvertToImage();
			}
			GDIBitmap GetBitmap() const
			{
				return Image(m_Bitmap.ConvertToImage()).ToBitmap();
			}
			void SetBitmap(const GDIBitmap& bitmap);
			void SetBitmap(const Image& image);
			void SetBitmap(const wxGraphicsBitmap& image, const Size& size);
			void Load(IInputStream& stream, ImageFormat format = ImageFormat::Any, int index = -1);

		public:
			wxDECLARE_DYNAMIC_CLASS(ImageView);
	};
}
