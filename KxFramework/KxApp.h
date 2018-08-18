#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/cmdline.h>

namespace KxAppInternal
{
	bool ParseCommandLine(wxCmdLineParser& parser);
}

template<class BaseT = wxApp, class DerivedT = wxApp>
class KxApp: public BaseT
{
	public:
		static DerivedT& Get()
		{
			return *static_cast<DerivedT*>(wxApp::GetInstance());
		}

	private:
		wxCmdLineParser m_CmdLineParser;
		wxString m_Version;
		int m_ReturnCode = 0;
		bool m_IsReturnCodeSet = false;

	private:
		BaseT* GetThis()
		{
			return static_cast<BaseT*>(this);
		}
		const BaseT* GetThis() const
		{
			return static_cast<const BaseT*>(this);
		}

	public:
		KxApp()
		{
			wxInitAllImageHandlers();
		}
		virtual ~KxApp()
		{
		}

	protected:
		virtual int OnRun() override
		{
			int retCode = BaseT::OnRun();
			return m_IsReturnCodeSet ? m_ReturnCode : retCode;
		}

	public:
		bool ParseCommandLine()
		{
			return KxAppInternal::ParseCommandLine(m_CmdLineParser);
		}
		const wxCmdLineParser& GetCmdLineParser() const
		{
			return m_CmdLineParser;
		}
		wxCmdLineParser& GetCmdLineParser()
		{
			return m_CmdLineParser;
		}

		wxString GetAppVersion() const
		{
			return m_Version;
		}
		void SetAppVersion(const wxString& version)
		{
			m_Version = version;
		}

		virtual void ExitApp(int retCode = 0)
		{
			m_ReturnCode = retCode;
			m_IsReturnCodeSet = true;
			wxExit();
		}
};
