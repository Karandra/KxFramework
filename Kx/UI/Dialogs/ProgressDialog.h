#pragma once
#include "Kx/UI/Common.h"
#include "StdDialog.h"
#include "Kx/UI/Controls/ProgressBar.h"

namespace KxFramework::UI
{
	class KX_API ProgressDialog: public StdDialog, public ProgressBarWrapper
	{
		public:
			static constexpr int DefaultProgressWidth = 300;
			static constexpr int DefaultProgressHeight = 16;
		
		private:
			ProgressBar* m_View = nullptr;

		private:
			wxOrientation GetViewLabelSizerOrientation() const override;
			bool IsEnterAllowed(wxKeyEvent& event, wxWindowID* idOut = nullptr) const override;

		public:
			ProgressDialog() = default;
			ProgressDialog(wxWindow* parent,
						   wxWindowID id,
						   const String& caption,
						   const wxPoint& pos = wxDefaultPosition,
						   const wxSize& size = wxDefaultSize,
						   StdButton buttons = DefaultButtons,
						   DialogStyle style = DefaultStyle
			)
			{
				Create(parent, id, caption, pos, size, buttons, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						StdButton buttons = DefaultButtons,
						DialogStyle style = DefaultStyle
			);

		public:
			ProgressBar* GetProgressBar() const
			{
				return m_View;
			}
			ProgressBar* GetDialogMainCtrl() const override
			{
				return GetProgressBar();
			}
			int GetViewSizerProportion() const override
			{
				return 0;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ProgressDialog);
	};
}
