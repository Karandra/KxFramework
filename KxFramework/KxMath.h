#pragma once
#include "KxFramework/KxFramework.h"

class KxMath
{
	private:
		static const double PI;
		static const double PI_2;
		static std::vector<double> FactorialValues;

	private:
		static void Fibonacci(int64_t x, double* u, double* v);
		static char GetDecimalSeparator()
		{
			return std::use_facet<std::numpunct<char>>(std::cout.getloc()).decimal_point();
		}
		template<class T = double> static constexpr T GetPI()
		{
			return std::acos(-T(1));
		}
		template<class T = double> static constexpr T GetPI_2()
		{
			return std::acos(T(0));
		}

	public:
		static double Round(double x, int64_t roundTo = 0);
		static double Truncate(double x, int64_t truncateTo = 0);

		static double Log(double x, int64_t base = 2);
		static double LdExp(double mantissa, int64_t exp, int64_t base = 2);
		static double FrExp(double x, int* exp, int64_t base = 2);
		static double Exp2(double x);

		static double Sec(double x);
		static double Csc(double x);
		static double Cot(double x);
		static double ACot(double x);
		static double Sinh(double x);
		static double Cosh(double x);
		static double Tanh(double x);
		static double Coth(double x);
		static double ASinh(double x);
		static double ACosh(double x);
		static double ATanh(double x);
		static double ACoth(double x);

		static int64_t RandomInt(int64_t start, int64_t end);
		static int64_t RandomInt(int64_t end);
		static double RandomFloat();
		static double RandomFloat(double start, double end);
		static double RandomFloat(double end);

		static double Map(double x, double inMin, double inMax, double outMin, double outMax);

		static int64_t FromBase(const wxString& x, int64_t base = 10);
		static wxString ToBase(int64_t x, int64_t base = 10);

		static double Factorial(int64_t x);
		static double Fibonacci(int64_t x);

		static int Classify(double x);
		static bool IsFinite(double x);
		static bool IsInf(double x);
		static bool IsNan(double x);
		static bool IsNormal(double x);
		static bool SignBit(double x);
};
