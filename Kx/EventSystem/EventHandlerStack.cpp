#include "KxStdAfx.h"
#include "EventHandlerStack.h"

bool KxEvtHandlerStack::Push(wxEvtHandler& evtHandler)
{
	// New handler can't be part of another chain
	if (evtHandler.IsUnlinked())
	{
		evtHandler.SetNextHandler(m_Last);
		m_Last->SetPreviousHandler(&evtHandler);
		m_Last = &evtHandler;

		return true;
	}
	return false;
}
bool KxEvtHandlerStack::Remove(wxEvtHandler& evtHandler)
{
	// Short circuit for last handler
	if (&evtHandler == m_Last)
	{
		return Pop() != nullptr;
	}

	// Check if this handler is part of any chain
	if (!evtHandler.IsUnlinked())
	{
		// Is it part of our chain?
		wxEvtHandler* unlinked = ForEachItem<Order::LastToFirst>([&evtHandler](wxEvtHandler& chainItem)
		{
			// Unlink it
			if (&chainItem == &evtHandler)
			{
				chainItem.Unlink();
				return false;
			}
			return true;
		});
		return unlinked != nullptr;
	}
	return false;
}
wxEvtHandler* KxEvtHandlerStack::Pop()
{
	// We need to pop the stack, i.e. we need to remove the latest added handler
	wxEvtHandler* topHandler = m_Last;

	// We can't pop if we have only one item and the top handler should have no previous handlers set
	if (topHandler != m_First && !topHandler->GetPreviousHandler())
	{
		// The second handler should have non-null next handler
		if (wxEvtHandler* nextHandler = topHandler->GetNextHandler())
		{
			topHandler->SetNextHandler(nullptr);
			nextHandler->SetPreviousHandler(nullptr);

			// Now top handler is completely unlinked, set next handler as the new current handler
			m_Last = nextHandler;

			// And return the popped one
			return topHandler;
		}
	}
	return nullptr;
}
