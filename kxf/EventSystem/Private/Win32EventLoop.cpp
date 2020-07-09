#include "stdafx.h"
#include "Win32EventLoop.h"
#include "kxf/General/ICoreApplication.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include <Windows.h>

namespace kxf::EventSystem::Private
{
	bool Win32EventLoop::IsWakeUpRequested()
	{
		return ::WaitForSingleObject(m_WakeUpCondition.GetHandle(), 0) == WAIT_OBJECT_0;
	}
	uint32_t Win32EventLoop::WaitForThread(void* threadHandle)
	{
		// The order is important here, the code using this function assumes that 'WAIT_OBJECT_0'
		// indicates the thread termination and anything else - the availability of an input event.
		// So the thread handle must come first.
		// Wait for any objects, not all of them. Without timeout and return as soon as there are any events (QS_*).

		const HANDLE handles[2] = {threadHandle, m_WakeUpCondition.GetHandle()};
		return ::MsgWaitForMultipleObjects(std::size(handles), handles, FALSE, INFINITE, QS_ALLINPUT|QS_ALLPOSTMESSAGE);
	}

	bool Win32EventLoop::GetNextMessage(tagMSG& message)
	{
		return GetNextMessage(message, TimeSpan::Milliseconds(INFINITE)) == DispatchTimeout::Success;
	}
	IEventLoop::DispatchTimeout Win32EventLoop::GetNextMessage(tagMSG& message, TimeSpan timeout)
	{
		// 'MsgWaitForMultipleObjects' won't notice any input which was already examined (e.g. using 'PeekMessageW')
		// but not yet removed from the queue so we need to remove any immediate messages manually.

		while (!::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			const HANDLE handle = m_WakeUpCondition.GetHandle();
			const uint32_t waitStatus = ::MsgWaitForMultipleObjects(1, &handle, FALSE, timeout.GetMilliseconds(), QS_ALLINPUT|QS_ALLPOSTMESSAGE);

			switch (waitStatus)
			{
				case WAIT_TIMEOUT:
				{
					return DispatchTimeout::Expired;
				}
				case WAIT_OBJECT_0:
				{
					// We were woken up by a background thread, which means there is no actual input message available,
					// but we should still return to the event loop, so pretend there was 'WM_NULL' in the queue.
					message = {};
					message.message = WM_NULL;

					return DispatchTimeout::Success;
				}
				case WAIT_OBJECT_0 + 1:
				{
					// Some message is supposed to be available, but spurious wake-ups are also possible,
					// so just return to the loop: either we'll get the message or start waiting again.
					break;
				}
				default:
				{
					wxLogDebug("unexpected 'MsgWaitForMultipleObjects' return value '%lu'", static_cast<unsigned long>(waitStatus));
					break;
				}
			};
		}

		if (message.message != WM_QUIT)
		{
			return DispatchTimeout::Success;
		}
		return DispatchTimeout::ShouldExit;
	}

	void Win32EventLoop::WakeUp()
	{
		m_WakeUpCondition.Signal();
	}
	bool Win32EventLoop::Pending()
	{
		MSG msg;
		return ::PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
	}
}
