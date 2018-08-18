#pragma once
#include "KxFramework/KxFramework.h"

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

class KxImageView: public wxControl
{
	private:
		KxImageView_BGMode m_BackgroundMode = KxIV_BG_SOLID;
		KxImageView_ScaleMode m_ScaleMode = KxIV_SCALE_NONE;
		wxDirection m_GradientDirection = wxDOWN;
		wxSize m_ScaledImageSize;
		double m_ScaleFactor = 1.0;

		bool m_IsAnimation = false;
		wxBitmap m_Bitmap;

	private:
		void OnDrawBackground(wxEraseEvent& event);
		void OnDrawForeground(wxPaintEvent& event);

	public:
		static const long DefaultStyle = wxBORDER_THEME;
		
		KxImageView() {}
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
		virtual ~KxImageView();

		#if 0
	public:
		virtual wxAnimation GetAnimation() const override;
		virtual void SetAnimation(const wxAnimation& anim) override;
		virtual bool IsPlaying() const override;
		virtual bool Play() override;
		bool Play(bool isLoop);
		virtual void Stop() override;
		virtual void SetInactiveBitmap(const wxBitmap& bitmap) override;
		#endif

	public:
		KxImageView_BGMode GetBackgroundMode()
		{
			return m_BackgroundMode;
		}
		void SetBackgroundMode(KxImageView_BGMode mode)
		{
			m_BackgroundMode = mode;
			Refresh();
		}
		
		wxDirection GetGradientDirection() const
		{
			return m_GradientDirection;
		}
		void SetGradientDirection(wxDirection mode)
		{
			m_GradientDirection = mode;
			Refresh();
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
			Refresh();
		}

		bool HasBitmap() const
		{
			return m_Bitmap.IsOk();
		}
		const wxBitmap& GetBitmap() const
		{
			return m_Bitmap;
		}
		void SetBitmap(const wxBitmap& image);
		void LoadFile(const wxString& filePath, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);
		void LoadFile(wxInputStream& stream, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxImageView);
};
