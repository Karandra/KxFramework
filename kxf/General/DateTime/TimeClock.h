#pragma once
#include "Common.h"
#include "TimeSpan.h"
#include "ITimeClock.h"

namespace kxf
{
	class KX_API SystemClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KX_API SteadyClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KX_API HighResolutionClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	#if __cplusplus > 201703L
	class KX_API FileClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KX_API UTCClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KX_API GPSClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};

	class KX_API AtomicTimeClock final: public ITimeClock
	{
		public:
			TimeSpan Now() const noexcept override;
	};
	#endif
}
