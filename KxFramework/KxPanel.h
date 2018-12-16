#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxPanel: public wxPanel
{
	public:
		using ScaleMode = wxGenericStaticBitmap::ScaleMode;
		static const long DefaultStyle = wxTAB_TRAVERSAL|wxBORDER_NONE;

	public:
		KxPanel() {}
		KxPanel(wxWindow* parent,
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
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxPoint& pos,
					const wxSize& size,
					long style = DefaultStyle,
					const wxString& name = wxEmptyString
		)
		{
			return Create(parent, id, style);
		}

	public:
		virtual bool ShouldInheritColours() const override
		{
			return true;
		}
		virtual bool SetBackgroundColour(const wxColour& color) override
		{
			return wxPanel::SetBackgroundColour(color);
		}
		virtual bool SetForegroundColour(const wxColour& color) override
		{
			return wxPanel::SetForegroundColour(color);
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxPanel);
};
using KxScrolledPanel = KX_API wxScrolled<KxPanel>;

//////////////////////////////////////////////////////////////////////////
enum KxPanelBGMode
{
	KxPANEL_BG_NONE = 0,
	KxPANEL_BG_SOLID = 1,
	KxPANEL_BG_GRADIENT = 2,
	KxPANEL_BG_TRANSPARENCY_PATTERN = 4,

	KxPANEL_BG_IMAGE = 128,
	KxPANEL_FG_IMAGE = 256,
};

class KX_API KxDrawablePanel: public KxPanel
{
	public:
		using ScaleMode = KxPanel::ScaleMode;

	private:
		static const wxBitmap ms_EmptyBitmap;

	public:
		static wxSize DrawScaledBitmap(wxGraphicsContext* gc, const wxBitmap& bitmap, const wxRect& rect, ScaleMode scaleMode, double globalScale = 1.0);
		static wxSize DrawScaledBitmap(wxWindowDC& dc, const wxBitmap& bitmap, const wxRect& rect, ScaleMode scaleMode, double globalScale = 1.0);
		static wxSize DrawScaledBitmap(wxMemoryDC& dc, const wxBitmap& bitmap, const wxRect& rect, ScaleMode scaleMode, double globalScale = 1.0);
		static void DrawTransparencyPattern(wxDC& dc);

	private:
		wxBitmap m_Bitmap;
		ScaleMode m_ImageScaleMode = ScaleMode::Scale_None;
		KxPanelBGMode m_BackgroundMode = KxPANEL_BG_SOLID;
		wxDirection m_GradientDirection = wxDOWN;
		wxSize m_ScaledImageSize;
		double m_ScaleFactor = 1.0;

	private:
		void OnDrawBackground(wxEraseEvent& event);
		void OnDrawForeground(wxPaintEvent& event);

	public:
		KxDrawablePanel() {}
		KxDrawablePanel(wxWindow* parent,
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
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxPoint& pos,
					const wxSize& size,
					long style = DefaultStyle,
					const wxString& name = wxEmptyString
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
			Refresh();
		}
		
		ScaleMode GetScaleMode() const
		{
			return m_ImageScaleMode;
		}
		void SetScaleMode(ScaleMode mode)
		{
			m_ImageScaleMode = mode;
			Refresh();
		}
		void SetScaleMode(int mode)
		{
			SetScaleMode((ScaleMode)mode);
		}
		
		KxPanelBGMode GetBGMode() const
		{
			return m_BackgroundMode;
		}
		void SetBGMode(KxPanelBGMode mode)
		{
			m_BackgroundMode = mode;
			Refresh();
		}
		void SetBGMode(int mode)
		{
			SetBGMode((KxPanelBGMode)mode);
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
		void SetScaleFactor(double factor)
		{
			m_ScaleFactor = factor;
			Refresh();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDrawablePanel);
};
using KxScrolledDrawablePanel = KX_API wxScrolled<KxDrawablePanel>;
