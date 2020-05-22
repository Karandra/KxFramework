#pragma once
#include "Kx/UI/Common.h"
#include "Kx/UI/Windows/TopLevelWindow.h"
#include "Kx/UI/StdDialogButtonSizer.h"
#include "Kx/UI/StdButton.h"
#include <wx/dialog.h>

namespace KxFramework::UI
{
	enum class DialogStyle
	{
		None = 0,

		Shaped = wxFRAME_SHAPED,
		NoParent = wxDIALOG_NO_PARENT,

		DefaultStyle = wxDEFAULT_DIALOG_STYLE
	};
	enum class DialogExStyle
	{
		None = 0,

		ContextHelp = wxDIALOG_EX_CONTEXTHELP,
		Metal = wxDIALOG_EX_METAL,
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::DialogStyle);
	Kx_EnumClass_AllowEverything(UI::DialogExStyle);
}

namespace KxFramework::UI
{
	class KX_API Dialog: public TopLevelWindow<wxDialog>
	{
		public:
			static constexpr DialogStyle DefaultStyle = EnumClass::Combine<DialogStyle>(DialogStyle::DefaultStyle|DialogStyle::Shaped, TopLevelWindowStyle::Resizeable);

		public:
			static void SetStdLabels(wxSizer* sizer);

		public:
			Dialog() = default;
			Dialog(wxWindow* parent,
				   wxWindowID id,
				   const String& title,
				   const Point& pos = Point::UnspecifiedPosition(),
				   const Size& size = Size::UnspecifiedSize(),
				   DialogStyle style = DefaultStyle
			)
			{
				Create(parent, id, title, pos, size, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& title,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						DialogStyle style = DefaultStyle
			);

		public:
			virtual void CenterIfNoPosition(const Point& pos);
			StdDialogButtonSizer* CreateStdDialogButtonSizer(StdButton buttons, StdButton defaultButtons = StdButton::None);

		public:
			wxDECLARE_DYNAMIC_CLASS(Dialog);
	};
}
