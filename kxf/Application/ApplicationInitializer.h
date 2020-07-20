#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include "kxf/System/Common.h"

// This class can be used to perform full initialize-run-shutdown sequence of the application.
// Use as shown:
/*
	int main(int argc, char** argv)
	{
		MyAppNamespace::MyApp app;
		return kxf::ApplicationInitializer(app, argc, argv).Run();
	}
*/

// Useful MSVC linker config when using this method:
/*
	/SUBSYSTEM:windows
	/ENTRY:mainCRTStartup
*/

namespace kxf
{
	class ApplicationInitializer final
	{
		private:
			ICoreApplication& m_App;
			bool m_IsInitializedCommon = false;
			bool m_IsInitialized = false;
			bool m_IsCreated = false;

		private:
			bool OnInitCommon();
			bool OnInit();
			bool OnInit(int argc, char** argv);
			bool OnInit(int argc, wchar_t** argv);
			void OnInitDone();

			template<class... Args>
			void RunInitSequence(Args&&... arg)
			{
				if (m_IsInitializedCommon = OnInitCommon())
				{
					if (m_IsInitialized = OnInit(std::forward<Args>(arg)...))
					{
						if (m_IsCreated = m_App.OnCreate())
						{
							OnInitDone();
						}
					}
				}
			}
			
			void OnTerminate();

		public:
			ApplicationInitializer(ICoreApplication& app)
				:m_App(app)
			{
				RunInitSequence();
			}
			ApplicationInitializer(ICoreApplication& app, int argc, char** argv)
				:m_App(app)
			{
				RunInitSequence(argc, argv);
			}
			ApplicationInitializer(ICoreApplication& app, int argc, wchar_t** argv)
				:m_App(app)
			{
				RunInitSequence(argc, argv);
			}
			~ApplicationInitializer()
			{
				OnTerminate();
			}

		public:
			bool IsInitialized() const noexcept
			{
				return m_IsInitializedCommon && m_IsInitialized;
			}
			bool IsCreated() const noexcept
			{
				return m_IsCreated;
			}

			int Run() noexcept;
	};
}
