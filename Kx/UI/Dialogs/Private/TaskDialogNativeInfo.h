#pragma once
#include "Kx/UI/Common.h"
#include <CommCtrl.h>

namespace KxFramework::UI
{
	class TaskDialog;
}

namespace KxFramework::UI::Private
{
	class KX_API TaskDialogNativeInfo final
	{
		friend class TaskDialog;

		private:
			TaskDialog& m_TaskDialog;
			TASKDIALOGCONFIG m_DialogConfig = {};
			std::vector<TASKDIALOG_BUTTON> m_ButtonSpecs;
			std::vector<TASKDIALOG_BUTTON> m_RadioButtonSpecs;

		private:
			wxNotifyEvent CreateEvent(EventID eventID);
			wxWindowID TranslateButtonIDFromNative(int id) const noexcept;

		public:
			TaskDialogNativeInfo(TaskDialog& dialog);

		public:
			void UpdateBase();
			void UpdateText();
			void UpdateIcons();
			void UpdateButtonSpecs();
			void UpdateStdButtons(FlagSet<StdButton> buttons);
			void UpdateAutoDefaultButton(FlagSet<StdButton> buttons);
			
			void Realize();
			bool UpdateWindowUI(bool sendNavigateEvents = false);
			bool UpdateTextElement(int id, const String& value);

			wxWindowID ShowDialog(bool isModal);
	};
}
