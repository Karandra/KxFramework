#pragma once
#include "KxFramework/KxFramework.h"

enum KxColor_ToStringMode
{
	KxC2S_LUA_SYNTAX = 0,
	KxC2S_NAME = wxC2S_NAME,
	KxC2S_CSS_SYNTAX = wxC2S_CSS_SYNTAX,
	KxC2S_HTML_SYNTAX = wxC2S_HTML_SYNTAX,
};

class KxColor: public wxColour
{
	public:
		static const int ALPHA_OPAQUE_PERCENT = 100;
		static const double SRGB_WeightR;
		static const double SRGB_WeightG;
		static const double SRGB_WeightB;

	public:
		static KxColor FromRGBA(wxUint32 color)
		{
			return KxColor().SetRGBA(color);
		}
		static KxColor FromARGB(wxUint32 color)
		{
			return KxColor().SetARGB(color);
		}
		static KxColor FromCOLORREF(wxUint32 color)
		{
			return KxColor().SetCOLORREF(color);
		}

	private:
		static double HUE2RGB(double p, double q, double t);
		static wxByte GetAlphaCOLORREF(COLORREF RGB)
		{
			return LOBYTE((RGB >> 24));
		}
		static COLORREF MakeCOLORREF(wxByte R, wxByte G, wxByte B, wxByte A)
		{
			return ((DWORD)A << 24) | RGB(R, G, B);
		}

	public:
		KxColor()
			:wxColour()
		{
		}
		KxColor(wxByte R, wxByte G, wxByte B, wxByte A = wxALPHA_OPAQUE)
			:wxColour(R, G, B, A)
		{
		}
		KxColor(const wchar_t* name)
			:wxColour(name)
		{
		}
		KxColor(const char* name)
			:wxColour(name)
		{
		}
		KxColor(const wxString& name)
			:wxColour(name)
		{
		}
		KxColor(wxUint32 RGB)
			:wxColour(RGB)
		{
		}
		KxColor(const wxColour& color)
			:wxColour(color)
		{
		}

	public:
		virtual bool IsOk() const override;
		virtual wxString GetAsString(long mode = KxC2S_LUA_SYNTAX) const override;
		KxColor Clone() const;
		
		KxColor& AlphaBlend(const KxColor& source);
		KxColor& MakeMono(bool bOn = true);
		KxColor& MakeGray(double weightR, double weightG, double weightB);
		KxColor& MakeGraySRGB()
		{
			return MakeGray(SRGB_WeightR, SRGB_WeightG, SRGB_WeightB);
		}
		KxColor& MakeGray();
		KxColor& ChangeLightness(int alpha);
		KxColor& RotateHue(double angle);
		KxColor& Invert();
		KxColor& Negate();

		KxColor AlphaBlend(const KxColor& source) const
		{
			return Clone().AlphaBlend(source);
		}
		KxColor MakeMono(bool bOn = true) const
		{
			return Clone().MakeMono(bOn);
		}
		KxColor MakeGray(double weightR, double weightG, double weightB) const
		{
			return Clone().MakeGray(weightR, weightG, weightB);
		}
		KxColor MakeGraySRGB() const
		{
			return Clone().MakeGraySRGB();
		}
		KxColor MakeGray() const
		{
			return Clone().MakeGray();
		}
		KxColor ChangeLightness(int alpha) const
		{
			return Clone().ChangeLightness(alpha);
		}
		KxColor RotateHue(double angle) const
		{
			return Clone().RotateHue(angle);
		}
		KxColor Invert() const
		{
			return Clone().Invert();
		}
		KxColor Negate() const
		{
			return Clone().Negate();
		}
		
		wxByte GetR() const
		{
			return Red();
		}
		wxByte GetG() const
		{
			return Green();
		}
		wxByte GetB() const
		{
			return Blue();
		}
		wxByte GetA() const
		{
			return Alpha();
		}
		
		KxColor& SetR(wxByte value)
		{
			Set(value, GetG(), GetB(), GetA());
			return *this;
		}
		KxColor& SetG(wxByte value)
		{
			Set(GetR(), value, GetB(), GetA());
			return *this;
		}
		KxColor& SetB(wxByte value)
		{
			Set(GetR(), GetB(), value, GetA());
			return *this;
		}
		KxColor& SetA(wxByte value)
		{
			Set(GetR(), GetG(), GetB(), value);
			return *this;
		}

		wxUint32 GetRGBA() const;
		KxColor& SetRGBA(wxUint32 color);
		wxUint32 GetARGB() const;
		KxColor& SetARGB(wxUint32 color);
		wxUint32 GetCOLORREF() const;
		KxColor& SetCOLORREF(wxUint32 color);
		
		void GetHSV(double& H, double& S, double& V, double* A = NULL) const;
		void GetHSV(int& H, int& S, int& V, int* A = NULL) const;
		KxColor& SetHSV(double H, double S, double V, double A = ALPHA_OPAQUE_PERCENT);
		
		void GetHSB(double& H, double& S, double& B, double* A = NULL) const;
		void GetHSB(int& H, int& S, int& B, int* A = NULL) const;
		KxColor& SetHSB(double H, double S, double B, double A = ALPHA_OPAQUE_PERCENT);
};

// Standard colors names
/*
AQUAMARINE
BLACK
BLUE
BLUE VIOLET
BROWN
CADET BLUE
CORAL
CORNFLOWER BLUE
CYAN
DARK GREY
DARK GREEN
DARK OLIVE GREEN
DARK ORCHID
DARK SLATE BLUE
DARK SLATE GREY
DARK TURQUOISE
DIM GREY	FIREBRICK
FOREST GREEN
GOLD
GOLDENROD
GREY
GREEN
GREEN YELLOW
INDIAN RED
KHAKI
LIGHT BLUE
LIGHT GREY
LIGHT STEEL BLUE
LIME GREEN
MAGENTA
MAROON
MEDIUM AQUAMARINE
MEDIUM BLUE	MEDIUM FOREST GREEN
MEDIUM GOLDENROD
MEDIUM ORCHID
MEDIUM SEA GREEN
MEDIUM SLATE BLUE
MEDIUM SPRING GREEN
MEDIUM TURQUOISE
MEDIUM VIOLET RED
MIDNIGHT BLUE
NAVY
ORANGE
ORANGE RED
ORCHID
PALE GREEN
PINK
PLUM
PURPLE	RED
SALMON
SEA GREEN
SIENNA
SKY BLUE
SLATE BLUE
SPRING GREEN
STEEL BLUE
TAN
THISTLE
TURQUOISE
VIOLET
VIOLET RED
WHEAT
WHITE
YELLOW
YELLOW GREEN
*/
