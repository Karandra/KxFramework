#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxStaticBitmap: public wxGenericStaticBitmap
{
	private:
		wxSize m_InitialSize;

	private:
		void OnPaint(wxPaintEvent& event);

	public:
		static const long DefaultStyle = 0;

		KxStaticBitmap() {}
		KxStaticBitmap(wxWindow* parent,
					   wxWindowID id,
					   const wxBitmap& bitmap = wxNullBitmap,
					   long style = DefaultStyle
		)
		{
			Create(parent, id, bitmap, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxBitmap& bitmap = wxNullBitmap,
					long style = DefaultStyle
		);

	public:
		virtual wxImage GetImage() const
		{
			return GetBitmap().ConvertToImage();
		}
		virtual void SetImage(const wxImage& image)
		{
			SetBitmap(wxBitmap(image, 32));
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStaticBitmap);
};
