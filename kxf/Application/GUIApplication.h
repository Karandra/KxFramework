#pragma once
#include "Common.h"
#include "IGUIApplication.h"
#include "CoreApplication.h"

namespace kxf
{
	class KX_API GUIApplication: public RTTI::Implementation<GUIApplication, CoreApplication, IGUIApplication>
	{
		private:
			enum class ExitOnLastFrameDelete
			{
				Never,
				Always,
				Later
			};

		private:
			wxWindow* m_TopWindow = nullptr;
			LayoutDirection m_LayoutDirection = LayoutDirection::Default;
			ExitOnLastFrameDelete m_ExitOnLastFrameDelete = ExitOnLastFrameDelete::Later;
			bool m_IsActive = true;

		protected:
			void DeleteAllTopLevelWindows();

		public:
			GUIApplication() = default;

		public:
			// ICoreApplication
			bool OnCreate() override;
			void OnDestroy() override;
			int OnRun() override;

			// Application::IMainEventLoop
			std::unique_ptr<IEventLoop> CreateMainLoop() override;

			// Application::IActiveEventLoop
			bool DispatchIdle() override;

			// Application::IExceptionHandler
			bool OnMainLoopException() override;

			// IGUIApplication
			wxWindow* GetTopWindow() const override;
			void SetTopWindow(wxWindow* window) override;

			bool ShoudExitOnLastFrameDelete() const override;
			void ExitOnLastFrameDelete(bool enable = true) override;

			bool IsActive() const override;
			void SetActive(bool active = true, wxWindow* window = nullptr) override;

			LayoutDirection GetLayoutDirection() const override;
			void SetLayoutDirection(LayoutDirection direction) override;

			String GetNativeTheme() const override;
			bool SetNativeTheme(const String& themeName) override;

			using CoreApplication::Yield;
			bool Yield(wxWindow& window, FlagSet<EventYieldFlag> flags) override;
			bool YieldFor(wxWindow& window, FlagSet<EventCategory> toProcess) override;
	};
}
