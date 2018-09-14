#pragma once
#include "KxFramework/KxFramework.h"
class KxProgressBar;

class KxIProgressBar
{
	protected:
		virtual int DoGetRange() const = 0;
		virtual void DoSetRange(int range) = 0;

		virtual int DoGetValue() const = 0;
		virtual void DoSetValue(int value) = 0;

		virtual int DoGetStep() const = 0;
		virtual void DoSetStep(int step) = 0;

		virtual void DoPulse() = 0;
		virtual bool DoIsPulsing() const = 0;

	public:
		virtual ~KxIProgressBar() = default;

	public:
		int GetRange() const
		{
			return DoGetRange();
		}
		void SetRange(int range)
		{
			DoSetRange(range);
		}

		int GetValue() const
		{
			return DoGetValue();
		}
		void SetValue(int value)
		{
			DoSetValue(value);
		}
		void SetValue(int64_t current, int64_t max)
		{
			const int range = DoGetRange();

			int value = 0;
			if (max >= current)
			{
				value = range;
			}
			else if (max > 0)
			{
				value = ((double)current / (double)max) * range;
			}
			DoSetValue(value);
		}

		void Advance(int value)
		{
			const int max = DoGetRange();
			value += DoGetValue();

			DoSetValue(value <= max ? value : max);
		}
		void StepIt()
		{
			Advance(DoGetStep());
		}

		void Pulse()
		{
			DoPulse();
		}
		bool IsPulsing() const
		{
			return DoIsPulsing();
		}
};
