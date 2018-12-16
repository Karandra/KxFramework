#include "KxStdAfx.h"
#include "KxFramework/KxColor.h"

const double KxColor::SRGB_WeightR = 0.2126;
const double KxColor::SRGB_WeightG = 0.7152;
const double KxColor::SRGB_WeightB = 0.0722;

double KxColor::HUE2RGB(double p, double q, double t)
{
	if (t < 0)
	{
		t += 1.0;
	}
	if (t > 1)
	{
		t -= 1.0;
	}
	if (t < 1.0/6.0)
	{
		return p + (q - p) * 6.0 * t;
	}
	if (t < 1.0/2.0)
	{
		return q;
	}
	if (t < 2.0/3.0)
	{
		return p + (q - p) * (2.0/3.0 - t) * 6.0;
	}
	return p;
}
uint8_t KxColor::GetAlphaCOLORREF(uint32_t rgb)
{
	return LOBYTE((rgb >> 24));
}
uint32_t KxColor::MakeCOLORREF(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
	return ((DWORD)A << 24) | RGB(R, G, B);
}

KxColor::KxColor(const wxColour& other)
	:wxColour(other)
{
}
KxColor::KxColor(const KxColor& other)
	:wxColour()
{
	if (other.IsOk())
	{
		Set(other.GetR(), other.GetG(), other.GetB(), other.GetA());
	}
}

bool KxColor::IsOk() const
{
	return wxColour::IsOk();
}
KxColor KxColor::Clone() const
{
	return KxColor(GetR(), GetG(), GetB(), GetA());
}
wxString KxColor::GetAsString(long mode) const
{
	if (mode == (long)ToString::LuaSyntax)
	{
		return KxString::Format(wxS("{R = %1, G = %2, B = %3, A = %4}"), GetR(), GetG(), GetB(), GetA());
	}
	return wxColour::GetAsString(mode);
}

KxColor& KxColor::AlphaBlend(const KxColor& source)
{
	double A = GetA() / 255.0;
	Set
	(
		wxColour::AlphaBlend(GetR(), source.GetR(), A),
		wxColour::AlphaBlend(GetG(), source.GetG(), A),
		wxColour::AlphaBlend(GetB(), source.GetB(), A),
		wxColour::AlphaBlend(GetA(), source.GetA(), A)
	);
	return *this;
}
KxColor& KxColor::MakeMono(bool bOn)
{
	uint8_t R = Red();
	uint8_t G = Green();
	uint8_t B = Blue();

	wxColour::MakeMono(&R, &G, &G, bOn);
	Set(R, G, B, Alpha());

	return *this;
}
KxColor& KxColor::MakeGray(double weightR, double weightG, double weightB)
{
	uint8_t R = Red();
	uint8_t G = Green();
	uint8_t B = Blue();

	wxColour::MakeGrey(&R, &G, &G, weightR, weightG, weightB);
	Set(R, G, B, Alpha());
	return *this;
}
KxColor& KxColor::MakeGray()
{
	uint8_t R = Red();
	uint8_t G = Green();
	uint8_t B = Blue();

	wxColour::MakeGrey(&R, &G, &G);
	Set(R, G, B, Alpha());
	return *this;
}
KxColor& KxColor::ChangeLightness(int alpha)
{
	wxColour color = wxColour::ChangeLightness(alpha);
	Set(color.Red(), color.Green(), color.Blue(), color.Alpha());
	return *this;
}
KxColor& KxColor::RotateHue(double angle)
{
	double H, S, V = 0;
	GetHSV(H, S, V);

	H += angle;
	if (H > 1.0)
	{
		H -= 1.0;
	}
	else if (H < 0.0)
	{
		H += 1.0;
	}
	SetHSV(H, S, V);
	return *this;
}
KxColor& KxColor::Invert()
{
	double H, S, B, A = 0;
	GetHSB(H, S, B, &A);
	H = (int)(H + 180) % 360;
	SetHSB(H, S, B, A);

	return *this;
}
KxColor& KxColor::Negate()
{
	SetRGBA(0xFFFFFF00u ^ GetRGBA());
	return *this;
}

