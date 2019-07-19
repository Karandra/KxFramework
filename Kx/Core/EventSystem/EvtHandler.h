#pragma once
#include "BasicEvtHandler.h"
#include <wx/window.h>

class KX_API KxEvtHandler: public KxBasicEvtHandler
{
	private:
		wxEvtHandler m_EvtHandler;

	public:
		KxEvtHandler()
			:KxBasicEvtHandler(m_EvtHandler)
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

class KX_API KxRefEvtHandler: public KxBasicEvtHandler
{
	private:
		wxEvtHandler& m_EvtHandler;

	public:
		KxRefEvtHandler(wxEvtHandler* evtHandler)
			:KxBasicEvtHandler(*evtHandler), m_EvtHandler(*evtHandler)
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

class KX_API KxWindowEvtHandler: public KxBasicEvtHandler
{
	private:
		wxWindow& m_Window;

	public:
		KxWindowEvtHandler(wxWindow* window)
			:KxBasicEvtHandler(*window), m_Window(*window)
		{
		}
		KxWindowEvtHandler(const KxWindowEvtHandler&) = delete;

	public:
		wxEvtHandler& GetTargetHandler() override
		{
			return *m_Window.GetEventHandler();
		}
		const wxWindow& GetWindow() const
		{
			return m_Window;
		}
		wxWindow& GetWindow()
		{
			return m_Window;
		}

		void SetPrevHandler(wxEvtHandler* evtHandler) = delete;
		void SetNextHandler(wxEvtHandler* evtHandler) = delete;

		void PushEventHandler(wxEvtHandler& evtHandler)
		{
			m_Window.PushEventHandler(&evtHandler);
		}
		bool RemoveEventHandler(wxEvtHandler& evtHandler)
		{
			return m_Window.RemoveEventHandler(&evtHandler);
		}
		wxEvtHandler* PopEventHandler()
		{
			return m_Window.PopEventHandler(false);
		}

	public:
		KxWindowEvtHandler& operator=(const KxWindowEvtHandler&) = delete;
};
