#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "ITimeClock.h"

namespace kxf
{
	class SystemClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class SteadyClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class HighResolutionClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	#if __cplusplus > 201703L
	class FileClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class UTCClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class GPSClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class AtomicTimeClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};
	#endif
}
