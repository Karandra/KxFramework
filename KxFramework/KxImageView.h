#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/UI/WindowRefreshScheduler.h"

enum KxImageView_BGMode
{
	KxIV_BG_SOLID,
	KxIV_BG_GRADIENT,
	KxIV_BG_TRANSPARENCY_PATTERN,
};
enum KxImageView_ScaleMode
{
	KxIV_SCALE_NONE = wxStaticBitmapBase::Scale_None,
	KxIV_SCALE_FILL = wxStaticBitmapBase::Scale_Fill,
	KxIV_SCALE_ASPECT_FIT = wxStaticBitmapBase::Scale_AspectFit,
	KxIV_SCALE_ASPECT_FILL = wxStaticBitmapBase::Scale_AspectFill,
};

class KX_API KxImageView: public wxSystemThemedControl<KxFramework::WindowRefreshScheduler<wxControl>>
{
	private:
		wxGraphicsRenderer* m_Renderer = nullptr;
		KxImageView_BGMode m_BackgroundMode = KxIV_BG_SOLID;
		KxImageView_ScaleMode m_ScaleMode = KxIV_SCALE_NONE;
		wxDirection m_GradientDirection = wxDOWN;
		double m_ScaleFactor = 1.0;
		bool m_IsAnimation = false;

		wxGraphicsBitmap m_Bitmap;
		wxSize m_ScaledImageSize;
		wxSize m_ImageSize;

	private:
		void OnDrawBackground(wxEraseEvent& event);
		void OnDrawForeground(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);

		void DoSetBitmap(const wxGraphicsBitmap& bitmap, const wxSize& size);

	public:
		static const long DefaultStyle = wxBORDER_THEME;
		
		KxImageView() = default;
		KxImageView(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle
		)
		{
			Create(parent, id, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle
		);

	public:
		KxImageView_BGMode GetBackgroundMode()
		{
			return m_BackgroundMode;
		}
		void SetBackgroundMode(KxImageView_BGMode mode)
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

		wxSize GetScaledImageSize() const
		{
			return m_ScaledImageSize;
		}
		double GetScaleFactor() const
		{
			return m_ScaleFactor;
		}
		void SetScaleFactor(double factor);

		KxImageView_ScaleMode GetScaleMode() const
		{
			return m_ScaleMode;
		}
		void SetScaleMode(KxImageView_ScaleMode mode)
		{
			m_ScaleMode = mode;
			ScheduleRefresh();
		}

		bool HasBitmap() const
		{
			return !m_Bitmap.IsNull();
		}
		wxImage GetImage() const
		{
			return m_Bitmap.ConvertToImage();
		}
		wxBitmap GetBitmap() const
		{
			return wxBitmap(m_Bitmap.ConvertToImage(), 32);
		}
		void SetBitmap(const wxBitmap& image);
		void SetBitmap(const wxImage& image);
		void SetBitmap(const wxGraphicsBitmap& image, const wxSize& size);

		void LoadFile(const wxString& filePath, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);
		void LoadFile(wxInputStream& stream, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxImageView);
};
