#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxTopLevelWindow.h"
#include "Kx/UI/StdDialogButtonSizer.h"
#include "Kx/UI/StdButton.h"

class KX_API KxButton;
class KX_API KxDialog: public KxTopLevelWindow<wxDialog>
{
	public:
		static void SetStdLabels(wxSizer* sizer);

	public:
		static const long DefaultStyle = wxDEFAULT_DIALOG_STYLE|wxFRAME_SHAPED|wxRESIZE_BORDER;

		KxDialog() = default;
		KxDialog(wxWindow* parent,
				 wxWindowID id,
				 const wxString& title,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize,
				 long style = DefaultStyle
		)
		{
			Create(parent, id, title, pos, size, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = DefaultStyle
		);
		virtual ~KxDialog();

	public:
		virtual void CenterIfNoPosition(const wxPoint& pos);
		KxFramework::UI::StdDialogButtonSizer* CreateStdDialogButtonSizer(KxFramework::StdButton buttons, KxFramework::StdButton defaultButtons = KxFramework::StdButton::None);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDialog);
};
