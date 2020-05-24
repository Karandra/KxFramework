#pragma once
#include "Common.h"
#include "String.h"

namespace kxf::Math
{
	wxUniChar GetDecimalSeparator();
	wxUniChar GetDecimalCSeparator();

	double GetPI();
	double GetHalfPI();

	double Round(double x, int roundTo = 0);
	double Truncate(double x, int truncateTo = 0);

	double Log(double x, int base);
	double LdExp(double mantissa, int64_t exp, int base);
	double FrExp(double x, int* exp, int base);

	double Sec(double x);
	double Csc(double x);
	double Cot(double x);
	double ACot(double x);
	double Sinh(double x);
	double Cosh(double x);
	double Tanh(double x);
	double Coth(double x);
	double ASinh(double x);
	double ACosh(double x);
	double ATanh(double x);
	double ACoth(double x);

	int64_t RandomInt(int64_t start, int64_t end);
	int64_t RandomInt(int64_t end);
	double RandomFloat();
	double RandomFloat(double start, double end);
	double RandomFloat(double end);

	double Map(double x, double inMin, double inMax, double outMin, double outMax);

	std::optional<int64_t> FromBase(const String& x, int base = 10);
	String ToBase(int64_t x, int base = 10);

	int Classify(double x);
	bool IsFinite(double x);
	bool IsInf(double x);
	bool IsNan(double x);
	bool IsNormal(double x);
	bool SignBit(double x);
};
