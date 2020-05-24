#pragma once
#include "BasicEvtHandler.h"
#include <wx/window.h>

namespace kxf
{
	class KX_API EvtHandler: public BasicEvtHandler
	{
		private:
			wxEvtHandler m_EvtHandler;

		public:
			EvtHandler()
				:BasicEvtHandler(m_EvtHandler)
			{
			}
			EvtHandler(const EvtHandler&) = delete;

		public:
			wxEvtHandler& GetTargetHandler() override
			{
				return m_EvtHandler;
			}

		public:
			EvtHandler& operator=(const EvtHandler&) = delete;
	};

	class KX_API KxRefEvtHandler: public BasicEvtHandler
	{
		private:
			wxEvtHandler& m_EvtHandler;

		public:
			KxRefEvtHandler(wxEvtHandler* evtHandler)
				:BasicEvtHandler(*evtHandler), m_EvtHandler(*evtHandler)
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

	class KX_API KxWindowEvtHandler: public BasicEvtHandler
	{
		private:
			wxWindow& m_Window;

		public:
			KxWindowEvtHandler(wxWindow* window)
				:BasicEvtHandler(*window), m_Window(*window)
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
}
