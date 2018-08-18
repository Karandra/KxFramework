#include "KxStdAfx.h"
#include "KxFramework/KxMath.h"
#include <random>
#include <cmath>

const double KxMath::PI = KxMath::GetPI();
const double KxMath::PI_2 = KxMath::GetPI_2();
std::vector<double> KxMath::FactorialValues = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800};

void KxMath::Fibonacci(int64_t x, double* u, double* v)
{
	if (x == 1)
	{
		*u = 1;
		*v = 1;
	}
	else
	{
		double a;
		double b;
		Fibonacci(x-1, &a, &b);
		*u = b;
		*v = a+b;
	}
}

double KxMath::Round(double x, int64_t roundTo)
{
	if (roundTo <= 0)
	{
		return (int64_t)x;
	}
	else
	{
		double n10r = std::pow(10, roundTo);
		return std::round(x * n10r) / n10r;
	}
}
double KxMath::Truncate(double x, int64_t truncateTo)
{
	if (truncateTo > 0)
	{
		wxString buffer = wxString::Format(wxString::Format("%%.%dlf", DBL_DIG), x);
		int pointPos = buffer.Find(GetDecimalSeparator());

		if ((size_t)pointPos != wxString::npos)
		{
			buffer.Truncate(pointPos+1+truncateTo);
			if (buffer.ToDouble(&x))
			{
				return x;
			}
		}
		return x;
	}
	return (int64_t)x;
}

double KxMath::Log(double x, int64_t base)
{
	switch (base)
	{
		case 2:
		{
			return std::log(x);
		}
		case 10:
		{
			return std::log10(x);
		}
		default:
		{
			return std::log(x) / std::log(base);
		}
	};
}
double KxMath::LdExp(double mantissa, int64_t exp, int64_t base)
{
	if (base == 2)
	{
		return std::ldexp(mantissa, exp);
	}
	else
	{
		return mantissa * std::pow(base, exp);
	}
}
double KxMath::FrExp(double x, int* exp, int64_t base)
{
	switch (base)
	{
		case 2:
		{
			return std::frexp(x, exp);
		}
		case 10:
		{
			*exp = (int)std::log10(x);
			return std::pow(x / 10.0, *exp);
		}
		default:
		{
			*exp = (int)Log(x, base);
			return std::pow(x / (double)base, *exp);
		}
	};
}
double KxMath::Exp2(double x)
{
	return std::exp2(x);
}

double KxMath::Sec(double x)
{
	return 1.0 / std::cos(x);
}
double KxMath::Csc(double x)
{
	return 1.0 / std::sin(x);
}
double KxMath::Cot(double x)
{
	return 1.0 / std::tan(x);
}
double KxMath::ACot(double x)
{
	return PI_2 - std::atan(x);
}
double KxMath::Sinh(double x)
{
	return std::sinh(x);
}
double KxMath::Cosh(double x)
{
	return std::cosh(x);
}
double KxMath::Tanh(double x)
{
	return std::tanh(x);
}
double KxMath::Coth(double x)
{
	return (std::exp(x) + std::exp(-x)) / (std::exp(x) - std::exp(-x));
}
double KxMath::ASinh(double x)
{
	return std::asinh(x);
}
double KxMath::ACosh(double x)
{
	return std::acosh(x);
}
double KxMath::ATanh(double x)
{
	return std::atanh(x);
}
double KxMath::ACoth(double x)
{
	return ATanh(1.0 / x);
}

int64_t KxMath::RandomInt(int64_t start, int64_t end)
{
	double value = RandomFloat();
	value *= (double)(end - start) + 1.0;
	return (int64_t)value + start;
}
int64_t KxMath::RandomInt(int64_t end)
{
	return RandomInt(0, end);
}
double KxMath::RandomFloat()
{
	return (double)std::rand() * (1.0 / ((double)RAND_MAX + 1.0));
}
double KxMath::RandomFloat(double start, double end)
{
	double x = RandomInt(start, end)*RandomFloat();
	while (x < start || x > end)
	{
		x = RandomInt(start, end) * RandomFloat();
	}
	return x;
}
double KxMath::RandomFloat(double end)
{
	return RandomFloat(0, end);
}

double KxMath::Map(double x, double inMin, double inMax, double outMin, double outMax)
{
	return (x - inMin) * (outMax - outMin)/(inMax - inMin) + outMin;
}

int64_t KxMath::FromBase(const wxString& x, int64_t base)
{
	return std::strtol(x, NULL, base);
}
wxString KxMath::ToBase(int64_t x, int64_t base)
{
	if (base == 10)
	{
		return wxString::Format("%lld", x);
	}
	else if (base <= 36)
	{
		if (x != 0)
		{
			// Fill symbols table
			char symbols[36] = {0};
			for (int i = 0; i < 36; i++)
			{
				if (i < 10)
				{
					symbols[i] = i+'0';
				}
				else
				{
					symbols[i] = i+'A'-10;
				}
			}

			wxString out;
			int64_t nReminder = 0;
			while (x != 0)
			{
				nReminder = std::fmod(x, base);
				x = x/base;

				out.Prepend(symbols[nReminder]);
			}

			return out;
		}
		return "0";
	}
	else
	{
		return wxEmptyString;
	}
}

double KxMath::Factorial(int64_t x)
{
	if (x < (int64_t)FactorialValues.size())
	{
		return FactorialValues[x];
	}
	else
	{
		int64_t size = FactorialValues.size();
		double value = FactorialValues[size-1];
		FactorialValues.reserve(x);

		for (int64_t i = size; i <= x; i++)
		{
			value *= i;
			FactorialValues.insert(FactorialValues.begin()+i, value);
		}
		return value;
	}
}
double KxMath::Fibonacci(int64_t x)
{
	double u = 1;
	double v = 1;
	Fibonacci(x, &u, &v);
	return v;
}

int KxMath::Classify(double x)
{
	return std::fpclassify(x);
}
bool KxMath::IsFinite(double x)
{
	return std::isfinite(x);
}
bool KxMath::IsInf(double x)
{
	return std::isinf(x);
}
bool KxMath::IsNan(double x)
{
	return std::isnan(x);
}
bool KxMath::IsNormal(double x)
{
	return std::isnormal(x);
}
bool KxMath::SignBit(double x)
{
	return std::signbit(x);
}
