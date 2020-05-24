#pragma once
#include "Common.h"
#include "String.h"
#include "Version.h"
#include <wx/app.h>
#include <wx/cmdline.h>

namespace kxf
{
	template<class TDerived, class TBase>
	class BasicApplication: public TBase
	{
		public:
			static TDerived& GetInstance()
			{
				return *static_cast<TDerived*>(wxAppConsole::GetInstance());
			}

		private:
			wxCmdLineParser m_CmdLineParser;
			Version m_Version;
			std::optional<int> m_ExitCode;

		private:
			TDerived& Self()
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& Self() const
			{
				return static_cast<const TDerived&>(*this);
			}

		public:
			BasicApplication()
			{
				wxInitAllImageHandlers();
			}

		protected:
			int OnRun() override
			{
				const int exitCode = TBase::OnRun();
				return m_ExitCode.value_or(exitCode);
			}

		public:
			bool ParseCommandLine()
			{
				m_CmdLineParser.SetCmdLine(this->argc, this->argv);
				return m_CmdLineParser.Parse() == 0;
			}
			const wxCmdLineParser& GetCmdLineParser() const
			{
				return m_CmdLineParser;
			}
			wxCmdLineParser& GetCmdLineParser()
			{
				return m_CmdLineParser;
			}

			Version GetAppVersion() const
			{
				return m_Version;
			}
			void SetAppVersion(const Version& version)
			{
				m_Version = version;
			}

			std::optional<int> GetExternalExitCode() const
			{
				return m_ExitCode;
			}
			virtual void ExitApp(int exitCode = 0)
			{
				m_ExitCode = exitCode;
				wxExit();
			}
	};
}
