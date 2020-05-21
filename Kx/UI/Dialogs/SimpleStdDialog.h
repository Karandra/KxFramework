#pragma once
#include "Kx/UI/Common.h"
#include "StdDialog.h"

namespace KxFramework::UI
{
	class KX_API SimpleStdDialog: public StdDialog
	{
		private:
			wxWindow* m_View = nullptr;

		private:
			wxOrientation GetWindowResizeSide() const override
			{
				return wxBOTH;
			}

		public:
			SimpleStdDialog() = default;
			bool Create(wxWindow* parent,
						wxWindowID id,
						wxWindow* pView,
						const wxString& caption,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						StdButton buttons = DefaultButtons,
						DialogStyle style = DefaultStyle
			);

		public:
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_View;
			}
			int GetViewSizerProportion() const override
			{
				return 1;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(SimpleStdDialog);
	};
}