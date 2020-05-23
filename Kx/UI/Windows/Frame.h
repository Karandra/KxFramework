#pragma once
#include "Kx/UI/Common.h"
#include "TopLevelWindow.h"
#include <wx/frame.h>

namespace KxFramework::UI
{
	enum class FrameStyle
	{
		None = 0,

		ToolWindow = wxFRAME_TOOL_WINDOW,
		NoTaskBar = wxFRAME_NO_TASKBAR,
		FloatOnParent = wxFRAME_FLOAT_ON_PARENT,
		Shaped = wxFRAME_SHAPED,

		DefaultStyle = wxDEFAULT_FRAME_STYLE
	};
}
namespace KxFramework
{
	Kx_DeclareFlagSet(UI::FrameStyle);
}

namespace KxFramework::UI
{
	class KX_API Frame: public TopLevelWindow<wxFrame>
	{
		public:
			static constexpr FrameStyle DefaultStyle = FrameStyle::DefaultStyle|FrameStyle::Shaped;

		public:
			Frame() = default;
			Frame(wxWindow* parent,
				  wxWindowID id,
				  const String& title,
				  const Point& pos = Point::UnspecifiedPosition(),
				  const Size& size = Size::UnspecifiedSize(),
				  FlagSet<FrameStyle> style = DefaultStyle
			)
			{
				Create(parent, id, title, pos, size, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& title,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<FrameStyle> style = DefaultStyle
			);
			bool Create(wxWindow* parent,
						wxWindowID id,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<FrameStyle> style = DefaultStyle,
						const String& name = {}
			)
			{
				SetName(name);
				return Create(parent, id, {}, pos, size, style);
			}
		
		public:
			wxDECLARE_DYNAMIC_CLASS(Frame);
	};
}