uint32_t KxColor::GetRGBA() const
{
	return ((GetR() & 0xff) << 24) + ((GetG() & 0xff) << 16) + ((GetB() & 0xff) << 8) + (GetA() & 0xff);
}
KxColor& KxColor::SetRGBA(uint32_t color)
{
	if (((color >> 24) & 0xFF) == 0)
	{
		Set((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, wxALPHA_OPAQUE);
	}
	else
	{
		Set((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
	}
	return *this;
}
uint32_t KxColor::GetARGB() const
{
	return ((GetA() & 0xff) << 24) + ((GetR() & 0xff) << 16) + ((GetG() & 0xff) << 8) + (GetB() & 0xff);
}
KxColor& KxColor::SetARGB(uint32_t color)
{
	uint8_t R = ((color >> 16) & 0xff);
	uint8_t G = ((color >> 8) & 0xff);
	uint8_t B = (color & 0xff);
	uint8_t A = ((color >> 24) & 0xff);

	Set(R, G, B, A);
	return *this;
}
uint32_t KxColor::GetCOLORREF() const
{
	return MakeCOLORREF(GetR(), GetG(), GetB(), GetA());
}
KxColor& KxColor::SetCOLORREF(uint32_t color)
{
	Set(GetRValue(color), GetGValue(color), GetBValue(color), GetAlphaCOLORREF(color));
	return *this;
}

void KxColor::GetHSV(double& H, double& S, double& V, double* A) const
{
	double R1 = GetR()/255.0;
	double G1 = GetG()/255.0;
	double B1 = GetB()/255.0;
	double A1 = GetA()/255.0;

	double max = std::max(std::max(R1, G1), B1);
	double min = std::min(std::min(R1, G1), B1);
	double nDelta = max-min;

	// H
	H = 0;
	if (max == R1 && G1 >= B1)
	{
		H = 60.0*(((G1 - B1)/nDelta));
	}
	else if (max == R1 && G1 < B1)
	{
		H = 60.0*((((G1 - B1)/nDelta)))+360.0;
	}
	else if (max == G1)
	{
		H = 60.0*((((B1 - R1)/nDelta)))+120.0;
	}
	else if (max == B1)
	{
		H = 60.0*((((R1-G1)/nDelta)))+240.0;
	}

	// S
	S = (nDelta/(1.0-abs((1.0-(max+min)))))*100.0;

	// L
	V = (0.5*(max+min))*100.0;

	// A
	if (A)
	{
		*A = A1*100.0;
	}
}
void KxColor::GetHSV(int& H, int& S, int& V, int* A) const
{
	double H1, S1, V1, A1 = 0;
	GetHSV(H1, S1, V1, &A1);

	H = (int)H1;
	S = (int)S1;
	V = (int)V1;

	if (A)
	{
		*A = (int)A1;
	}
}
KxColor& KxColor::SetHSV(double H, double S, double V, double A)
{
	H = H/360.0;
	S = S/100.0;
	V = V/100.0;
	A = A/100.0;

	double R, G, B;
	if (S == 0)
	{
		R = V;
		G = V;
		B = V;
	}
	else
	{
		double q = V < 0.5 ? V * (1.0 + S) : V + S - V * S;
		double p = 2.0 * V - q;
		R = HUE2RGB(p, q, H + (1.0/3.0));
		G = HUE2RGB(p, q, H);
		B = HUE2RGB(p, q, H - 1.0/3.0);
	}

	Set(R*255, G*255, B*255, A*255);
	return *this;
}

void KxColor::GetHSB(double& H, double& S, double& B, double* A) const
{
	double R1 = GetR()/255.0;
	double G1 = GetG()/255.0;
	double B1 = GetB()/255.0;
	double A1 = GetA()/255.0;

	double max = std::max(std::max(R1, G1), B1);
	double min = std::min(std::min(R1, G1), B1);
	double nDelta = max-min;

	// H
	H = 0;
	if (max == R1 && G1 >= B1)
	{
		H = 60.0*(((G1 - B1)/nDelta));
	}
	else if (max == R1 && G1 < B1)
	{
		H = 60.0*((((G1 - B1)/nDelta)))+360.0;
	}
	else if (max == G1)
	{
		H = 60.0*((((B1 - R1)/nDelta)))+120.0;
	}
	else if (max == B1)
	{
		H = 60.0*((((R1-G1)/nDelta)))+240.0;
	}

	// S
	S = 0;
	if (max != 0.0)
	{
		S = (1.0-(min/max))*100.0;
	}

	// B
	B = max*100.0;

	// A
	if (A)
	{
		*A = A1*100.0;
	}
}
void KxColor::GetHSB(int& H, int& S, int& B, int* A) const
{
	double H1, S1, B1, A1 = 0;
	GetHSV(H1, S1, B1, &A1);

	H = (int)H1;
	S = (int)S1;
	B = (int)B1;

	if (A)
	{
		*A = (int)A1;
	}
}
KxColor& KxColor::SetHSB(double H, double S, double B, double A)
{
	double n;
	wxByte Hi = floor(H/60.0);
	std::modf(Hi/6.0, &n);
	Hi = (wxByte)n;

	double Bmin = ((100.0-S)*B)/100.0;
	std::modf(H/60.0, &n);
	double a = (B-Bmin)*((n)/60.0);
	double Binc = Bmin+a;
	double Bdec = B-a;

	const double tRGB[][3] =
	{
		{B, Binc, Bmin},
		{Bdec, B, Bmin},
		{Bmin, B, Binc},
		{Bmin, Bdec, B},
		{Binc, Bmin, B},
		{B, Bmin, Bdec},
	};
	double R1 = tRGB[Hi][0];
	double G1 = tRGB[Hi][1];
	double B1 = tRGB[Hi][2];

	Set(R1*2.55, G1*2.55, B1*2.55, A*2.55);
	return *this;
}
