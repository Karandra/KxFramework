#pragma once
#include "kxf/EventSystem/Event.h"

namespace kxf::Threading::Private
{
	template<class T>
	static void AssertThreadExitCode()
	{
		static_assert(std::is_pointer_v<T> || std::is_integral_v<T> || std::is_enum_v<T>);
	}
}

namespace kxf
{
	class ThreadEvent: public CommonEvent
	{
		public:
			KxEVENT_MEMBER(ThreadEvent, Execute);
			KxEVENT_MEMBER(ThreadEvent, Started);
			KxEVENT_MEMBER(ThreadEvent, Finished);

		private:
			void* m_ExitCode = nullptr;

		public:
			ThreadEvent() = default;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ThreadEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				// This is important to avoid that calling 'IEventLoop::YieldFor' thread events gets processed when this is unwanted.
				return EventCategory::Thread;
			}

		public:
			template<class T = void*>
			T GetExitCode() const
			{
				Threading::Private::AssertThreadExitCode<T>();

				return static_cast<T>(m_ExitCode);
			}
			
			template<class T>
			void SetExitCode(T code)
			{
				Threading::Private::AssertThreadExitCode<T>();

				m_ExitCode = static_cast<void*>(code);
			}
	};
}
