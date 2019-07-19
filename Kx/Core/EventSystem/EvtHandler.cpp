#include "KxStdAfx.h"
#include "EvtHandler.h"
#include "CallWrapper.h"
#include "../Utility.h"
#include <wx/window.h>

KxEvtHandler::KxEvtHandler(wxEvtHandler* evtHandler)
	:m_EvtHandler(evtHandler)
{
}
KxEvtHandler::KxEvtHandler(wxWindow* window)
	:m_EvtHandler(window->GetEventHandler())
{
}

KxEvtHandler& KxEvtHandler::operator=(KxEvtHandler&& other)
{
	using Kx::Utility::ExchangeAndReset;

	ExchangeAndReset(m_EvtHandler, other.m_EvtHandler, nullptr);

	return *this;
}
