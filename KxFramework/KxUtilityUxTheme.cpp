#include "KxStdAfx.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxColor.h"
#include "KxFramework/KxUxTheme.h"

bool KxUtility::DrawParentBackground(const wxWindow* window, wxDC& dc, const wxRect& rect)
{
	RECT rectWin = CopyRectToRECT(rect);
	::DrawThemeParentBackground(window->GetHandle(), dc.GetHDC(), &rectWin);
	return true;
}
bool KxUtility::DrawThemeBackground(const wxWindow* window, const wxString& className, wxDC& dc, int iPartId, int iStateId, const wxRect& rect)
{
	if (KxUxTheme::Handle handle(window, className); handle)
	{
		auto rectWin = CopyRectToRECT(rect);
		return ::DrawThemeBackground(handle, dc.GetHDC(), iPartId, iStateId, &rectWin, nullptr) == S_OK;
	}
	return false;
}

KxColor KxUtility::GetThemeColor(const wxWindow* window, const wxString& className, int iPartId, int iStateId, int iPropId, const wxColour& defaultColor)
{
	KxColor color = defaultColor;
	if (KxUxTheme::Handle handle(window, className); handle)
	{
		COLORREF colorref = 0;
		if (::GetThemeColor(handle, iPartId, iStateId, iPropId, &colorref) == S_OK)
		{
			color.SetCOLORREF(colorref);
		}
	}
	return color;
}
KxColor KxUtility::GetThemeColor_Caption(const wxWindow* window)
{
	wxColour color = GetThemeColor(window, L"TEXTSTYLE", TEXT_MAININSTRUCTION, 0, TMT_TEXTCOLOR);

	// Approximation of caption color from default Aero style
	return color.IsOk() ? color : wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT).ChangeLightness(65);
}

bool KxUtility::DrawLabel(const wxWindow* window, wxDC& dc, const wxString& label, const wxBitmap& icon, wxRect rect, int alignment, int accelIndex, wxRect* boundingRect)
{
	return false;
	#if 0
	HTHEME hTheme = OpenThemeData(window->GetHandle(), L"LISTVIEW");
	//
	DTTOPTS tOptions = {0};
	tOptions.dwSize = sizeof(tOptions);
	tOptions.dwFlags = DTT_COMPOSITED|DTT_GLOWSIZE;
	tOptions.iGlowSize = 5;
	tOptions.crText = dc.GetTextForeground().GetPixel();
	if (icon.IsOk())
	{
		dc.DrawLabel(wxEmptyString, icon, rect, alignment, accelIndex, boundingRect);
		rect.SetLeft(rect.GetLeft() + icon.GetWidth() + 3);
	}

	RECT tRectWin = CopyRectToRECT(rect);
	wxMemoryDC hMemDC(&dc);
	hMemDC.CopyAttributes(dc);

	wxBitmap hBMP(dc.GetSize(), 32);
	hMemDC.SelectObject(hBMP);
	DrawThemeTextEx(hTheme, hMemDC.GetHDC(), 0, 0, label.wc_str(), label.Length(), alignment, &tRectWin, &tOptions);
	//hMemDC.SelectObject(wxNullBitmap);
	//hBMP = wxBitmap(hBMP.ConvertToImage().Mirror(false), 32);
	//dc.DrawBitmap(hBMP, rect.GetPosition(), true);
	
	wxImage image = hBMP.ConvertToImage().Mirror(false);
	BYTE* pColorData = image.GetData();
	BYTE* pAlphaData = image.GetAlpha();
	for (size_t i = 0; i < (size_t)image.GetWidth()*image.GetHeight()*3; i += 3)
	{
		BYTE A = pAlphaData[i/3];
		pColorData[i] = pColorData[i] * A / 255;
		pColorData[i+1] = pColorData[i+1] * A / 255;
		pColorData[i+2] = pColorData[i+2] * A / 255;
	}

	hBMP = wxBitmap(image, 32);
	hMemDC.Clear();
	hMemDC.DrawBitmap(hBMP, rect.GetPosition(), true);

	BLENDFUNCTION tBlendFunction = {0};
	tBlendFunction.BlendOp = AC_SRC_OVER;
	tBlendFunction.SourceConstantAlpha = 255;
	tBlendFunction.AlphaFormat = AC_SRC_ALPHA;
	AlphaBlend(dc.GetHDC(), rect.GetPosition().x, rect.GetPosition().y, rect.GetWidth(), rect.GetHeight(), hMemDC.GetHDC(), 0, 0, rect.GetWidth(), rect.GetHeight(), tBlendFunction);

	dc.GetAsBitmap().SaveFile("zzz.png", wxBITMAP_TYPE_PNG);
	hMemDC.SelectObject(wxNullBitmap);

	if (boundingRect != nullptr)
	{
		*boundingRect = CopyRECTToRect(tRectWin);
	}
	//
	CloseThemeData(hTheme);
	#endif
}
bool KxUtility::DrawGripper(const wxWindow* window, wxDC& dc, const wxRect& rect)
{
	RECT rectWin = CopyRectToRECT(rect);
	if (!::DrawFrameControl(dc.GetHDC(), &rectWin, DFC_SCROLL, DFCS_SCROLLSIZEGRIP))
	{
		static const char* ms_SizeGripData[] =
		{
			/* width height num_colors chars_per_pixel */
			"11 11 3 1",
			/* colors */
			"  c None",
			"! c #ffffff",
			"# c #a0a0a0",
			/* pixels */
			"        ## ",
			"        ##!",
			"         !!",
			"           ",
			"    ##  ## ",
			"    ##! ##!",
			"     !!  !!",
			"           ",
			"##  ##  ## ",
			"##! ##! ##!",
			" !!  !!  !!"
		};
		static const wxBitmap ms_SizeGripBitmap(ms_SizeGripData);
		dc.DrawBitmap(ms_SizeGripBitmap, rect.GetPosition(), true);
	}
	return true;
}
