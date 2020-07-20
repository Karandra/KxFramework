#pragma once
#include "Common.h"
#include "IGUIApplication.h"
#include "CoreApplication.h"

namespace kxf
{
	class KX_API GUIApplication: public RTTI::ImplementInterface<GUIApplication, CoreApplication, IGUIApplication>
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
			UI::LayoutDirection m_LayoutDirection = UI::LayoutDirection::Default;
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

			// Application::IActiveEventLoop
			bool DispatchIdle() override;

			// Application::IExceptionHandler
			bool OnMainLoopException() override;

			// IGUIApplication
			wxWindow* GetTopWindow() const override;
			void SetTopWindow(wxWindow* window) override;

			bool ShoudExitOnLastFrameDelete() const override
			{
				return m_ExitOnLastFrameDelete == ExitOnLastFrameDelete::Always;
			}
			void ExitOnLastFrameDelete(bool enable = true) override
			{
				m_ExitOnLastFrameDelete = enable ? ExitOnLastFrameDelete::Always : ExitOnLastFrameDelete::Never;
			}
			
			bool IsActive() const override;
			void SetActive(bool active = true, wxWindow* window = nullptr) override;

			UI::LayoutDirection GetLayoutDirection() const override;
			void SetLayoutDirection(UI::LayoutDirection direction) override;

			String GetNativeTheme() const override;
			bool SetNativeTheme(const String& themeName) override;

			using CoreApplication::Yield;
			bool Yield(wxWindow& window, FlagSet<EventYieldFlag> flags) override;
			bool YieldFor(wxWindow& window, FlagSet<EventCategory> toProcess) override;
	};
}
