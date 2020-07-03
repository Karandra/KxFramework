#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/Windows/TopLevelWindow.h"
#include "kxf/UI/StdDialogButtonSizer.h"
#include "kxf/UI/StdButton.h"
#include <wx/dialog.h>

namespace kxf::UI
{
	enum class DialogStyle: uint32_t
	{
		None = 0,

		Shaped = wxFRAME_SHAPED,
		NoParent = wxDIALOG_NO_PARENT,

		DefaultStyle = wxDEFAULT_DIALOG_STYLE
	};
	enum class DialogExStyle: uint32_t
	{
		None = 0,

		ContextHelp = wxDIALOG_EX_CONTEXTHELP,
		Metal = wxDIALOG_EX_METAL,
	};
}
namespace kxf
{
	Kx_DeclareFlagSet(UI::DialogStyle);
	Kx_DeclareFlagSet(UI::DialogExStyle);
}

namespace kxf::UI
{
	class KX_API Dialog: public TopLevelWindow<wxDialog>
	{
		public:
			static constexpr FlagSet<DialogStyle> DefaultStyle = CombineFlags<DialogStyle>(DialogStyle::DefaultStyle|DialogStyle::Shaped, TopLevelWindowStyle::Resizeable);

		public:
			static void SetStdLabels(wxSizer* sizer);

		public:
			Dialog() = default;
			Dialog(wxWindow* parent,
				   wxWindowID id,
				   const String& title,
				   const Point& pos = Point::UnspecifiedPosition(),
				   const Size& size = Size::UnspecifiedSize(),
				   FlagSet<DialogStyle> style = DefaultStyle
			)
			{
				Create(parent, id, title, pos, size, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& title,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<DialogStyle> style = DefaultStyle
			);

		public:
			virtual void CenterIfNoPosition(const Point& pos);
			StdDialogButtonSizer* CreateStdDialogButtonSizer(FlagSet<StdButton> buttons, FlagSet<StdButton> defaultButtons = StdButton::None);

		public:
			wxDECLARE_DYNAMIC_CLASS(Dialog);
	};
}
