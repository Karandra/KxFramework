#pragma once
#include "Common.h"
#include "IEventLoop.h"

namespace kxf
{
	class KX_API CommonEventLoop: public RTTI::ImplementInterface<CommonEventLoop, IEventLoop>
	{
		private:
			size_t m_YieldLevel = 0;
			FlagSet<EventCategory> m_AllowedToYield;

			int m_ExitCode = -1;
			bool m_IsInsideRun = false;
			bool m_ShouldExit = false;

		private:
			bool DispatchEvents();

		protected:
			int GetExitCode() const noexcept
			{
				return m_ExitCode;
			}
			bool IsInsideRun() const
			{
				return m_IsInsideRun;
			}
			bool ShouldExit() const
			{
				return m_ShouldExit;
			}

			void OnYieldFor(FlagSet<EventCategory> toProcess) override = 0;
			int OnRun() override;

		public:
			int Run() override;
			void Exit(int exitCode = 0) override;
			void ScheduleExit(int exitCode = 0) override;

			bool DispatchIdle() override;

		public:
			bool IsYielding() const override
			{
				return m_YieldLevel != 0;
			}
			bool IsEventAllowedInsideYield(FlagSet<EventCategory> eventCategory) const override
			{
				return m_AllowedToYield.Contains(eventCategory);
			}

			bool Yield(FlagSet<EventYieldFlag> flags) override;
			bool YieldFor(FlagSet<EventCategory> toProcess) override;
	};
}
