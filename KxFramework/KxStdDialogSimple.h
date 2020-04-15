#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStdDialog.h"

class KX_API KxStdDialogSimple: public KxStdDialog
{
	private:
		wxWindow* m_View = nullptr;

	private:
		virtual wxOrientation GetWindowResizeSide() const
		{
			return wxBOTH;
		}

	public:
		static const long DefaultStyle = KxStdDialog::DefaultStyle;

		KxStdDialogSimple() {}
		bool Create(wxWindow* parent,
					wxWindowID id,
					wxWindow* pView,
					const wxString& caption,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					StdButton buttons = DefaultButtons,
					long style = DefaultStyle
		);
		virtual ~KxStdDialogSimple();

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
		wxDECLARE_DYNAMIC_CLASS(KxStdDialogSimple);
};
