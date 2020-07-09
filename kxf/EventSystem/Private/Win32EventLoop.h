#pragma once
#include "../Common.h"
#include "../CommonEventLoop.h"
#include "kxf/Threading/SynchronizedCondition.h"
struct tagMSG;

namespace kxf::EventSystem::Private
{
	class KX_API Win32EventLoop: public CommonEventLoop
	{
		private:
			SynchronizedCondition m_WakeUpCondition;

		protected:
			// Return true if wake up was requested and not handled yet, i.e. if 'm_WakeUpEvent' is signaled.
			bool IsWakeUpRequested();

			// A method to wait for the termination of the specified (by its native handle) thread or any input
			// message arriving (in GUI case). Return value is 'WAIT_OBJECT_0' if the thread terminated, 'WAIT_OBJECT_0 + 1'
			// if a message arrived with anything else indicating an error.
			uint32_t WaitForThread(void* threadHandle);

			// Get the next message from queue and return true or return false if we got 'WM_QUIT' or an error occurred.
			bool GetNextMessage(tagMSG& message);

			// Same as above but with a timeout and return value can be -1 meaning that time out expired in addition to true/false
			DispatchTimeout GetNextMessage(tagMSG& message, TimeSpan timeout);

		public:
			Win32EventLoop() = default;

		public:
			void WakeUp() override;
			bool Pending() override;
	};
}
