/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

namespace KxMath
{
	KX_API char GetDecimalSeparator();
	KX_API wchar_t GetDecimalSeparatorW();

	KX_API double GetPI();
	KX_API double GetPI_2();

	KX_API double Round(double x, int64_t roundTo = 0);
	KX_API double Truncate(double x, int64_t truncateTo = 0);

	KX_API double Log(double x, int64_t base = 2);
	KX_API double LdExp(double mantissa, int64_t exp, int64_t base = 2);
	KX_API double FrExp(double x, int* exp, int64_t base = 2);
	KX_API double Exp2(double x);

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

	KX_API int64_t FromBase(const wxString& x, int64_t base = 10);
	KX_API wxString ToBase(int64_t x, int64_t base = 10);

	KX_API double Factorial(int64_t x);
	KX_API double Fibonacci(int64_t x);

	KX_API int Classify(double x);
	KX_API bool IsFinite(double x);
	KX_API bool IsInf(double x);
	KX_API bool IsNan(double x);
	KX_API bool IsNormal(double x);
	KX_API bool SignBit(double x);
};
