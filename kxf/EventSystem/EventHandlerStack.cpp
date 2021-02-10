#include "stdafx.h"
#include "EventHandlerStack.h"

namespace kxf
{
	bool EvtHandlerStack::Push(IEvtHandler& evtHandler)
	{
		// New handler can't be part of another chain
		if (evtHandler.IsUnlinked())
		{
			evtHandler.SetNextHandler(m_Top);
			m_Top->SetPrevHandler(&evtHandler);
			m_Top = &evtHandler;

			return true;
		}
		return false;
	}
	bool EvtHandlerStack::Remove(IEvtHandler& evtHandler)
	{
		// Short circuit for the last handler
		if (&evtHandler == m_Top)
		{
			return Pop() != nullptr;
		}

		// Check if this handler is part of any chain
		if (!evtHandler.IsUnlinked())
		{
			// Is it part of our chain?
			for (IEvtHandler& chainItem: EnumItems(Order::LastToFirst))
			{
				// Unlink it
				if (&chainItem == &evtHandler)
				{
					chainItem.Unlink();
					return true;
				}
			};
		}
		return false;
	}
	IEvtHandler* EvtHandlerStack::Pop()
	{
		// We need to pop the stack, i.e. we need to remove the latest added handler
		IEvtHandler* topHandler = m_Top;

		// We can't pop if we have only one item and the top handler should have no previous handlers set
		if (topHandler != m_Base && !topHandler->GetPrevHandler())
		{
			// The second handler should have non-null next handler
			if (IEvtHandler* nextHandler = topHandler->GetNextHandler())
			{
				topHandler->SetNextHandler(nullptr);
				nextHandler->SetPrevHandler(nullptr);

				// Now top handler is completely unlinked, set next handler as the new current handler
				m_Top = nextHandler;

				// And return the popped one
				return topHandler;
			}
		}
		return nullptr;
	}
}
