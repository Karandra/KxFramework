#pragma once
#include "IEvtHandler.h"
#include <wx/window.h>

class KX_API KxEvtHandler: public Kx::EventSystem::IEvtHandler
{
	private:
		wxEvtHandler m_EvtHandler;

	public:
		KxEvtHandler()
			:IEvtHandler(m_EvtHandler)
		{
		}
		KxEvtHandler(const KxEvtHandler&) = delete;

	public:
		wxEvtHandler& GetTargetHandler() override
		{
			return m_EvtHandler;
		}

	public:
		KxEvtHandler& operator=(const KxEvtHandler&) = delete;
};

class KX_API KxRefEvtHandler: public Kx::EventSystem::IEvtHandler
{
	private:
		wxEvtHandler& m_EvtHandler;

	public:
		KxRefEvtHandler(wxEvtHandler* evtHandler)
			:IEvtHandler(*evtHandler), m_EvtHandler(*evtHandler)
		{
		}
		KxRefEvtHandler(const KxRefEvtHandler&) = delete;

	public:
		wxEvtHandler& GetTargetHandler() override
		{
			return m_EvtHandler;
		}

	public:
		KxRefEvtHandler& operator=(const KxRefEvtHandler&) = delete;
};

class KX_API KxWindowEvtHandler: public Kx::EventSystem::IEvtHandler
{
	private:
		wxWindow& m_Window;

	public:
		KxWindowEvtHandler(wxWindow* window)
			:IEvtHandler(*window), m_Window(*window)
		{
		}
		KxWindowEvtHandler(const KxWindowEvtHandler&) = delete;

	public:
		wxEvtHandler& GetTargetHandler() override
		{
			return *m_Window.GetEventHandler();
		}

	public:
		KxWindowEvtHandler& operator=(const KxWindowEvtHandler&) = delete;
};
