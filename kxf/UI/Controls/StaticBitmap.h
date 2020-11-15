#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/generic/statbmpg.h>

namespace kxf::UI
{
	enum class BitmapScaleMode
	{
		None = wxStaticBitmapBase::ScaleMode::Scale_None,
		Fill = wxStaticBitmapBase::ScaleMode::Scale_Fill,
		AspectFit = wxStaticBitmapBase::ScaleMode::Scale_AspectFit,
		AspectFill = wxStaticBitmapBase::ScaleMode::Scale_AspectFill,
	};
}

namespace kxf::UI
{
	class KX_API StaticBitmap: public wxGenericStaticBitmap
	{
		public:
			static constexpr FlagSet<WindowStyle> DefaultStyle = WindowStyle::None;

		private:
			Size m_InitialSize;

		private:
			void OnPaint(wxPaintEvent& event);

		public:
			StaticBitmap() = default;
			StaticBitmap(wxWindow* parent,
						 wxWindowID id,
						 const GDIBitmap& bitmap = {},
						 FlagSet<WindowStyle> style = DefaultStyle
			)
			{
				Create(parent, id, bitmap, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const GDIBitmap& bitmap = {},
						FlagSet<WindowStyle> style = DefaultStyle
			);

		public:
			virtual Image GetImage() const;
			virtual void SetImage(const Image& image);

			BitmapScaleMode GetBitmapScaleMode() const
			{
				return static_cast<BitmapScaleMode>(wxGenericStaticBitmap::GetScaleMode());
			}
			void SetBitmapScaleMode(BitmapScaleMode scaleMode)
			{
				wxGenericStaticBitmap::SetScaleMode(static_cast<wxGenericStaticBitmap::ScaleMode>(scaleMode));
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(StaticBitmap);
	};
}
