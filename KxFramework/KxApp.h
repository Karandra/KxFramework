/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/cmdline.h>

namespace KxAppInternal
{
	KX_API bool ParseCommandLine(wxAppConsole* app, wxCmdLineParser& parser);
}

template<class BaseT = wxApp, class DerivedT = wxApp>
class KxApp: public BaseT
{
	private:
		static inline KxApp* ms_Instance = nullptr;

	public:
		static DerivedT* GetInstance()
		{
			return static_cast<DerivedT*>(ms_Instance);
		}
		static bool HasInstance()
		{
			return ms_Instance != nullptr;
		}

	private:
		wxCmdLineParser m_CmdLineParser;
		wxString m_Version;
		int m_ExitCode = 0;
		bool m_HasExternalExitCode = false;

	private:
		DerivedT* GetThis()
		{
			return static_cast<DerivedT*>(this);
		}
		const DerivedT* GetThis() const
		{
			return static_cast<const DerivedT*>(this);
		}

	public:
		KxApp()
		{
			ms_Instance = this;
			wxInitAllImageHandlers();
		}
		virtual ~KxApp()
		{
			ms_Instance = nullptr;
		}

	protected:
		virtual int OnRun() override
		{
			int exitCode = BaseT::OnRun();
			return m_HasExternalExitCode ? m_ExitCode : exitCode;
		}

	public:
		bool ParseCommandLine()
		{
			return KxAppInternal::ParseCommandLine(this, m_CmdLineParser);
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

		bool HasExternalExitCode() const
		{
			return m_HasExternalExitCode;
		}
		int GetExternalExitCode() const
		{
			return m_ExitCode;
		}
		virtual void ExitApp(int exitCode = 0)
		{
			m_ExitCode = exitCode;
			m_HasExternalExitCode = true;
			wxExit();
		}
};
