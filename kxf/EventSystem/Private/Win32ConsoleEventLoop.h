#pragma once
#include "../Common.h"
#include "Win32EventLoop.h"

namespace kxf::EventSystem::Private
{
	class KX_API Win32ConsoleEventLoop: public Win32EventLoop
	{
		protected:
			void OnYieldFor(FlagSet<EventCategory> toProcess) override
			{
				Win32EventLoop::OnYieldFor(toProcess);
			}
			
			bool PreProcessMessage(Win32Message& message) override
			{
				return false;
			}
			void ProcessMessage(Win32Message& message) override;

		public:
			Win32ConsoleEventLoop() = default;

		public:
			bool Dispatch() override;
			DispatchTimeout Dispatch(TimeSpan timeout) override;
	};
}
