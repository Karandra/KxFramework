#pragma once
#include "Common.h"
#include "String.h"

namespace kxf::Math
{
	KX_API wxUniChar GetDecimalSeparator();
	KX_API wxUniChar GetDecimalCSeparator();

	KX_API double GetPI();
	KX_API double GetHalfPI();

	KX_API double Round(double x, int roundTo = 0);
	KX_API double Truncate(double x, int truncateTo = 0);

	KX_API double Log(double x, int base);
	KX_API double LdExp(double mantissa, int64_t exp, int base);
	KX_API double FrExp(double x, int* exp, int base);

	KX_API double Sec(double x);
	KX_API double Csc(double x);
	KX_API double Cot(double x);
	KX_API double ACot(double x);
	KX_API double Sinh(double x);
	KX_API double Cosh(double x);
	KX_API double Tanh(double x);
	KX_API double Coth(double x);
	KX_API double ASinh(double x);
	KX_API double ACosh(double x);
	KX_API double ATanh(double x);
	KX_API double ACoth(double x);

	KX_API int64_t RandomInt(int64_t start, int64_t end);
	KX_API int64_t RandomInt(int64_t end);
	KX_API double RandomFloat();
	KX_API double RandomFloat(double start, double end);
	KX_API double RandomFloat(double end);

	KX_API double Map(double x, double inMin, double inMax, double outMin, double outMax);

	KX_API std::optional<int64_t> FromBase(const String& x, int base = 10);
	KX_API String ToBase(int64_t x, int base = 10);

	KX_API int Classify(double x);
	KX_API bool IsFinite(double x);
	KX_API bool IsInf(double x);
	KX_API bool IsNan(double x);
	KX_API bool IsNormal(double x);
	KX_API bool SignBit(double x);
};
