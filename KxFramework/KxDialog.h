#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxTopLevelWindow.h"

class KxButton;
class KxStdDialogButtonSizer;
class KxDialog: public KxTopLevelWindow<wxDialog>
{
	public:
		static void SetStdLabels(wxSizer* sizer);

	public:
		static const long DefaultStyle = wxDEFAULT_DIALOG_STYLE|wxFRAME_SHAPED|wxRESIZE_BORDER;

		KxDialog() {}
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
		KxStdDialogButtonSizer* CreateStdDialogButtonSizer(int buttons);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxDialog);
};
