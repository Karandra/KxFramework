#pragma once
#include "Kx/UI/Common.h"
#include "Kx/UI/WindowRefreshScheduler.h"
#include <wx/generic/statbmpg.h>

namespace KxFramework::UI
{
	enum class BitmapScaleMode
	{
		None = wxStaticBitmapBase::ScaleMode::Scale_None,
		Fill = wxStaticBitmapBase::ScaleMode::Scale_Fill,
		AspectFit = wxStaticBitmapBase::ScaleMode::Scale_AspectFit,
		AspectFill = wxStaticBitmapBase::ScaleMode::Scale_AspectFill,
	};
}

namespace KxFramework::UI
{
	class KX_API StaticBitmap: public wxGenericStaticBitmap
	{
		public:
			static constexpr WindowStyle DefaultStyle = WindowStyle::None;

		private:
			wxSize m_InitialSize;

		private:
			void OnPaint(wxPaintEvent& event);

		public:
			StaticBitmap() = default;
			StaticBitmap(wxWindow* parent,
						 wxWindowID id,
						 const wxBitmap& bitmap = {},
						 WindowStyle style = DefaultStyle
			)
			{
				Create(parent, id, bitmap, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const wxBitmap& bitmap = {},
						WindowStyle style = DefaultStyle
			);

		public:
			virtual wxImage GetImage() const;
			virtual void SetImage(const wxImage& image);

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
